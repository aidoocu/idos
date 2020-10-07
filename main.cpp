
#include "idOS/idos.h"

TIMER_INIT (etimer);


TASK(hola_mundo, "descripcion_task");

void rtimer_fun(void){
  Serial.println("-real-timer-");
  task_set_ready(&hola_mundo);
}

RTIMER_INIT(rtimer, rtimer_fun);


TASKS_AUTO_START(&hola_mundo)

/* Hilo hola */

TASK_PT(hola_mundo){


TASK_BEGIN

  Serial.println("H: 1");


  TIMER_SET(rtimer, 1000000); //1Hz


  Serial.println("H: 2");
  //Serial.println(TIFR1);
  //TASK_YIELD


TASK_END

}


/* ------------------------------- Arduino code ------------------------------ */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  idos_init();
  task_autostart(tasks_auto_start);

}

void loop() {
  // put your main code here, to run repeatedly:

  schedule();

}