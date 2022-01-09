/** 
 *  En este es un ejemplo para Arduino. Aquí mostramos un LED parpadeando 
 *  a 1Hz. Note que la variable toggle ha sido declarada como static. Esto
 *  es debido a que cada que un protohilo cede el CPU se avandona el contexto
 *  y se destruyen todas sus variables. En el caso de toggle (y de todas las)
 *  variables que se espere que sobrevivan a un YIELD deben ser o declaradas
 *  static o declaradas de forma global, por ejemplo justo después de declarar
 *  la tarea
 */
#include "os/idos.h"

/* Declaro dos protohilos */
TASK(task_uno, "primera tarea");

/* Arranco el primer protohilo al inicio */
TASKS_AUTO_START(&task_uno)



/* Defino el comportamiento de ambos protohilo */

TASK_PT(task_uno){

  /* Comienza la tarea, no debe haber nada escrito antes */
  TASK_BEGIN
    /* Seteo led de placa arduino */
    pinMode(LED_BUILTIN, OUTPUT);

    /* Estado del LED */
    static bool toggle = 0;

    /* Seteamos creamos un timer y lo seteamos a 0,5 seg */
    timer_set(timer_a, 500);

    while (1)
    {
      toggle = !toggle;
      digitalWrite(13, toggle);

      /* Cedemos la CPU hasta el expire el timer */
      TASK_YIELD

      /* El timer ha expirado, así que lo reseteamos */
      timer_reset(&timer_a);
    }

  /* Finalizamos la tarea, no debe habe nada escrito después */
  TASK_END
}
