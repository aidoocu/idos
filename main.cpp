/** 
 *  En este ejemplo mostramos la multitarea. Se declaran dos pt, "uno" y "dos", ambos 
 *  prácticamente iguales. Los pt tienen un ciclo infinito donde se le pasa un mensaje
 *  al otro pt y luego se va a dormir. Así cada, continuamente, pt despierta al otro
 *  y se va a dormir.
 */
#include "idOS/os/idos.h"

/* Declaro dos protohilos */
TASK(task_uno, "primera tarea");

/* Arranco el primer protohilo al inicio */
TASKS_AUTO_START(&task_uno)

/* Defino el comportamiento de ambos protohilo */

TASK_PT(task_uno){

  TASK_BEGIN
    pinMode(13, OUTPUT);
    timer_set(timer_a, 1000);

    while (1)
    {
      printf("hello \n");
      TASK_YIELD
      timer_reset(&timer_a);
    }
    
  TASK_END
}
