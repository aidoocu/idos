
/* Incluir idOS ++ */
#include "os/idos.h"


/*  */
TASK(task_sr04, "Tarea de HC-SR04");

/*  */
TASKS_AUTO_START(&task_sr04)

/*  */
#define ULTRASONIC_PIN 5
#define MAX_DISTANCE 200

/* Sonar */
NewPing sonar(ULTRASONIC_PIN, ULTRASONIC_PIN, MAX_DISTANCE);

/* Distancia */
uint8_t dstc;


TASK_PT(task_sr04) {

	TASK_BEGIN

		/* Timer a 5 segundos */
		timer_set(timer_a, 5000);

		while (1) {

			TASK_YIELD

			/* Si el timer ha expirado, lo reseteamos */
 			if (timer_expired(&timer_a)) {

				/* Valor del sonar */
				dstc = sonar.ping_cm();
			
				printf("Distancia: %d\n", dstc);
				timer_reset(&timer_a);
			}

		}
		
	TASK_END
}