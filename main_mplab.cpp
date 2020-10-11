
#include "idOS/idos.h"

TASK(hola_mundo, "description_task");

TASKS_AUTO_START(&hola_mundo)

/* Threat Hello */

TASK_PT(hola_mundo){

TASK_BEGIN

TASK_YIELD

TASK_END

}

