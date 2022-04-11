#ifndef _UDP_H_
#define _UDP_H_

#include "../ip/ip.h"

#ifdef ESP_NET_STACK
#include <WiFiUdp.h>
#endif /* ESP_NET_STACK */

/* Tamaño total de todos lo headers de UDP (LLH + IPH + UDPH) */
#define UIP_UDP_PHYH_LEN LLH_LEN + IPUDPH_LEN
/* Tamaño máximo del mensaje (payload) que se puede enviar */
#define MAX_UDP_MSG_SIZE NET_BUFSIZE - UIP_UDP_PHYH_LEN

/* Aisla en la estructura uip_udpip_hdr los headers IP, UDP y el payload */
#define udp_buf ((struct uip_udpip_hdr *)&uip_buf[LLH_LEN])


enum {
	UDP_MSG_NULL = 0,
	UDP_MSG_IN,
	UDP_MSG_READED,
	UDP_MSG_RESPONSE,
	UDP_MSG_SENDED
}; 


struct udp_listener_st {

	uint8_t status;						/**< Estado del mensaje */
	struct task_st * task;				/**< Puntero a la tarea que setea el listerner */
	/* Parece que esto no va a ser necesario */
	struct msg_st ipc_msg;				/**< Mensaje a la tarea que setea el listerner */
    uint16_t msg_len_in;                /**< Tamaño del mensaje que está en el buffer de entrada */
    uint16_t msg_len_out;               /**< Tamaño del mensaje que está en el buffer de salida */
    uint8_t msg_in[MAX_UDP_MSG_SIZE];   /**< Buffer que contendrá el mensaje de la red */
    uint8_t msg_out[MAX_UDP_MSG_SIZE];  /**< Buffer que contendrá el mensaje de la red */

#ifdef UIP_STACK
	struct uip_udp_conn * udp_conn;		/**< Estructura a la conexión que maneja uIP */
	//ipaddr_t ripaddr;					/**< IP remoto desde donde hay una conexión >*/
	//uint16_t rport;						/**< Puerto remoto desde donde hay una conexión >*/
	//uint16_t lport;						/**< Puerto local >*/
#endif /* UIP_STACK */

#ifdef ESP_NET_STACK
	WiFiUDP udp_conn;
	udp_listener_st * next;
#endif /* ESP_NET_STACK */

};


#ifdef ESP_NET_STACK
extern struct udp_listener_st * udp_listeners;		
#endif /* ESP_NET_STACK */

/** 
 * 	\brief	Crear e inicializar un listener udp
 */
#define udp_listener(listener, port)            \
            static udp_listener_st listener;    \
			udp_listener_begin(&listener, port);\
            listener.task = task

/* --------------------------------------------------------------------------------- */

/** 
 * \brief Pasarle un mensaje (despertar) a la tarea que tiene el listener
 * \note Todo lo que la tarea necesita está en el listener, así que el mensaje está 
 *  vacío
 */
#define udp_ipc(listener) 									\
			listener->task->msg->msg_src |= MSG_NETWORK;	\
			task_set_ready(listener->task)

/* --------------------------------------------------------------------------------- */


/** 
 * \details Como UDP no es orientado a la conexión no hay necesidad ni de crear una
 *  "conexión" ni mucho menos que esta persista. Así que uIP reutiliza la misma idea
 *  de conexión de TCP pero creando una única por app que atiende a todos los extremos.
 *  A esta conexión que no es real le llamaremos aquí pseudoconexión UDP o simplemente
 *  pseudoconexión.
 */
bool udp_listener_begin(struct udp_listener_st * listener, uint16_t port);

/** 
 * \brief Verifica si se ha recivido algo desde UDP
 */
bool udp_recv(struct udp_listener_st * listener);

