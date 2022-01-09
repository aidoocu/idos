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
    timer_set(timer_a, 1000);

    while (1)
    {
      printf("hello world\n");
      TASK_YIELD
      timer_reset(&timer_a);
    }
    
  TASK_END
}