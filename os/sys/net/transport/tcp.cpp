/** 
 * 
 * 
 * 
 *  \date diciembre 2021
 */

#include "tcp.h"

/* Puntero al primer server creado */
struct tcp_listener_st * tcp_listeners = NULL;

void tcp_listener_begin(tcp_listener_st * listener, uint16_t port, task_st * task) {

    /** Crear una escucha de TCP por port 
     * \note uIP recive el puerto (y el resto) leyendo el byte alto primero, así es 
     *      necesario hacer lo mismo cuando le pasemos el puerto. Ejemplo, 80 en uint16_t
     *      es 0x0050, uip lee el frame como 0x5000, htons cambia de lugar los bytes 
    */
    uip_listen((u16_t)htons(port));

    listener->port = port;
    listener->task = task;
    listener->msg_len_in = listener->msg_len_out = 0;

    /* Este puerto está escuchando */
    listener->state = LISTENER_LISTENING;
    /* Este listener acaba de llegar por lo tanto es el último */
    listener->next = NULL;
    /* Si es el primero, tcp_servers lo apuntará */
    if(tcp_listeners == NULL) {
        tcp_listeners = listener;
    } else {
        /* Si no es el primero hay que buscar el último y que este lo apunte */
        tcp_listener_st * tcp_listener_index = tcp_listeners;
        while (tcp_listener_index->next != NULL) {
            tcp_listener_index = tcp_listener_index->next;
        }
         tcp_listener_index->next = listener;
    }

}

/** 
 * 
 */
void tcp_listener_end(tcp_listener_st listener, uint16_t port) {

    uip_unlisten(port);
    listener.state = false;


}

/** 
 * 
 */
uint8_t tcp_read(tcp_listener_st * listener) {

    if (listener->ipc_msg.status == MSG_NULL)
        return 0;

    listener->ipc_msg.status = MSG_NULL;
    return listener->ipc_msg.event;  
}


/** 
 * 
 */
bool tcp_write(struct tcp_listener_st * listener, uint8_t * buffer, uint16_t len) {

	if (listener->msg_len_out == 0) {

        if (len > MAX_NET_MSG_SIZE) {
            //Aquí hay que hacer algo, picar el mensaje, denegarlo...
            ;
            //por ahora solo le limitaremos el tamanno pero no es correcto
            len = MAX_NET_MSG_SIZE;
        }

        listener->msg_len_out = len;
	    memcpy(listener->net_msg_out, buffer, len);
        return true;
    } 
    /* Si algo pasa en el e */
    return false;
}

/* --------------------------------------------------------------------------------- */

/** 
 *   
 *  
 */
