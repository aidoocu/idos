
/** 
 * 
 * 
 * 
 */



#include "tcp_client.h"

/* ---------------------- Private TCP ---------------------- */

#define UIP_TCP_PHYH_LEN UIP_LLH_LEN + UIP_IPTCPH_LEN

uip_user_st * data;

uip_user_st uip_users[UIP_CONNS];


/** 
 * 
 */
void _flush_blocks(mem_handle_t* block) {
    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++) {

        mem_block_free(block[i]);
        block[i] = NOBLOCK;
    
    }
}

/** 
 * 
 */
void _eat_block(mem_handle_t * block) {

/*     #ifdef DEBUGGER
    printf("eatblok(%d): ", * block)
    mem_handle_t * start = block;

    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++) {
        printf("%X ", start[i]);
    }
    //???? Serial.print(F("-> "));
    #endif */

    mem_block_free(block[0]);
    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS - 1; i++) {
        block[i] = block[i+1];
    }
    block[UIP_SOCKET_NUMPACKETS - 1] = NOBLOCK;

/*     #ifdef DEBUGGER
    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++) {
        Serial.print(start[i]);
        Serial.print(F(" "));
    }
    Serial.println();
    #endif  */ 

}

void stop() {
    if (data && data->state) {

        #ifdef NET_DEBUGGER_
        printf("before stop(), with data\r\n");
        //_dumpAllData();
        #endif

        _flush_blocks(&data->packets_in[0]);
        if (data->state & UIP_CLIENT_REMOTECLOSED) {
            data->state = 0;
        } else {
            data->state |= UIP_CLIENT_CLOSE;
        }

        #ifdef NET_DEBUGGER_
        printf("after stop()\r\n");
        //_dumpAllData();
        #endif
    }
    #ifdef NET_DEBUGGER_
    else {
        printf("stop(), data: NULL\r\n");
    }
    #endif

    data = NULL;
    net_tick();
}



/* --------------------- Interface TCP --------------------- */

/** 
 *   
 *  
 */
void uipclient_appcall(void) {

    uint16_t send_len = 0;
    /** appstate es un puntero de tipo void el que vamos a apuntar "casteando" con uip_userdata_st
      * así el appstate "representa" al usuario (app) de esta conexión. Si esta conexión es nueva 
      * su appstate seguirá siendo void y null */
    uip_user_st * uip_user = (uip_user_st *)uip_conn->appstate;

    /* Creamos un puntero al principio de la lista de los listeners */
    tcp_listener_st * listener = tcp_listeners;
    /* y buscamos en la lista de listeners quien está escuchando por el puerto */
    do {
        if (listener->port == uip_conn->lport) 
            //uip_conn->appstate = listener;
            ;

    } while(tcp_listeners->next != NULL);

    /* Si la conexión no está registrada (uip_conn->appstate == NULL), pero efectivamente hay una 
    conexión activa... */
    if (!uip_conn->appstate && uip_connected()) {

        #if NET_DEBUG >= 2
        printf("TCP connected -> ");
        #endif

        /* Buscamos un usuario libre y se asigna a la conexión */
        //uip_user = (uip_user_st*)_allocate_data();
        uip_user = NULL;
        for (uint8_t sock = 0; sock < UIP_CONNS; sock++) {
            uip_user = &uip_users[sock];
            if (!uip_user->state) {
                uip_user->state = sock | UIP_CLIENT_CONNECTED;
                //????
                memset(&uip_user->packets_in[0], 0 , sizeof(uip_user_st) - sizeof(uip_user->state));
            }
        }

        /* Si existe el usuario libre lo asignamos a la conexión actual como uip_conn->appstate */
        if (uip_user) {
            uip_conn->appstate = uip_user;
            #if NET_DEBUG >= 3
            printf("Conn allocated: ");
            #endif
        }
            #if NET_DEBUG >= 3
            else
            printf("Conn allocation failed\r\n");
            #endif  
    }
    
    /** Si la conexión registrada (uip_conn->appstate != NULL) se procede a verificar las distintas banderas 
     * de uip_proccess
    */
    //if (1) {
    if (uip_user) {
        
        /* Se ha recibido un dato nuevo. uip_len ahora contiene el largo del net_msg) */
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

            /* Si efectivamente hay datos y nadie ha cerrado la conexión */
            if (uip_len && !(uip_user->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED))) {

                /* Preparar el listener con el mensaje pasándole la longitud que no puede ser mayor que MAX_NET_MSG_SIZE */
                /* !!!! hay que ver que hacer cuando el paquete es más que el buffer del listener ¡¡¡¡ */
                if (uip_len > MAX_NET_MSG_SIZE)
                    listener->msg_len_in = MAX_NET_MSG_SIZE;
                else
                    listener->msg_len_in = uip_len;
                
                /** y el contenido de uip_buf a partir del mensaje (UIP_LLH_LEN + UIP_TCPIP_HLEN) */
                memcpy(listener->net_msg_in, &uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN], (int)listener->msg_len_in);

                /* Pasarle un mensaje a la tarea notificando la recepción del mensaje */
                ipc_msg_net(NET_MSG_RECEIVED);

                goto finish_newdata;
                
            /* Si ya no quedan mas datos por recibir, se ?detiene tcp_uip? */
            packet_state &= ~UIPETHERNET_FREEPACKET;
            uip_stop();
            
            }
        } 

        finish_newdata:
        if (uip_user->state & UIP_CLIENT_RESTART) {

            uip_user->state &= ~UIP_CLIENT_RESTART;
            uip_restart();
            
        }

        /* La conexión ha sido cerrada por alguien o ha sido timeout */
        if (uip_closed() || uip_timedout()) {
            
            #if NET_DEBUG >= 2
            printf("client closed\r\n");
            #endif

            _flush_blocks(&uip_user->packets_out[0]);
            if (uip_user->packets_in[0] != NOBLOCK) {
                ((uip_userdata_closed_st *)uip_user)->lport = uip_conn->lport;
                uip_user->state |= UIP_CLIENT_REMOTECLOSED;
            } else {
                uip_user->state = 0;
            }
            /* Desasociar appdata */
            uip_conn->appstate = NULL;
            goto finish;
        }

        /* ???? */
        if (uip_acked()) {

            #if NET_DEBUG >= 2
            printf("uip_acked\r\n");
            #endif
            
          _eat_block(&uip_user->packets_out[0]);
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
            
            goto finish;

        }
        
        /* No cerrar la conexión hasta que todos los paquetes hayan sido enviados */ 
        if (uip_user->state & UIP_CLIENT_CLOSE) {
            
            #if NET_DEBUG >= 2
            printf("close state\r\n");
            #endif

            if (uip_user->packets_out[0] == NOBLOCK) {

                uip_user->state = 0;
                uip_conn->appstate = NULL;
                uip_close();

                #if NET_DEBUG >= 3
                printf("no blocks out -> free userdata");
                #endif

            } else {
                uip_stop();
                
                #if NET_DEBUG >= 3
                printf("blocks outstanding transfer -> uip_stop()");
                #endif
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

    stop();

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
                data = (uip_user_st *) conn->appstate;
                
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