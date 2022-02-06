/** 
 *  Aquí mostramos "hello" por consola a 1Hz.
 */
#include "../../../src/os/idos.h"
#include <stdio.h>

/* Declaro dos protohilos */
TASK(task_uno, "primera tarea");

/* Arranco el primer protohilo al inicio */
TASKS_AUTO_START(&task_uno)

/* Defino el comportamiento de ambos protohilo */

TASK_PT(task_uno){

  TASK_BEGIN
    timer_set(timer_a, 5000);

    tcp_listener(escucha);
    tcp_listener_begin(&escucha, COAP_PORT);

    while (1)
    {

      TASK_YIELD

      /* Si el timer ha expirado, lo reseteamos */
			if (timer_expired(&timer_a)) {
        printf("hello world\n");
			  timer_reset(&timer_a);
			}
    
      /* Si se ha recibido algo por el listener tcp */
      if (tcp_recv(&escucha)) {
        printf("Rec: %s\n", escucha.net_msg_in);

        uint8_t resp[] = {"respuesta"};
        printf("resp: %d\r\n", escucha.msg_len_out);
        tcp_send(&escucha, &resp[0], sizeof(resp) - 1);     

      }
      if (tcp_ack(escucha))
        tcp_conn_close(escucha);    
    }
    
  TASK_END
}