void uipclient_appcall(void) {

    uint16_t send_len = 0;

    /* Creamos un puntero al principio de la lista de los listeners... */
    tcp_listener_st * listener = tcp_listeners;
    /* ...y buscamos en la lista de listeners quien está escuchando por el puerto */
    do {
        if (listener->port == uip_conn->lport) 
            ;

    } while(tcp_listeners->next != NULL);

/*     do {
        if (listener->port == uip_conn->lport) 
            break;
        listener = listener->next;
    } while(listener->next != NULL); */

    /* Se verifica que efectivamente exite una conn y el listener está efectivamente activo */
    if(uip_connected() /* && (listener->state & LISTENER_LISTENING) */) {
        
        /* Se ha recibido un dato nuevo. uip_len ahora contiene el largo del net_msg) */
        #if NET_DEBUG >= 2
        printf("tcp conn -> %u\r\n", uip_len);
        #endif

        uip_conn->appstate = listener;
        listener->state |= LISTENER_CONNECTED;
    
    }

    if (listener->state & LISTENER_CONNECTED) {

        printf("state %02X\r\n", listener->state);

        if (uip_newdata()) {
            
            #if NET_DEBUG >= 3
            /* Imprime el dato nuevo */
            if(uip_len) {
                for (int c = 0; c < (int)uip_len; c++) {
                    printf("%c", (char)uip_buf[c + UIP_LLH_LEN + UIP_TCPIP_HLEN]);
                }
            }
            printf("\r\n");
            #endif

            #if NET_DEBUG >= 2
            printf("new data ");
            #endif       

            /* Si el task ha cerrado o el extemo han cerrado la conexión */
            if (uip_len && !(listener->state & (LISTENER_CLOSE | LISTENER_REMOTECLOSED))) {

                #if NET_DEBUG >= 2
                printf("proccess\r\n");
                #endif 

                /* Preparar el listener con el mensaje pasándole la longitud que no puede ser mayor que MAX_NET_MSG_SIZE */
                /* !!!! hay que ver que hacer cuando el paquete es más que el buffer del listener ¡¡¡¡ */
                if (uip_len > MAX_NET_MSG_SIZE)
                    listener->msg_len_in = MAX_NET_MSG_SIZE;
                else
                    listener->msg_len_in = uip_len;
                
                /** y el contenido de uip_buf a partir del mensaje (UIP_LLH_LEN + UIP_TCPIP_HLEN) */
                memcpy(listener->net_msg_in, &uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN], (int)listener->msg_len_in);

                /* Pasarle un mensaje a la tarea notificando la recepción del mensaje */
                ipc_msg_net(listener, NET_MSG_RECEIVED);

            } else {
                
                #if NET_DEBUG >= 2
                printf("droped\r\n");
                #endif 

                //packet_state &= ~UIPETHERNET_FREEPACKET;
                uip_stop();

                goto finish;
            }
        } 

        /* Si el task pide reiniciar la conexión */
        if (listener->state & LISTENER_RESTART) {
            
            #if NET_DEBUG >= 2
            printf("Conn restart\r\n");
            #endif

            listener->state &= ~LISTENER_RESTART;
            uip_restart();
            
        }

        /* La conexión ha sido cerrada por el cliente o ha sido timeout */
        if (uip_closed() || uip_timedout()) {
            
            #if NET_DEBUG >= 2
            printf("client closed: ");
            #endif

            /* Si el extremo ha cerrado la conexión pero hay un mensaje pendiente por leer */
            if (listener->msg_len_in != 0) {

                #if NET_DEBUG >= 2
                printf("last message\r\n");
                #endif

                /* Le informamos a la tarea del cierre de la conexión */
                listener->state &= 0xF0;

            } else {

                /* Si no hemos terminado por completo de procesar la conexión */
                listener->state &= 0x0F;
                
                #if NET_DEBUG >= 2
                printf("no more message\r\n");
                #endif

            }
            /* Desasociar appdata */
            uip_conn->appstate = NULL;
            goto finish;
        }

        /* El extremo notifica la llegada del datagrama */
        if (uip_acked()) {

            #if NET_DEBUG >= 2
            printf("uip_acked\r\n");
            #endif

            /* Limpio el buffer de salida */
            listener->msg_len_out = 0;

            /* Notifico a la tarea */
            ipc_msg_net(listener, NET_MSG_ACKED);

            goto finish;
            
        }
        
        /* Si la tarea ha cerrado la conexión */
        if (listener->state & LISTENER_CLOSE) {
            
            #if NET_DEBUG >= 2
            printf("app close connection: ");
            #endif

            /* Si no queda nada pendiente por enviar se cierra por completo la conexión */
            if (listener->msg_len_out == 0) {

                /* liberamos el lisneter */
                listener->state &= 0x0F;
                uip_conn->appstate = NULL;
                /* cerramos la conexión */
                uip_close();

                #if NET_DEBUG >= 2
                printf("close\r\n");
                #endif

            } else {
                /* De quedar algo pendiente se le indica al extremo que la conexión fue interrumpida */
                uip_stop();
                
                #if NET_DEBUG >= 2
                printf("stop\r\n");
                #endif
            }
            
            goto finish;
        
        }

        /* Aquí uip espera encontrar datos nuevos a enviar o retransmitir según sea el caso */
        if (uip_poll() || uip_rexmit()) {
        
            #if NET_DEBUG >= 2
            if(uip_poll()) 
                printf("poll\r\n");
            else
                printf("rexmit\r\n");
            #endif

            /* Si hay algo en el buffer de salida del listener */
            if (listener->msg_len_out && (listener->msg_len_out <= MAX_NET_MSG_SIZE)) {

                /** Según la documentación de uIP:
                 * If the application wishes to send any data, this data should be
                 * put into the uip_appdata and the length of the data should be
                 * put into uip_len. If the application don't have any data to
                 * send, uip_len must be set to 0. 
                 * */
                send_len = listener->msg_len_out;
                uip_appdata = &listener->net_msg_out[0];

            }
        }
    }

    finish:
    uip_send(uip_appdata, send_len);
    uip_len = send_len;

}


int client_connect(ip_address_t ip_dst, uint16_t port_dst) {

    if(!nic_is_active()) {

        #ifdef NET_DEBUGGER_
        printf("NIC is no active\r\n");
        #endif
        return 0;
    
    }

    //stop();

    uip_ipaddr_t uip_dst;
    uip_ip_addr(uip_dst, ip_dst);

    #ifdef NET_DEBUGGER_
    printf ("Try to connect to: %d.%d.%d.%d:%d\r\n", 
                ip_dst[0],
                ip_dst[1],
                ip_dst[2],
                ip_dst[3],
                port_dst);
    #endif

    /* Intenta crear una estructura para la conexión y se guarda en conn */
    struct uip_conn* conn = uip_connect(&uip_dst, htons(port_dst));

    if (conn) {
        #if UIP_CONNECT_TIMEOUT > 0
        int32_t timeout = millis() + 1000 * UIP_CONNECT_TIMEOUT;
        #endif

        while((conn->tcpstateflags & UIP_TS_MASK) != UIP_CLOSED) {
            net_tick();

            if ((conn->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED) {
                
                #ifdef NET_DEBUGGER_
                printf("connected, state: %d\r\n", data->state);
                //printf("first packet: %d\r\n", packets_in[0]);
                #endif

                return 1;
            }
            #if UIP_CONNECT_TIMEOUT > 0
            if (((int32_t)(millis() - timeout)) > 0) {
                conn->tcpstateflags = UIP_CLOSED;

                #ifdef NET_DEBUGGER_
                printf("connection timeout\r\n");
                #endif
                break;
            }
            #endif
        }   
    }
    return 0;
}