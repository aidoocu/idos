
/* Incluir idOS ++ */
#include "../../../src/os/idos.h"
#include <stdio.h>

/* Incluir el coap */
#include "../../apps/coap/coap.h"

/*  */
TASK(task_uno, "primera tarea");

/*  */
TASKS_AUTO_START(&task_uno)


//static uint8_t udp_remote[] = {172, 18, 0, 1};
//bool toggle = false;

TASK_PT(task_uno){

  TASK_BEGIN
    //timer_set(timer_a, 5000);

    udp_listener(listener);
    udp_listener_begin(&listener, COAP_PORT);    

    while (1)
    {

      TASK_YIELD

      /* Si el timer ha expirado, lo reseteamos */
/* 			if (timer_expired(&timer_a)) {
        printf("hello world\n");
			  timer_reset(&timer_a);
			}  */


       
    }
    
  TASK_END
}