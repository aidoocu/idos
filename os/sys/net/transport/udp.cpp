
#include "../../../idos.h"
#include "udp.h"

/* Si UDP está activado */
//#if UIP_UDP

#define UIP_ARPHDRSIZE 42

#ifdef ESP_NET_STACK
struct udp_listener_st * udp_listeners = NULL;		
#endif /* ESP_NET_STACK */

/////// !esto ppuede que este mal definido
static struct udp_sender_st {
	bool send;						/* Actúa como bandera para uipudp_appcall() y como 
									semáforo en udp_sender() cuando la task envía un 
									mensaje a un remoto */
	bool response;					/* Actúa como bandera para uipudp_appcall() y como 
									semáforo en udp_respoce() cuando la task responde un 
									mensaje a un remoto previa pseudoconxión establecida */	
	uint16_t len;					/* Largo del mensaje */
	uint8_t msg[MAX_UDP_MSG_SIZE];	/* Buffer para el mensaje */
} udp_sender;



/** 
 * 
 */
bool udp_listener_begin(udp_listener_st * listener, uint16_t port) {

    /* Como el listener siempre está asociado a la pseudoconexión, no es necesario listarlo */

	#ifdef UIP_STACK
    /* Creamos un puntero temporal y lo inicializamos con una pseudoconexión */
    struct uip_udp_conn * udp_conn = uip_udp_new(NULL, 0);

    if(udp_conn) {

        /* Si la pseudoconexión la asociamos al puerto del listener... */
        uip_udp_bind(udp_conn, ip_htons(port));
        /* y asociamos el listener a la pseudoconexión */
        udp_conn->appstate = listener;
		/* y a su vez asociamos la pseudoconexión al listener */
		//listener->udp_conn = udp_conn;
        /* Como la pseudoconexión será manejada internamente por uIP no es necesario devolver 
        referencia a la task */
        return true;
    }
    return false;

	#endif /* UIP_STACK */

	#ifdef ESP_NET_STACK

	listener->udp_conn.begin(port);
	
	#if NET_DEBUG >= 3
	printf("UDP listener initialized on port %d\n", listener->udp_conn.localPort());
	#endif

	/* En este caso los listeners se listarán pues deberán ser chequeados
	por net_tick en busca de cambios */
    /* Este listener acaba de llegar por lo tanto es el último */
    listener->next = NULL;

    /* Si es el primero, udp_listeners lo apuntará */
    if(udp_listeners == NULL) {
        udp_listeners = listener;
    } else {
        /* Si no es el primero hay que buscar el último y que este lo apunte */
        udp_listener_st * udp_listener_index = udp_listeners;
        while (udp_listener_index->next != NULL) {
            udp_listener_index = udp_listener_index->next;
        }
        udp_listener_index->next = listener;
    }

	#endif /* ESP_NET_STACK */	

	return true;

}


/** 
 * 
 */
bool udp_recv(udp_listener_st * listener){
	if (listener->status == UDP_MSG_IN) {
		/* ???? Se establece el mensaje como leído */
		listener->status = UDP_MSG_READED;
		return true;
	}
	return false;
}


/** 
 * 
 */
bool udp_send(ip_address_t dst_addr, uint16_t port, uint8_t * msg, uint16_t len){

	if(!udp_sender.send && !udp_sender.response){	
		/* Se convierte IP en uIP */

		#ifdef UIP_STACK

		ipaddr_t uip_addr;
		uip_ip_addr(uip_addr, dst_addr);

		/* Creamos un puntero temporal y lo inicializamos con una pseudoconexión */
		struct uip_udp_conn * udp_conn = uip_udp_new(&uip_addr, ip_htons(port));

		if(udp_conn){

			/* Copiamos al sender el mensaje */
			memcpy(udp_sender.msg, msg, len);
			/* Le pasamos al sender el tamaño del mensaje */
			udp_sender.len = len;

			/* Levanto la bandera de send */
			udp_sender.send = true;
			return true;
		}

		#endif /* UIP_STACK */
		
		return true;
	}

	return false;
}

/** 
 * 
 */
bool udp_response_to(udp_listener_st * listener, uint8_t * msg, uint16_t len){

	if (!listener->msg_len_out) {

		/* Copiamos al sender el mensaje */
		memcpy(listener->msg_out, msg, len);
		/* Le pasamos al sender el tamaño del mensaje */
		listener->msg_len_out = len;
		/* y activamos la bandera de respuesta */
		listener->status = UDP_MSG_RESPONSE;

		return true;
	}

	return false;

}

/* ---------------------------- ESP_NET_STACK UIP ---------------------------- */

#ifdef ESP_NET_STACK

