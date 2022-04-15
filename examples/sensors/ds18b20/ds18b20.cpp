
/* Incluir idOS ++ */
#include "os/idos.h"
#include <stdio.h>


/*  */
TASK(task_ds18b20, "primera tarea");

/*  */
TASKS_AUTO_START(&task_ds18b20)

OneWire ourWire(D5);
DallasTemperature sensors(&ourWire);
float dallas_temp;

/* Protohilo */
TASK_PT(task_ds18b20){

  	TASK_BEGIN

    /* Timer a 5 segundos */
	timer_set(timer_a, 5000);

	sensors.begin();

    while (1) {

      	TASK_YIELD

      	/* Si el timer ha expirado... */
 		if (timer_expired(&timer_a)) {
 
			/* Se envía el comando para leer la temperatura */
			sensors.requestTemperatures();
			/* Se obtiene la temperatura en ºC */
			dallas_temp= sensors.getTempCByIndex(0);

			printf("Dallas temp: %f\n\r", dallas_temp);

		}
	}

	TASK_END

}