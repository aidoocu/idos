/** 
 * 
 * 
 *  \date diciembre 2021
 */

#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "../netstack.h"

/** 
 *  Significado de las banderas uip_conn->tcpstateflags:
 *  - UIP_CLOSED: La conexión se ha cerrado de consenso (el extremo ha respondido con un ack al cierre o simplemente
 *              no hay conexión creada. Con esta bandera puesta no se debe llegar aquí porque esto lo chequea net_tick)
 *  - UIP_CLOSING: La app ha cerrado la conexión pero aun no ha tenido confirmación del extremo.
 *  - UIP_STOPPED: La app ha detenido el flujo de datos usando uip_stop() así que no se aceptará datos del extremo.
*/

/* --------------------------------------------------------------------------------- */

/** 
 *  \brief Banderas de estado del listener
 *  \details Se utilizan para que la tarea pase información de estado al appcall
 */
#define LISTENER_LISTENING      0x01        /**< El listener está escuchando */
#define LISTENER_STOP           0x02        /**< El listener no está escuchando */
#define LISTENER_CONNECTED      0x10        /**< Hay una conexion activa en el listener */
#define LISTENER_CLOSE          0x20        /**< El app ha detenido la conexión del listner */
#define LISTENER_REMOTECLOSED   0x40        /**< El extremo ha cerrado la conexión del listner */
#define LISTENER_RESTART        0x80        /**< El app reinicia la conexión del listner */


/** \brief  El total de octetos dedicados a encabezados en la trama 
 *  \note   El mensaje comienza en uip_buf[UIP_LLH_LEN - UIP_TCPIP_HLEN - 1]
 *          y su largo es uip_len - UIP_LLH_LEN - UIP_TCPIP_HLEN
 *          y no será más largo que UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN
*/
#define MAX_NET_MSG_SIZE UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN

/* Algunos puertos estándar */
#define HTTP_PORT 80
#define COAP_PORT 5683

/* --------------------------------------------------------------------------------- */

/** \brief Crear y arrancar el servidor TCP
 *  \details Crea la estructura del listener y llama a la función que arranca el server 
 *  \note Escrito de esta manera no es necesario verificar si el puerto está ocupado pues
 *          se crearán dos variables con el mismo nombre induciendo un error de compilación
 */
#define tcp_listener(listener_name, tcp_port)                   \
            static struct tcp_listener_st listener_name;        \
            tcp_listener_begin(&listener_name, tcp_port, task)


/** \brief Detiene el servidor TCP
 *  \details 
 */
#define tcp_server_end(listener_name)               \
            tcp_listener_end(&listener_name)

/** 
 * \brief Detener la conexión activa que hubiera en el listener 
 */
#define conn_close(listener_name)                   \
    listener_name.state |= LISTENER_CLOSE


/* --------------------------------------------------------------------------------- */

/** \brief Estrucctura de un punto de escucha TCP 
 *  \details Las tareas que deseen escuchar por un puerto TCP porque sean servidores que
 *          o pretendan conectarse a uno externo utilizarán esta estrutura para gestionar 
 *          la escucha. Los servidores TCP se guardan como una lista enlazada.
 */
struct tcp_listener_st {
    uint16_t port;                              /**< Puerto TCP de escucha */
    uint8_t state = 0;                          /**< Banderas de estado de la escucha */        
    tcp_listener_st * next = NULL;              /**< Puntero al próximo listener o a NULL */
    task_st * task;                             /**< Puntero a la tarea que setea el listerner */
    msg_st ipc_msg;                             /**< Mensaje a la tarea que setea el listerner */
    uint16_t msg_len_in;                        /**< Tamaño del mensaje que está en el buffer de entrada */
    uint16_t msg_len_out;                       /**< Tamaño del mensaje que está en el buffer de salida */
    uint8_t net_msg_in[MAX_NET_MSG_SIZE];       /**< Buffer que contendrá el mensaje de la red */
    uint8_t net_msg_out[MAX_NET_MSG_SIZE];      /**< Buffer que contendrá el mensaje de la red */
};

/** 
 * 
 */
extern struct tcp_listener_st * tcp_listeners;

/* --------------------------------------------------------------------------------- */

/** Mensajes desde net */
enum {
   NET_MSG_RECEIVED = 1,         /**< Mensaje recibido desde la red*/
   NET_MSG_SENDED,               /**< Mensaje enviado a la red */
   NET_MSG_ACKED                 /**< Notificación de recepción del extremo */
};

/** 
 * \def   ipc_msg_net
 * \brief Enviar mensaje desde la interface de red hacia una tarea
 * \note  Esta macro solo se debe llamar dentro de net_tick() una vez que se haya 
 *        identificado el tipo de la operación.
 * \param net_event Evento de red 
 */
#define ipc_msg_net(listener, net_event)            \
            do {                                    \
                listener->ipc_msg = {               \
                    MSG_AVAILABLE,                  \
                    MSG_NETWORK,                    \
                    net_event,                      \
                    uip_conn                        \
                };                                  \
                task_set_ready(listener->task);     \
            } while(0)


/**  */
#define tcp_event(listener) listener.ipc_msg.event

/**  */
#define tcp_msg(listener) listener.ipc_msg.event == NET_MSG_RECEIVED
#define tcp_ack(listener) listener.ipc_msg.event == NET_MSG_ACKED

/* --------------------------------------------------------------------------------- */

/** 
 *  \brief Initializar el servidor TCP en el puerto:
 *  \param listener Estructura del servidor TCP
 *  \param port Puerto en el que escucha esta aplicación
 */
void tcp_listener_begin(tcp_listener_st * listener, uint16_t port, task_st * task);

/** 
 *  \brief Detener el servidor TCP en el puerto:
 *  \param listener Estructura del servidor TCP
 *  \param port Puerto en el que escucha esta aplicación
 */
void tcp_listener_end(tcp_listener_st * listener);

/** 
 *  \brief Verifica si ha llegado un mensaje desde la red.
 *  \attention Esta función modifica el mensaje del listener desactivándolo. No debería
 *  hacer dos llamadas consecutivas de esta función a menos que sepa lo que está haciendo 
 *  pues en caso de haber un mensaje la primera devolverá el evento mientras que la segunda
 *  devolverá 0.
 */
uint8_t tcp_read(tcp_listener_st * listener);

#endif /* _TCP_SERVER_H_ */