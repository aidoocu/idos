/** 
 *  En este ejemplo mostramos la multitarea. Se declaran dos pt, "uno" y "dos", ambos 
 *  prácticamente iguales. Los pt tienen un ciclo infinito donde se le pasa un mensaje
 *  al otro pt y luego se va a dormir. Así cada, continuamente, pt despierta al otro
 *  y se va a dormir.
 */
#include "os/idos.h"

/* Declaro dos protohilos */
TASK(task_uno, "primera tarea");
TASK(task_dos, "segunda tarea");

/* Arranco el primer protohilo al inicio */
TASKS_AUTO_START(&task_uno)

/* Defino el comportamiento de ambos protohilo */

TASK_PT(task_uno){

  TASK_BEGIN
    
    while (1)
    {
      printf("uno+\n");
      MSG_TASK_SEND(task_dos, NULL);
      TASK_YIELD
      printf("uno-\n");

    }
    
  TASK_END
}

TASK_PT(task_dos){
  
  TASK_BEGIN
    
    while (1)
    {
      printf("dos+\n");
      MSG_TASK_SEND(task_uno, NULL);
      TASK_YIELD
      printf("dos-\n");
    }
    
  TASK_END
}