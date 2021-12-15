
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

		tcp_server(80);
		
        while (1) {
			toggle = !toggle;
			digitalWrite(2, toggle);

			/* Cedemos la CPU hasta que algún evento despierte a esta tarea */
			TASK_YIELD

            /* Si el timer ha expirado, lo reseteamos */
			if (task->msg->msg_src == MSG_TIMER)
			    timer_reset(&timer_a);

            if (task->msg->msg_src == MSG_NETWORK) {
                printf("Net msg: len %02d -> ", (int)listener_80.msg_len_in);
                for(int i = 0; i < (int)listener_80.msg_len_in; i++) {
                    printf("%c", (char)listener_80.net_msg_in[i]);
                }

                uint8_t resp[] = {"respuesta"};
                listener_80.msg_len_out = sizeof(resp) - 1;
                memcpy(listener_80.net_msg_out, resp, sizeof(resp) - 1);
                printf(" resp: %d\r\n", sizeof(resp));
            }
		}

	/* Finalizamos la tarea, no debe habe nada escrito después */
	TASK_END
}