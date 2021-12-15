/** 
 * 
 * 
 *  \date diciembre 2021
 */

#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "../netstack.h"

/** \brief  El total de octetos dedicados a encabezados en la trama 
 *  \note   El mensaje comienza en uip_buf[UIP_LLH_LEN - UIP_TCPIP_HLEN - 1]
 *          y su largo es uip_len - UIP_LLH_LEN - UIP_TCPIP_HLEN
 *          y no será más largo que UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN
*/
#define MAX_NET_MSG_SIZE UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN

/* Puertos estándar */
#define HTTP_PORT 80

/** \brief Crear y arrancar el servidor TCP
 *  \details Crea la estructura del listener y llama a la función que arranca el server 
 *  \note Escrito de esta manera no es necesario verificar si el puerto está ocupado pues
 *          se crearán dos variables con el mismo nombre induciendo un error de compilación
 */
#define tcp_server(tcp_port)                                        \
            static struct tcp_listener_st listener_##tcp_port;      \
            tcp_listener_begin(&listener_##tcp_port, tcp_port, task)


/** \brief Detiene el servidor TCP
 *  \details 
 */
#define tcp_server_end(port)                        \
            tcp_listener_end(&listener_##port)

/** \brief Estrucctura de un punto de escucha TCP 
 *  \details Las tareas que deseen escuchar por un puerto TCP porque sean servidores que
 *          o pretendan conectarse a uno externo utilizarán esta estrutura para gestionar 
 *          la escucha. Los servidores TCP se guardan como una lista enlazada.
 */
struct tcp_listener_st {
    uint16_t port;                              /**< Puerto TCP de escucha */
    bool listening = false;                     /**< Estado de la escucha */        
    tcp_listener_st * next = NULL;              /**< Puntero al próximo listener o a NULL */
    task_st * task;                             /**< Puntero a la tarea que setea el listerner */
    uint16_t msg_len_in;                        /**< Tamaño del mensaje que está en el buffer */
    uint16_t msg_len_out;                       /**< Tamaño del mensaje que está en el buffer */
    uint8_t net_msg_in[MAX_NET_MSG_SIZE];       /**< Buffer que contendrá el mensaje de la red */
    uint8_t net_msg_out[MAX_NET_MSG_SIZE];      /**< Buffer que contendrá el mensaje de la red */
};

/** 
 * 
 */
extern struct tcp_listener_st * tcp_listeners;

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
 * \brief Busca conexión activa el puerto especificado
 * \param port Puerto en el que se busca la conexión activa
 */
//uip_user_st available(uint16_t port);

#endif /* _TCP_SERVER_H_ */