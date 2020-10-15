
#include "idOS/idos.h"

//TIMER_INIT (etimer);

static volatile bool toggle = 0; 

TASK(hola_mundo, "descripcion_task");

void rtimer_fun(void){

  digitalWrite(9, ~toggle);

  task_set_ready(&hola_mundo);
}

//RTIMER_INIT(rtimer, rtimer_fun);


TASKS_AUTO_START(&hola_mundo)

/* Hilo hola */

TASK_PT(hola_mundo){


TASK_BEGIN

  //TIMER_SET(rtimer, 100);
  toggle = ~toggle;
  //TASK_YIELD


TASK_END

}

