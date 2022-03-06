
#include "../../../idos.h"
#include "udp.h"

/* Si UDP está activado */
//#if UIP_UDP

#define UIP_ARPHDRSIZE 42

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
		ipaddr_t uip_addr;

		#ifdef UIP_STACK
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
bool udp_response(uint8_t * msg, uint16_t len){

	if (!udp_sender.send) {

		/* Copiamos al sender el mensaje */
		memcpy(udp_sender.msg, msg, len);
		/* Le pasamos al sender el tamaño del mensaje */
		udp_sender.len = len;

		/* Levanto la bandera de send */
		udp_sender.response = true;
	
		return true;
	}
	return false;
}


/* ------------------------------ STACK UIP ------------------------------ */

#ifdef UIP_STACK

/* --------------------------------------------------------------------------------- */

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
			listener->msg_len = ip_htons(udp_buf->udplen) - UIP_UDPH_LEN;

            #if NET_DEBUG >= 3
            printf("-> new data ");
            printf("from: %d.%d.%d.%d:%d ", 
					(uint8_t)(listener->ripaddr[0]), 
					(uint8_t)ip_htons(listener->ripaddr[0]),
					(uint8_t)(listener->ripaddr[1]), 
					(uint8_t)ip_htons(listener->ripaddr[1]),
					ip_htons(listener->rport));
            printf("to local port: %d -> ", ip_htons(uip_udp_conn->lport));
			printf("len: %d\n\r", listener->msg_len);
            #endif

			if (listener->msg_len) {
				/* Anunciamos a la tarea que hay un mensaje nuevo */
				listener->status = UDP_MSG_IN;

				/* y copiamos el mensaje al buffer del listener */
				memcpy(listener->msg, 
						&uip_buf[UIP_UDP_PHYH_LEN], 
						(int)listener->msg_len);

				/* Pasarle un mensaje a la tarea notificando la recepción del mensaje */
				udp_ipc(listener);

			} else {
					/* Anunciamos un mensaje vacío */
					listener->status = UDP_MSG_NULL;
			}
		}

		if (uip_poll() && udp_sender.response){

			#if NET_DEBUG >= 3
			printf("udp poll response\n\r");
			#endif

			/* Situar puerto e IP del remoto */
			uip_udp_conn->rport = listener->rport;
			uip_ipaddr_copy(uip_udp_conn->ripaddr, listener->ripaddr);

			uip_appdata = &udp_sender.msg[0];
			/* set uip_slen (uip private) by calling uip_udp_send */
			uip_udp_send(udp_sender.len);

			/* Bajar la bandera */
			udp_sender.response = false;

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