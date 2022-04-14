
/* Incluir idOS ++ */
#include "os/idos.h"
#include <stdio.h>


/*  */
TASK(task_bmp, "Tarea de BMP180");

/*  */
TASKS_AUTO_START(&task_bmp)

/* Sensor BMP180 */
SFE_BMP180 bmp180;
 
/* Altitud actual en metros */
const double altitudActual = 100;

/* Variables para obtener datos del sensor BMP180 */
uint8_t status;
double temp, pres, altitud, presNivelMar;

/* Protohilo */
TASK_PT(task_bmp) {

  	TASK_BEGIN

    /* Timer a 5 segundos */
	timer_set(timer_a, 5000);

	if(bmp180.begin()){
		printf("bmp180 init\n\r");
	} else {
		printf("bmp180 fail\n\r");		
	}

    while (1) {

      	TASK_YIELD

      	/* Si el timer ha expirado... */
 		if (timer_expired(&timer_a)) {
 
    		/* Inicio de lectura de temperatura. Si estatus es 0 
            es que huvo un fallo al leer la temperatura */
    		status = bmp180.startTemperature();

            /* Si hay un valor en estatus es la cantidad de milisegundos
            que deberá tomar en tener una lectura de temperatura. El valor
            obtenido en las pruebas es 5 ms */
			if(status){

                /* Note que aquí se hace una espera pasiva */
				delay(status);
     			/* Se lee la temperatura en la variable temp */
                if (bmp180.getTemperature(temp)) {
                    /* Si es exitosa la lectura se imprime */
                    printf("temp %f\n", temp);
                }
            }

    		/* Lectura de la presión con oversampling de 3. Status funciona
            de la misma manera que en la temperatura */
    		status = bmp180.startPressure(3);

			if(status){
				delay(status);
     			if(bmp180.getPressure(pres, temp))
    				printf("presión absoluta: %f\n", pres);
			}

            /* Convertir de presión absoluta a presión sobre el nivel del mar
            que es el dato con el que se trabaja en meteorología */
			presNivelMar = bmp180.sealevel(pres, altitudActual);
			printf("presNivelMar: %f\n", presNivelMar);

            /* Estimar la altitud a partir de la presión sobre el nivel del mar */
			altitud = bmp180.altitude(pres, presNivelMar);
			printf("altitud: %f\n", altitud);

			/* Se resetea el time */
        	timer_reset(&timer_a);



		}
	}

	TASK_END

}