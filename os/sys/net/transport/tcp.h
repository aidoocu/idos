/** 
 * 
 * 
 *  \date diciembre 2021
 */

#ifndef _TCP_H_
#define _TCP_H_

#include "../netstack.h"

/** 
 *  Significado de las banderas uip_conn->tcpstateflags:
 *  - UIP_CLOSED (0): La conexión se ha cerrado de consenso (el extremo ha respondido con un ack al cierre o simplemente
 *              no hay conexión creada. Con esta bandera puesta no se debe llegar aquí porque esto lo chequea net_tick)
 *  - UIP_CLOSING (6): La app ha cerrado la conexión pero aun no ha tenido confirmación del extremo.
 *  - UIP_STOPPED (16): La app ha detenido el flujo de datos usando uip_stop() así que no se aceptará datos del extremo
 *                      En este caso la conexión no está cerrada!!
 *  - UIP_ESTABLISHED (3): Esta conexion está activa
 *  - UIP_SYN_RCVD (1): 
 *  - UIP_SYN_SENT (2):  
 *  - UIP_FIN_WAIT_1 (4): 
 *  - UIP_FIN_WAIT_2 (5): 
 *  - UIP_TIME_WAIT (7): 
 *  - UIP_LAST_ACK (8): 
 *  - UIP_TS_MASK (15): 
*/

/* --------------------------------------------------------------------------------- */

/** 
 *  \brief Banderas de estado del listener
 *  \details Se utilizan para que la tarea pase información de estado al appcall. 
 */
#define LISTENER_OFFLINE    0   /**< No hay conexion activa en el listener */
#define LISTENER_CONNECTED  1   /**< Hay una conexion activa en el listener */
#define LISTENER_DISCONNECT 2   /**< El app indica cerrar la conexión */
#define LISTENER_STOP       3   /**< El app indica detener la conexión */
#define LISTENER_STOPED     4   /**< La conexión está detenida */
#define LISTENER_RESTART    5   /**< El app indica reiniciar la conexión detenida previamente */

#define LISTENER_CONNECTING 9   /**< La conexióm está en proceso de conexión o reinicio */


/** \brief  El total de octetos dedicados a encabezados en la trama 
 *  \note   El mensaje comienza en uip_buf[UIP_LLH_LEN - UIP_TCPIP_HLEN - 1]
 *          y su largo es uip_len - UIP_LLH_LEN - UIP_TCPIP_HLEN
 *          y no será más largo que UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN
*/
#define MAX_TCP_MSG_SIZE UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN

/* --------------------------------------------------------------------------------- */

/* Algunos puertos estándar */
#define HTTP_PORT 80
#define COAP_PORT 5683

/* --------------------------------------------------------------------------------- */

/** \brief Estructura de un punto de escucha TCP 
 *  \details Las tareas que deseen escuchar por un puerto TCP porque sean servidores que
 *          o pretendan conectarse a uno externo utilizarán esta estrutura para gestionar 
 *          la escucha. Los servidores TCP se guardan como una lista enlazada.
 */
struct tcp_listener_st {
    uint16_t port;                              /**< Puerto TCP de escucha */
    uint8_t state;                              /**< Banderas de estado de la escucha */        
    tcp_listener_st * next;                     /**< Puntero al próximo listener o a NULL */
    task_st * task;                             /**< Puntero a la tarea que setea el listerner */
    msg_st ipc_msg;                             /**< Mensaje a la tarea que setea el listerner */
    uint16_t msg_len_in;                        /**< Tamaño del mensaje que está en el buffer de entrada */
    uint16_t msg_len_out;                       /**< Tamaño del mensaje que está en el buffer de salida */
    uint8_t net_msg_in[MAX_TCP_MSG_SIZE];       /**< Buffer que contendrá el mensaje de la red */
    uint8_t net_msg_out[MAX_TCP_MSG_SIZE];      /**< Buffer que contendrá el mensaje de la red */
};

/** 
 *  \brief
 */
extern struct tcp_listener_st * tcp_listeners;

/* --------------------------------------------------------------------------------- */

/** \brief Crear y arrancar el servidor TCP
 *  \details Crea la estructura del listener y llama a la función que arranca el server 
 *  \note Escrito de esta manera no es necesario verificar si el puerto está ocupado pues
 *          se crearán dos variables con el mismo nombre induciendo un error de compilación
 */
#define tcp_listener(listener_name)                         \
            static struct tcp_listener_st listener_name;    \
            listener_name.task = task


/** \brief Detiene el servidor TCP
 *  \details 
 */
#define tcp_server_end(listener_name)               \
            tcp_listener_end(&listener_name)

/** 
 *  \brief Initializar el servidor TCP en el puerto:
 *  \param listener Estructura del servidor TCP
 *  \param port Puerto en el que escucha esta aplicación
 */
void tcp_listener_begin(tcp_listener_st * listener, uint16_t port);

/** 
 *  \brief Detener el servidor TCP en el puerto:
 *  \param listener Estructura del servidor TCP
 *  \param port Puerto en el que escucha esta aplicación
 */
void tcp_listener_end(tcp_listener_st * listener);

/* --------------------------------------------------------------------------------- */

