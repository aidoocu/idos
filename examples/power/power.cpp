/** Este ejemplo nos permite ver la diferencia entre una espera pasiva entregando el CPU 
 *  y una activa con delay. Testeado en un Arduino Nano muestra unos 10mA de diferencia
 * */
#include "os/idos.h"

/* Declaro dos protohilos */
TASK(task_uno, "primera tarea");

/* Arranco el primer protohilo al inicio */
TASKS_AUTO_START(&task_uno)



/* Defino el comportamiento de ambos protohilo */

TASK_PT(task_uno){

  /* Comienza la tarea, no debe haber nada escrito antes */
  TASK_BEGIN

    /* Seteamos creamos un timer y lo seteamos a 0,5 seg */
    timer_set(timer_a, 4000);

    while (1)
    {
      /* Cedemos la CPU para espera pasiva */
      TASK_YIELD

      /* Espera activa (CPU encendido y ocupado) */
      delay(4000);

      /* Reseteamos el timer para la próxima espera pasiva*/
      timer_reset(&timer_a);
    }

  /* Finalizamos la tarea, no debe habe nada escrito después */
  TASK_END
}