void esp_net_udp_appcall(void){

	int udp_packet_size = 0;
	udp_listener_st * listener_index = udp_listeners;

	/* Buscando en todos los listeners si hay algo recibido para notificarlo al app dueña 
	del listener. Note que cada listener se recorre en este ciclo una sola vez, por lo que
	de haber más de un mensaje recibido en un listener en particular, se recibirá y procesará
	en el próximo ciclo */
	while(1) {

		/* ----------------------------------- Receive ---------------------------------- */
		udp_packet_size = listener_index->udp_conn.parsePacket();
	
		/* Si hay algo en el buffer se recibirá y se le notifica a la task */
		if (udp_packet_size) {

			printf("Algo en el buffer de largo %d\n", udp_packet_size);
						
			listener_index->msg_len_in = listener_index->udp_conn.read(listener_index->msg_in, MAX_UDP_MSG_SIZE);

			if (listener_index->msg_len_in > 0){

				listener_index->msg_in[listener_index->msg_len_in] = '\0';
				printf("Leidos %d chars: %s\n", udp_packet_size, listener_index->msg_in);

				/* Anunciamos a la tarea que hay un mensaje nuevo */
				listener_index->status = UDP_MSG_IN;
				
				//listener_index->ripaddr = (uint8_t *)listener_index->udp_conn.remoteIP();
				//listener_index->rport = listener_index->udp_conn.remotePort();
				
				/* Pasarle un mensaje a la tarea notificando la recepción del mensaje */
				udp_ipc(listener_index);
			}
		}

		/* --------------------------------- Send/Respoce ------------------------------- */
		// @@@!!!! aqui hay que identificar o garantizar de alguna manera que efectivamente la respuesta
		// desde este listener y no de otro !!!
		if(listener_index->status == UDP_MSG_RESPONSE){

			printf("Respondiendo a %s:%d desde %d\n", listener_index->udp_conn.remoteIP().toString().c_str(), 
												listener_index->udp_conn.remotePort(),
												listener_index->udp_conn.localPort());

			/* Situar puerto e IP del remoto en el datagrama de respuesta */
			listener_index->udp_conn.beginPacket(listener_index->udp_conn.remoteIP(), 
													listener_index->udp_conn.remotePort());

			/* Pasar el mensaje de respuesta */
			listener_index->udp_conn.write(listener_index->msg_out, listener_index->msg_len_out);
			listener_index->udp_conn.endPacket();

			/* Bajar la bandera */
			listener_index->status = UDP_MSG_NULL;

		}

		/* Si next es NULL implica que ya hemos revisado el último listener y salimos
		del while. No me gusta como queda esto pero :( */
		if(listener_index->next == NULL)
			break;
		
		listener_index = listener_index->next;

	}
}

#endif /* ESP_NET_SATCK */

/* ------------------------------ STACK UIP ------------------------------ */

#ifdef UIP_STACK

/** 
 * 
 */
void uipudp_appcall(void) {

    /* Creamos un listener temporal */
    udp_listener_st * listener = (udp_listener_st * )(uip_udp_conn->appstate);

    #if NET_DEBUG >= 3
    printf("udp appcall\n\r");
    #endif

    /** UDP no crea conexiones, así que tenemos solo dos posibilidades, o ha entrado algo, 
        o la tarea quiere enviar algo. Para ello verificamos si la conexión que se muestrea
        tiene un listener (una task al fin y cabo) asociada, y de ser así se procede */
    if (listener) {

        /* Si hay datos nuevos que procesar */
        if (uip_newdata()) {

			/* uIP no llena los campos del remoto, en su defecto uip_buf carga con el frame 
			completo, así que hay que aisla los headers. Aquí asignamos o actualizamos el ip 
			y el puerto remoto en el listener */
			listener->rport = udp_buf->srcport;
			uip_ipaddr_copy(listener->ripaddr, udp_buf->srcipaddr);


			/* Pasamos el largo del mensaje */
			listener->msg_len_in = ip_htons(udp_buf->udplen) - UIP_UDPH_LEN;

            #if NET_DEBUG >= 3
            printf("-> new data ");
            printf("from: %d.%d.%d.%d:%d ", 
					(uint8_t)(listener->ripaddr[0]), 
					(uint8_t)ip_htons(listener->ripaddr[0]),
					(uint8_t)(listener->ripaddr[1]), 
					(uint8_t)ip_htons(listener->ripaddr[1]),
					ip_htons(listener->rport));
            printf("to local port: %d -> ", ip_htons(uip_udp_conn->lport));
			printf("len: %d\n\r", listener->msg_len_in);
            #endif

			if (listener->msg_len_in) {
				/* Anunciamos a la tarea que hay un mensaje nuevo */
				listener->status = UDP_MSG_IN;

				/* y copiamos el mensaje al buffer del listener */
				memcpy(listener->msg_in, 
						&uip_buf[UIP_UDP_PHYH_LEN], 
						(int)listener->msg_len_in);

				/* Pasarle un mensaje a la tarea notificando la recepción del mensaje */
				udp_ipc(listener);

			} else {
					/* Anunciamos un mensaje vacío */
					listener->status = UDP_MSG_NULL;
			}
		}

		if (uip_poll() && (listener->status == UDP_MSG_RESPONSE)){

			#if NET_DEBUG >= 3
			printf("udp poll response\n\r");
			#endif

			/* Situar puerto e IP del remoto */
			uip_udp_conn->rport = listener->rport;
			uip_ipaddr_copy(uip_udp_conn->ripaddr, listener->ripaddr);

			uip_appdata = &listener->msg_out[0];
			/* set uip_slen (uip private) by calling uip_udp_send */
			uip_udp_send(listener->msg_len_out);

			/* Reset el largo del mensaje de salida para indicar buffer vacío */
			listener->msg_len_out = 0;

		}

	} else {
		/*  Se chequea si la task tiene algo que enviar. Note que siempre se hace poll() porque uIP 
			no tiene la referencia de conexión activa, así que siempre deberá encuestar. El control
			para saber si la tarea tiene algo que enviar lo tiene la variable listener.send */
		if (uip_poll() && udp_sender.send) {

			#if NET_DEBUG >= 3
			printf("udp poll send\n\r");
			#endif

			uip_appdata = &udp_sender.msg[0];
			/* set uip_slen (uip private) by calling uip_udp_send */
			uip_udp_send(udp_sender.len);

			/* Liberar la pseudoconexión */
			uip_udp_remove(uip_udp_conn);
			/* bajar la bandera */
			udp_sender.send = false;
		}
	}

}

#endif /* UIP_STACK */



//#endif /* UIP_UDP */