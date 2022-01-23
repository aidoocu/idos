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

TASK_PT(task_uno){

  TASK_BEGIN
    /* Seteamos el timer para creat un pequeño retardo */
    timer_set(timer_a, 1000);

    static uint8_t tcp_server[] = {172, 18, 0, 1};

    tcp_listener(escucha);

    while (1)
    {



      TASK_YIELD

      /* Si el timer ha expirado... */
			if (timer_expired(&timer_a)) {

        /* es necesario desabilitarlo para que no sea más tenido en cuenta 
        aunque en pos del ejemplo podemos hacer un delay() aquí */
        timer_disable(&timer_a);

        printf("hello world\n");
			  //timer_reset(&timer_a);
        tcp_open(&escucha,tcp_server, HTTP_PORT);
			}
    
      /* Si se ha recibido algo por el listener tcp */
      if (tcp_connected(&escucha)) {
        printf("Conectado desde la app\n\r");

        uint8_t saludo[] = {"hola server"};
        tcp_send(&escucha, &saludo[0], sizeof(saludo) - 1);     
      }
      if (tcp_recv(&escucha)){
        printf("Rec: %s\n", escucha.net_msg_in);
        tcp_conn_close(escucha);
      }    
    }
    
  TASK_END
}