/** Mensajes desde net */
enum {
    NET_MSG_CONNECTED = 1,  /**< Conexión aceptada por el extemo */
    NET_MSG_RECEIVED,       /**< Mensaje recibido desde la red */
    NET_MSG_SENDED,         /**< Mensaje enviado a la red */
    NET_MSG_ACKED,          /**< Notificación de recepción del extremo */
    NET_MSG_CLOSED          /**< Notificación de recepción del extremo*/ 
};
////// Esto hay que pasarlo al netstack !!!!!!!
/** 
 * \def   ipc_msg_net
 * \brief Enviar mensaje desde la interface de red hacia una tarea
 * \note  Esta macro solo se debe llamar dentro de net_tick() una vez que se haya 
 *        identificado el tipo de la operación.
 * \param net_event Evento de red 
 */
#define ipc_msg_net(listener, net_event)                    \
            do {                                            \
                listener->ipc_msg.status = MSG_AVAILABLE;   \
                listener->ipc_msg.msg_src = MSG_NETWORK;    \
                listener->ipc_msg.event = net_event;        \
                listener->ipc_msg.data = uip_conn;          \
                task_set_ready(listener->task);             \
            } while(0)


/**  */
#define tcp_ext_connect(listener) listener.ipc_msg.event == NET_MSG_CONNECTED
#define tcp_msg(listener) listener.ipc_msg.event == NET_MSG_RECEIVED
#define tcp_ack(listener) listener.ipc_msg.event == NET_MSG_ACKED

/* --------------------------------------------------------------------------------- */

/** 
 * \brief Cerrar la conexión
 */
#define tcp_conn_close(listener)            \
    listener.state = LISTENER_DISCONNECT

/** 
 * \brief Detener la conexión
 */
#define tcp_conn_stop(listener)             \
    listener.state = LISTENER_STOP

/** 
 * \brief Reiniciar la conexión 
 */
#define tcp_conn_restart(listener)          \
    listener.state = LISTENER_RESTART

/* --------------------------------------------------------------------------------- */

/** 
 * \brief Verificar si hay una conexión activa en el listener
 * \param listener listener que será verificado
 * \return Estado de la conexión, true si la conexión activa, false si no 
 */
bool tcp_connected(tcp_listener_st * listener);

/* ---------------------------------- según rfc0793 -------------------------------- */

// !!! se debe cambiar el listener por un numero que es el indice de la conn en uIP

/** 
 * \brief Solicita el establecimiento de una conexión
 * \details Esta función llama a uip_connect() que a su vez comienza una conexión tcp con 
 *  el host ip_addr por el puerto port. Si uip_connect() es exitosa devuelve un uip_conn 
 *  con SYN_SENT. Luego se asigna el listener al appstate y se devuelve. La app 
 * \note Esta función cuando retorna no ha establecido la conexión, solo la ha solicitado
 * \param listener Puntero al listener que intenta conectarse
 * \param ip_add Host al que nos queremos conectar. IP en 4 uint8_t
 * \param port Puerto al que nos queremos conectar
 * \return Si se ha podido solicitar la conexión o no.
 */
bool tcp_open(tcp_listener_st * listener, ip_address_t ip_addr, uint16_t port);

/** 
 * \brief
 * \param
 */
//void tcp_close(tcp_listener_st * listener);

/** 
 * \brief
 * \param
 * \return
 */
//uint8_t tcp_status(tcp_listener_st * listener);

/** 
 * \brief Recivir desde tcp
 * \details Esta función parte del hecho de que tcp envía un mensaje (despierta) a la
 *  app dueña del listener si hay un datagrama para ella. El app cuando despierta debe
 *  verificar la causa por la que ha sido despertada, así esta función podrá ser llamada
 *  periódicamente aún cuando no se ha recibido nada.
 * 
 * \param listener Puntero a la conexión
 * \return Largo que se ha recibido. Será 0 si no se ha recibido nada
 */
uint16_t tcp_recv(tcp_listener_st * listener);

/** 
 * \brief
 * \param 
 * \param
 * \param
 * \return 
 */
bool tcp_send(tcp_listener_st * listener, uint8_t * msg, uint16_t len);



/*  */















/** 
 *  \brief Verifica si ha llegado un mensaje desde la red.
 *  \attention Esta función modifica el mensaje del listener desactivándolo. No debería
 *  hacer dos llamadas consecutivas de esta función a menos que sepa lo que está haciendo 
 *  pues en caso de haber un mensaje la primera devolverá el evento mientras que la segunda
 *  devolverá 0.
 */
//uint8_t tcp_read(tcp_listener_st * listener);

/** 
 *  \brief
 */
//bool tcp_write(struct tcp_listener_st * listener, uint8_t * buffer, uint16_t len);

/** 
 *  \brief Esta función activa la negociación para conectar el host con un extremo TCP que 
 *  escucha por el puerto port_dst
 *  \note Esta función cuando sale no deja o no creada la conexión, esto es una terea de net_tick
 *  quien gestiona la negociación que hace uIP con el extremo y envía a la app un mensaje en 
 *  caso de éxito o no. 
 */
//bool tcp_client_connect(tcp_listener_st * listener, uint8_t * ip_dst, uint16_t port_dst);

#endif /* _TCP_H_ */