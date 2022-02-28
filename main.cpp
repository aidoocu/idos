
/* Incluir idOS ++ */
#include "os/idos.h"


/*  */
TASK(task_uno, "primera tarea");

/*  */
TASKS_AUTO_START(&task_uno)

/*  */
#define ULTRASONIC_PIN 5
#define MAX_DISTANCE 200

/* Sonar */
NewPing sonar(ULTRASONIC_PIN, ULTRASONIC_PIN, MAX_DISTANCE);

/*  */
static uint8_t dstc;

/* defiendo los callbacks de los recursos coap */

/*  ---------- Doppler ---------- */

/* GET */
static uint8_t doppler_get(coap_payload_st * payload) {
  
  printf("Doppler GET!!!\n");

    char dstc_char[3];

    sprintf(dstc_char, "%d", dstc);

    payload->send_len = 3;
    memcpy(payload->send, dstc_char, payload->send_len);
 

  return 1;

}



TASK_PT(task_uno){

  TASK_BEGIN
    timer_set(timer_a, 3000);

    udp_listener(listener);
    udp_listener_begin(&listener, COAP_PORT);

    coap_resource_create(doppler, "doppler", NULL);
    coap_resource_activate(&doppler);
    doppler.get = * doppler_get;


    while (1)
    {

      //printf("%s\n", pepe.name);
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