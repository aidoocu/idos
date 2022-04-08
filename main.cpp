
/* Incluir idOS ++ */
#include "../../../src/os/idos.h"
#include <stdio.h>


/*  */
TASK(task_uno, "primera tarea");

/*  */
TASKS_AUTO_START(&task_uno)

/*  */
#define ULTRASONIC_PIN 5
#define MAX_DISTANCE 200

/* Sonar */
//NewPing sonar(ULTRASONIC_PIN, ULTRASONIC_PIN, MAX_DISTANCE);

/*  */
static uint8_t dstc;

/* defiendo los callbacks de los recursos coap */

/*  ---------- Doppler ---------- */

/* GET */
static uint8_t doppler_get(coap_payload_st * payload) {
  
  printf("Doppler GET!!!\n");

    char dstc_char[4];

    sprintf(dstc_char, "%d", dstc);

    payload->send_len = 4;
    memcpy(payload->send, dstc_char, payload->send_len);
 

  return 1;

}



TASK_PT(task_uno){

  TASK_BEGIN
    timer_set(timer_a, 5000);

    coap_resource(doppler, "doppler", NULL);
    doppler.get = * doppler_get;

    static ip_address_t ip_remoto = {172, 18, 0, 1};
    coap_link(remoto, ip_remoto, 5683, "hello", __null);

    coap_client(coap_cliente);

    while (1)
    {

      //printf("%s\n", pepe.name);
      TASK_YIELD


      /* Si el timer ha expirado, lo reseteamos */
 			if (timer_expired(&timer_a)) {

         /* Valor del sonar */
         //dstc = sonar.ping_cm();
         dstc = 5;
      
        //printf("Distancia: %d\n", dstc);

        //coap_put_non(&remoto, "hola", 4);
        coap_get(&remoto, &coap_cliente);

        timer_reset(&timer_a);
			}

      if(coap_received(&coap_cliente)){
        printf("%s\n", (char *)(task->msg->data));
      }

    }
    
  TASK_END
}