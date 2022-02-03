
/* Incluir idOS ++ */
#include "../../../src/os/idos.h"
#include <stdio.h>


/*  */
TASK(task_uno, "primera tarea");

/*  */
TASKS_AUTO_START(&task_uno)


/*  */
static bool toggle_luz_sala = false;
static bool toggle_luz_cuarto = true;

/* defiendo los callbacks de los recursos coap */

/*  ---------- Sala ---------- */

/* GET */
static uint8_t luz_sala_get(coap_payload_st * payload) {
  
  printf("Luz sala GET!!!\n");

  /* Los dos estados de la luz */
  char on[] = {"ON_SALA"};
  char off[] = {"OFF_SALA"};

  if(toggle_luz_sala){
    payload->send_len = sizeof(on);
    memcpy(payload->send, on, sizeof(on));
  } else {
    memcpy(payload->send, off, sizeof(off));
    payload->send_len = sizeof(off);
  }

  return 1;

}
static uint8_t luz_sala_put(coap_payload_st * payload){

  printf("Luz sala PUT!!!\n");

  /* Verificar que el payload trae el largo correcto */
  if(payload->rcvd_len > 9){
    return 0;
  }

  printf("len %d - %s\n", payload->rcvd_len, payload->rcvd);

  char on[] = {"ON_SALA"};
  char off[] = {"OFF_SALA"};

  /* Si el payload contien ON_SALA, enciendo la sala */
  if(!memcmp(payload->rcvd, on, payload->rcvd_len)){
    toggle_luz_sala = true;
    printf("ON\r\n");
  }

  /* Si el payload contien OFF_SALA, apago la sala */
  if(!memcmp(payload->rcvd, off, payload->rcvd_len)){
    toggle_luz_sala = false;
    printf("OFF\r\n");
  }

  return 1;
}

/*  ---------- Sala ---------- */

/* GET */
static uint8_t luz_cuarto_get(coap_payload_st * payload) {
  
  printf("Luz cuarto!!!\n");

  /* Los dos estados de la luz */
  char on[] = {"ON_CUARTO"};
  char off[] = {"OFF_CUARTO"};

  if(toggle_luz_cuarto){
    payload->send_len = sizeof(on);
    memcpy(payload->send, on, sizeof(on));
  } else {
    memcpy(payload->send, off, sizeof(off));
    payload->send_len = sizeof(off);
  }

  return 1;

}


TASK_PT(task_uno){

  TASK_BEGIN
    //timer_set(timer_a, 5000);

    udp_listener(listener);
    udp_listener_begin(&listener, COAP_PORT);

    coap_resource_create(sala, "sala", NULL);
    coap_resource_activate(&sala);

    coap_resource_create(cuarto, "cuarto", NULL);
    coap_resource_activate(&cuarto);

    coap_resource_create(luz_sala, "luz", &sala);
    coap_resource_activate(&luz_sala);
    luz_sala.get = * luz_sala_get;
    luz_sala.put = * luz_sala_put;

    coap_resource_create(luz_cuarto, "luz", &cuarto);
    coap_resource_activate(&luz_cuarto);
    luz_cuarto.get = * luz_cuarto_get;

    while (1)
    {

      //printf("%s\n", pepe.name);
      TASK_YIELD


      /* Si el timer ha expirado, lo reseteamos */
/* 			if (timer_expired(&timer_a)) {
        printf("hello world\n");
			  timer_reset(&timer_a);
			}  */

    }
    
  TASK_END
}