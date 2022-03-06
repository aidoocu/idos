/** 
 * 
 * 
 * 
 *  \date diciembre 2021
 */

#include "../../../idos.h"
#include "tcp.h"

/* Puntero al primer server creado */
struct tcp_listener_st * tcp_listeners = NULL;

void tcp_listener_begin(tcp_listener_st * listener, uint16_t port) {

    /** Crear una escucha de TCP por port 
     * \note uIP recive el puerto (y el resto) leyendo el byte alto primero, así es 
     *      necesario hacer lo mismo cuando le pasemos el puerto. Ejemplo, 80 en uint16_t
     *      es 0x0050, uip lee el frame como 0x5000, ip_htons cambia de lugar los bytes 
    */
    #ifdef UIP_STACK
	uip_listen((u16_t)ip_htons(port));
	#endif /* UIP_STACK */

    listener->port = port;
    listener->msg_len_in = listener->msg_len_out = 0;

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
	#ifdef UIP_STACK
    uip_unlisten(port);
    listener.state = false;
	#endif /* UIP_STACK */
}



bool tcp_open(tcp_listener_st * listener, ip_address_t ip_addr, uint16_t port){

	/* Creamos una conexión y una dirección tipo uIP  */
	struct uip_conn * conn;
	ipaddr_t uip_addr;
  	//static ipaddr_t addr;

	#ifdef UIP_STACK
	/* Se convierte IP en uIP */
	uip_ip_addr(uip_addr, ip_addr);

	/* uIP solicita una conexión al remoto */
	conn = uip_connect(&uip_addr, ip_htons(port));

	/* Si la conexión se pudo solicitar... */
	if(conn){

		/* Poniendo el estado a conectando */
		listener->state = LISTENER_CONNECTING;

		printf("Connecting...\n");

		/* Asignamos el listener a la conexión que se intenta establecer */
		conn->appstate = listener;
		return true;
	}
	#endif /* UIP_STACK */

	
	/* Si no se pudo solicitar la conexión */
	return false;
}

/* --------------------------------------------------------------------------------- */

bool tcp_connected(tcp_listener_st * listener){
	if(listener->state == LISTENER_CONNECTED)
		return true;
	return false;
}

/* --------------------------------------------------------------------------------- */

uint16_t tcp_recv(tcp_listener_st * listener) {

	/* Si no se ha enviado un mensaje es que no hay datagrama */
	if (listener->ipc_msg.status == MSG_NULL)
		return 0;

	/* Se marcas el mensaje como leido */
	listener->ipc_msg.status = MSG_NULL;

	/* y se devuelve el tamaño del mensaje */
	return listener->msg_len_in;
}

/* --------------------------------------------------------------------------------- */

bool tcp_send(tcp_listener_st * listener, uint8_t * msg, uint16_t len) {

	if (listener->msg_len_out == 0) {

        if (len > MAX_TCP_MSG_SIZE) {
            //Aquí hay que hacer algo, picar el mensaje, denegarlo...
            ;
            //por ahora solo le limitaremos el tamanno pero no es correcto
            len = MAX_TCP_MSG_SIZE;
        }

        listener->msg_len_out = len;
	    memcpy(listener->net_msg_out, msg, len);
        return true;
    } 

    return false;
}

/* ------------------------------ STACK UIP ------------------------------ */

#ifdef UIP_STACK

/* --------------------------------------------------------------------------------- */

/** 
 *   
 *  \brief Callback de uIP para tcp 
 *  \details Esta función es el puente entre uIP y las tareas. Esta función es llamada
 * 	por uIP siempre que exista una conexión para situarle a las tareas resultados de 
 * 	sus operaciones y para ejecutar acciones referidas por las tareas. El listener
 * 	es la estructura compartida entre una tarea y la conexión asociada. 
 * 	A esta función solo se entra en caso de una conexión establecida por uIP. Primero
 *  se verifica si esta conexión (uip_conn) ya está asociada a un listerner 
 * 	(uip_conn->appstate != NULL) y de no ser así se busca que listener escucha por el 
 *  y se asocia.
 */
