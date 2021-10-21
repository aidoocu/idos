
#include "idOS/idos.h"

/* Declaro el protohilo */
TASK(hola_mundo, "descripcion_task");

/* Arranco el protohilo al inicio */
TASKS_AUTO_START(&hola_mundo)

/* Defino el comportamiento del protohilo */
TASK_PT(hola_mundo){


TASK_BEGIN

  int num = 9;
  printf("hello, %d", num);

TASK_END

}

