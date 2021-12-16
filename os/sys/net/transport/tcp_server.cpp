/** 
 * 
 * 
 * 
 *  \date diciembre 2021
 */

#include "tcp_server.h"

/* Puntero al primer server creado */
struct tcp_listener_st * tcp_listeners = NULL;

void tcp_listener_begin(tcp_listener_st * listener, uint16_t port, task_st * task) {

    /** Crear una escucha de TCP por port 
     * \note uip recive el puerto (y el resto) leyendo el byte alto primero, así es 
     *      necesario hacer lo mismo cuando le pasemos el puerto. Ejemplo, 80 en uint16_t
     *      es 0x0050, uip lee el frame como 0x5000, htons cambia de lugar los bytes 
    */
    uip_listen((u16_t)htons(port));

    listener->port = port;
    listener->task = task;
    listener->msg_len_in = listener->msg_len_out = 0;

    /* Este puerto está escuchando */
    listener->listening = true;
    /* Este listener acaba de llegar por lo tanto es el último */
    listener->next = NULL;
    /* Si es el primero, tcp_servers lo apuntará */
    if(tcp_listeners == NULL) {
        tcp_listeners = listener;
    } else {
        /* Si no es el primero hay que buscar el último y que este lo apunte */
        tcp_listener_st * tcp_listener_index = tcp_listeners;
        while (tcp_listener_index->next != NULL) {
            tcp_listener_index = tcp_listener_index->next;
        }
         tcp_listener_index->next = listener;
    }

}

/** 
 * 
 */
void tcp_listener_end(tcp_listener_st listener, uint16_t port) {

    uip_unlisten(port);
    listener.listening = false;

    //Sacarlo de la lista y liberar su memoria si fuera posible

/*   for (uip_userdata_st * data = &all_data[0]; data < &all_data[UIP_CONNS]; data++) {
      if ((data->state & UIP_CLIENT_CONNECTED) && uip_conns[data->conn_index].lport == port) {
          client(data);
          client.stop();
        }
    } */
}