void uipclient_appcall(void) {

    uint16_t send_len = 0;

	/* Creamos un listener temporal */
    struct tcp_listener_st * listener = (tcp_listener_st * )uip_conn->appstate;

    /* Si es una conexión nueva y aún desasociada */
    if(uip_connected()) {

		if (listener != NULL) {
			/* Si es una conexión solicitada al extremo, llegar aquí significa que ha aceptado... */
			if(listener->state == LISTENER_CONNECTING) {
				
				/* por lo que ya estamos efectivamente conectados como clientes */
				listener->state = LISTENER_CONNECTED;


				/* ¡¡¡¡ hay que ver por cual puerto escucha el conn ??? y pasarlo a listener ??? */
				#if NET_DEBUG >= 3 
				printf("Connecting as client\r\n");
				#endif

				/* Notificamos a la tarea */
				ipc_msg_net(listener, NET_MSG_CONNECTED);
			}
		} else { /* En este punto se trata de una conexión solicitada por el extremo */
			/* Cargamos al listener con el principio de la lista de los listeners... */
			listener = tcp_listeners;
			/* ...y buscamos en la lista de listeners quien está escuchando por el puerto */
			do {
				if (ip_htons(listener->port) == uip_conn->lport) 
					break;
				listener = listener->next;
			} while(listener->next != NULL);

			#if NET_DEBUG >= 3
		printf("income tcp connection");
			#endif

			/* entonces ya estamos conectados como servidores */
			uip_conn->appstate = listener;
			listener->state = LISTENER_CONNECTED;
		}    
    }

	/** 
	 * 	Si tenemos una conexión activa puede que la tarea asociada a la conexión le solicite a uIP:
	 * 	- Detener la conexión (no cerrarla) (listener->state = LISTENER_STOP)
	 * 	- Reiniciar la conexión (listener->state & LISTENER_RESTART)
	 * 	- Cerrar la conexión (listener->state & LISTENER_DISCONNECT)
	 *   Por otra parte uIP informe que:
	 * 	- Se ha recibido un dato nuevo (uip_newdata()) - uip_buf tiene el dato de tamaño uip_len
	 * 	- La conexión ha sido o cerrada por el cliente o timeout (uip_closed() || uip_timedout())
	 *  - El extremo notifica la llegada del datagrama (uip_acked())
	 *  - uIP encuesta a la tarea si hay algo que enviar (uip_poll()) o retransmitir (uip_rexmit())
	 * 	  en caso de ser necesario.
	 */
    if (listener != NULL) {

		/* El app indica detener la conexión */
		if (listener->state == LISTENER_STOP) {
                
			#if NET_DEBUG >= 3
		printf("stop connection\r\n");
			#endif

			uip_stop();
			listener->state = LISTENER_STOPED;

			return;
		}

        /* La tarea pide reiniciar la conexión */
        if (listener->state == LISTENER_RESTART) {

			/* Si efectivamente la conexión está detenida */
			if (uip_stopped(uip_conn)) {

				#if NET_DEBUG >= 3
			printf("Conn restart\r\n");
				#endif

				listener->state = LISTENER_CONNECTING;
				uip_restart();
				
				return;
			}
			/// Si no está detenida tiene que pasar algo
			return;
        }
        
        /* La tarea pide cerrar la conexión */
        if (listener->state == LISTENER_DISCONNECT) {
            
            #if NET_DEBUG >= 3
         printf("app close connection: ");
            #endif

			/* liberamos el lisneter */
			listener->state = LISTENER_OFFLINE;
			uip_conn->appstate = NULL;
			/* cerramos la conexión */
			uip_close();

			#if NET_DEBUG >= 3
		printf("close\r\n");
			#endif

			return;        
        }

		/* Se ha recibido un dato nuevo  */
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

            #if NET_DEBUG >= 3
         printf("new data\n\r");
            #endif       

			/* Preparar el listener con el mensaje pasándole la longitud que no puede ser mayor que MAX_TCP_MSG_SIZE */
			/* !!!! hay que ver que hacer cuando el paquete es más que el buffer del listener, lo loogico seriia que 
			el mensaje fuera descartado y solicitado reembiio ¡¡¡¡ */
			if (uip_len > MAX_TCP_MSG_SIZE)
				listener->msg_len_in = MAX_TCP_MSG_SIZE;
				// dropped!!!!!
			else
				listener->msg_len_in = uip_len;
			
			/** y el contenido de uip_buf a partir del mensaje (UIP_LLH_LEN + UIP_TCPIP_HLEN) */
			memcpy(listener->net_msg_in, &uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN], (int)listener->msg_len_in);

			/* Pasarle un mensaje a la tarea notificando la recepción del mensaje */
			ipc_msg_net(listener, NET_MSG_RECEIVED);

			return;
        } 

        /* La conexión ha sido cerrada por el cliente o ha sido timeout */
        if (uip_closed() || uip_timedout()) {
            
            #if NET_DEBUG >= 3
         printf("client closed: ");
            #endif

			/* Le informamos a la tarea del cierre de la conexión */
			listener->state = LISTENER_OFFLINE;
            ipc_msg_net(listener, NET_MSG_CLOSED);

            /* Desasociar appdata */
            uip_conn->appstate = NULL;
            
			return;
        }

        /* El extremo notifica la llegada del datagrama */
        if (uip_acked()) {

            #if NET_DEBUG >= 3
         printf("uip_acked\r\n");
            #endif

            /* Limpio el buffer de salida */
            listener->msg_len_out = 0;

            /* Notifico a la tarea */
            ipc_msg_net(listener, NET_MSG_ACKED);

            return;
            
        }

        /* Aquí uIP espera encontrar datos nuevos a enviar o retransmitir según sea el caso */
        if (uip_poll() || uip_rexmit()) {
        
            #if NET_DEBUG >= 3
         if(uip_poll()) 
                printf("poll\r\n");
            else
                printf("rexmit\r\n");
            #endif

            /* Si hay algo en el buffer de salida del listener */
            if (listener->msg_len_out && (listener->msg_len_out <= MAX_TCP_MSG_SIZE)) {

                /** Según la documentación de uIP:
                 * If the application wishes to send any data, this data should be
                 * put into the uip_appdata and the length of the data should be
                 * put into uip_len. If the application don't have any data to
                 * send, uip_len must be set to 0. 
                 * */
                send_len = listener->msg_len_out;
                uip_appdata = &listener->net_msg_out[0];

				uip_send(uip_appdata, send_len);
    			uip_len = send_len;

				return;
            }
        }
    }
}

/* --------------------------------------------------------------------------------- */

#endif /* UIP_STACK */

