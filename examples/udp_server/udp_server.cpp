/** 
 *  Aquí mostramos "hello" por consola a 1Hz.
 */
#include "../../../src/os/idos.h"
#include <stdio.h>

/* Declaro dos protohilos */
TASK(task_uno, "primera tarea");

/* Arranco el primer protohilo al inicio */
TASKS_AUTO_START(&task_uno)

/* Defino el comportamiento de ambos protohilo */

static uint8_t udp_remote[] = {172, 18, 0, 1};
bool toggle = false;

TASK_PT(task_uno){

  TASK_BEGIN
    timer_set(timer_a, 5000);

    udp_listener(listener);
    udp_listener_begin(&listener, COAP_PORT);    

    while (1)
    {

      TASK_YIELD

      /* Si el timer ha expirado, lo reseteamos */
			if (timer_expired(&timer_a)) {
        printf("hello world\n");
			  timer_reset(&timer_a);
			} 

      /* Si udp tiene un mensaje */
      if (udp_recv(&listener)) {

        /* obtenemos el ip y el puerto del remoto que envió el mensaje... */
        ip_address_t ip_remote;
        udp_remote_addr(listener, ip_remote);
        uint16_t remote_port = udp_remote_port(listener);
        /* ... y lo imprimimos */
        printf("from: %d.%d.%d.%d:%d\r\n", 
              ip_remote[0], ip_remote[1], ip_remote[2], ip_remote[3], 
              remote_port);

        /* imprimimos el mensaje */
        printf("%s\n", listener.msg);

        /* Creamos dos mensajes, uno de respuesta y otro libre. Como en cada ciclo 
        de planificador solo se puede enviar solo un mensaje udp, estos se alternan
        cada vez que reciben un mensaje del remoto */

        if(toggle){
          /* En un ciclo de planificador enviamos el mensaje libre */
          uint8_t hola[] = {"libre"};
          udp_send(udp_remote, 3333, hola, sizeof(hola));
        } else{
          /* En otro ciclo de planificador enviamos el mensaje respuesta */
          uint8_t cliente[] = {"respuesta"};
          udp_response(cliente, sizeof(cliente));
        }
        toggle = !toggle;
      }
       
    }
    
  TASK_END
}