/** 
 * 	\brief Enviar un mensaje por udp
 * 	\param dst_addr Dirección IP, en 4 octetos, de destino del mensaje
 * 	\param port Puerto de destino del mensaje
 * 	\param msg Puntero al mensaje
 * 	\param len Tamaño del mensaje
 * 	\return true si fue enviado exitosamente, false si no 
 * 	\attention Esta función sitúa el mensaje a enviar en una estructura que es única
 * 	para todas las tareas, así que solo se podrá alojar un mensaje por siclo del 
 * 	planificador. Siendo así, si la tarea antecesora pide enviar un mesaje, se devolverá
 * 	un false. La tarea que optiene el false tiene dos opciones, descartar el envío o
 *  intentar el próximo ciclo.
 */
bool udp_send(ip_address_t dst_addr, uint16_t port, uint8_t * msg, uint16_t len);

/** 
 * 	\brief Envía lo que esté seteado en el listener
 * 	\param listener Listener UDP a procesar
 */
bool udp_send_from(struct udp_listener_st * listener, ip_address_t dst_addr, uint16_t port, uint8_t * msg, uint16_t len);


/** 
 * \brief responder a un remoto udp
 * \details Esta función podemos utilizarla cuando hemos recibido un mensaje de un remoto
 * 	y queremos responderle a ese remoto en concreto. Los datos del remoto (ip y puerto) 
 *  persisten mientras no se reciba un mensaje de un remoto diferente.
 * \param msg Mensaje al remoto
 * \param len Tamaño del mensaje
 * \param listener listener a cual se le responderá
 * \return true si se puede enviar, false si no, normalmente porque el sender está ocupado
 * \code {.c}
 *  if (udp_recv(&listener)) {
 * 		printf("%s\n", listener.net_msg_in);
 *      
 * 		uint8_t mensaje[] = {"hola remoto"};
 * 		udp_response_to(&listener, mensaje, sizeof(mensaje));
 * }
 * \endcode
 */
bool udp_response_to(struct udp_listener_st * listener, uint8_t * msg, uint16_t len);



/* ------------------------------- Macros propios ------------------------------- */

#ifdef UIP_STACK

/** 
 * \brief Obtener la dirección ip del remoto
 * \param listener Puntero al listerner del que vamos a extraer el ip del remoto
 * \param ipaddr Puntero al arreglo donde vamos a poner la ip del remoto
 */
#define udp_remote_addr(listener, ip_addr)								\
	ip_addr[0] = (uint8_t)(listener.udp_conn->ripaddr[0]);				\
	ip_addr[1] = (uint8_t)ip_htons(listener.udp_conn->ripaddr[0]);		\
	ip_addr[2] = (uint8_t)(listener.udp_conn->ripaddr[1]);				\
	ip_addr[3] = (uint8_t)ip_htons(listener.udp_conn->ripaddr[1])


/** 
 * \brief Obtener el puerto de origen en el remoto
 * \param listener Puntero al listerner del que vamos a extraer el puerto
 */
#define udp_remote_port(listener) ip_htons(listener.udp_conn->rport)


/** 
 * 
 */
#define udp_remove_remote(udp_conn)		\
		do {							\
			udp_conn->rport = 0;		\
			udp_conn->ripaddr[0] = 0;	\
			udp_conn->ripaddr[1] = 0;	\
		} while (0)

#endif /* UIP_STACK */

#ifdef ESP_NET_STACK

/** 
 * \brief Obtener la dirección ip del remoto
 * \param listener Puntero al listerner del que vamos a extraer el ip del remoto
 * \param ipaddr Puntero al arreglo donde vamos a poner la ip del remoto
 */
#define udp_remote_addr(listener, ip_addr)			\
	ip_addr[0] = listener.udp_conn.remoteIP()[0];	\
	ip_addr[1] = listener.udp_conn.remoteIP()[1];	\
	ip_addr[2] = listener.udp_conn.remoteIP()[2];	\
	ip_addr[3] = listener.udp_conn.remoteIP()[3]	


/** 
 * \brief Obtener el puerto de origen en el remoto
 * \param listener Puntero al listerner del que vamos a extraer el puerto
 */
#define udp_remote_port(listener) listener.udp_conn.remotePort()
	

/** 
 * 
 */
#define udp_remove_remote(udp_conn)

#endif /* ESP_NET_STACK */

#endif /* _UDP_H_ */