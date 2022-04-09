
/* Incluir idOS ++ */
#include "os/idos.h"
#include <stdio.h>


/*  */
TASK(task_uno, "primera tarea");

/*  */
TASKS_AUTO_START(&task_uno)

/*  */
static uint16_t battery_level;

/* defiendo los callbacks de los recursos coap */

/*  ---------- Battery ---------- */

/* GET */
static uint8_t battery_get(coap_payload_st * payload) {
  
	printf("battery GET, value: %u\n", battery_level);

	char dstc_char[6];


    sprintf(dstc_char, "%u", battery_level);

    payload->send_len = 6;
    memcpy(payload->send, dstc_char, payload->send_len);
 
  	return 1;
}



TASK_PT(task_uno){

  	TASK_BEGIN

    timer_set(timer_a, 1000);

    coap_resource(battery, "battery", NULL);
    battery.get = * battery_get;

    while (1) {

      	TASK_YIELD

      	/* Si el timer ha expirado... */
 		if (timer_expired(&timer_a)) {

			/* Se lee el nivel de la batería */
        	battery_level = analogRead(A0);

			/* Se resetea el time */
        	timer_reset(&timer_a);
		}
	}

	TASK_END

}