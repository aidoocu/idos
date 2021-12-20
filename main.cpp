
/** 
 * *
 */
#include "os/idos.h"

/* Estado del LED */
static bool toggle = 0;

/* Declaro dos protohilos */
TASK(task_uno, "primera tarea");

/* Arranco el primer protohilo al inicio */
TASKS_AUTO_START(&task_uno)

/* Conectar a: */
/* uint8_t ip_server[] = {192, 168, 1, 1};
uint16_t port_server = 80; */

/* Defino el comportamiento de ambos protohilo */

TASK_PT(task_uno){

	/* Comienza la tarea, no debe haber nada escrito antes */
	TASK_BEGIN

		/* Seteo led de placa arduino */
		pinMode(2, OUTPUT);

		/* Seteamos creamos un timer y lo seteamos a 0,5 seg */
		timer_set(timer_a, 500);

		tcp_listener(coap_server, COAP_PORT);
		
        while (1) {

			/* Cedemos la CPU hasta que algún evento despierte a esta tarea */
			TASK_YIELD

            /* Si el timer ha expirado, lo reseteamos */
			if (timer_expired(&timer_a)) {

			    timer_reset(&timer_a);

				toggle = !toggle;
				digitalWrite(2, toggle);
			}


			/** !!!!! solo se puede manejar una única uip_conn por listener !!!!! */
			/** Si nos ha llegado un mensaje desde la red hay que verificar que ocurrió */
            if (tcp_read(&coap_server)) {

				/* Si es un mensaje recibido imprimirlo... */
				if (tcp_msg(coap_server)) {

					printf("Net msg: len %02d -> ", (int)coap_server.msg_len_in);
                	for(int i = 0; i < (int)coap_server.msg_len_in; i++) {
                    	printf("%c", (char)coap_server.net_msg_in[i]);
                	}
					printf("\n\r");
/*                 	for(int i = 0; i < (int)coap_server.msg_len_in; i++) {
                    	printf("%02X", coap_server.net_msg_in[i]);
                	}
					printf("\n\r"); */

					/** Hay que verificar que extremo terminó de transmitir el mensaje, 
					 * 	teniendo en cuenta que el mensaje completo puede ser más largo
					 * 	que el buffer de entrada: ???
					*/
					//if (coap_server.net_msg_in[coap_server.msg_len_in - 1] == '\0') {
						/* ...y responder si no hay nada en el buffer de salida */
						if (coap_server.msg_len_out == 0) {

							const char resp[] = {"respuesta"};
							coap_server.msg_len_out = sizeof(resp) - 1;
							memcpy(coap_server.net_msg_out, resp, coap_server.msg_len_out);
							printf(" resp: %d\r\n", coap_server.msg_len_out);

						} else {
							//Si hay algo en el buffer de salida hay que hacer algo, reitentar
							//en alguun momento
							;
						}
					//}
				}
				/** Si es una confirmación (ack) sacar el mensaje del buffer. */
				if (tcp_ack(coap_server)) {
					/* Verificar cual conexión ha transmitido exitosamente el mensaje */
					//struct uip_conn * conn = (struct uip_conn * )task->msg->data;
					/* Se libera el buffer de salida */
					coap_server.msg_len_out = 0;
					printf("ack, buf_out free\n\r");
					
					/* cerrar la conexión */
					conn_close(coap_server);
							
				}



            }
		}

	/* Finalizamos la tarea, no debe habe nada escrito después */
	TASK_END
}