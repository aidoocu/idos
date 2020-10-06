
#include "idOS/idos.h"

time_usec_t time_usec;


TASK(hola_mundo, "descripcion_task");


TASKS_AUTO_START(&hola_mundo)

/* Hilo hola */

TASK_PT(hola_mundo){


TASK_BEGIN

  Serial.println("H: 1");
  
  //timer_st timer_try;
  //timer_try.rtimer_call = &timer_callback;

  //timer_set(&timer_try, 1000000); //1Hz

  TIMER_COMP = USEC_TO_CLK(1000000L);
  TIMER_TEMP = 0x0000;

  SEI_TIMER();

  Serial.println("H: 2");
  //Serial.println(TIFR1);
  //TASK_YIELD


TASK_END

}

ISR(TIMER1_COMPA_vect){
    Serial.println("-INT-");
    MSG_TIMER_SEND(hola_mundo, NULL);
    CLI_TIMER();
}
/* void timer_callback(void){
  Serial.println(MICROS);
} */



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  idos_init();
  task_autostart(tasks_auto_start);



}

void loop() {
  // put your main code here, to run repeatedly:

  //Serial.println("--H--");

  schedule();


}