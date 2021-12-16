/**
 * \file netstack.h
 * \author Bernardo Yaser León Ávila (bernardo@idola.it)
 * \brief 
 * \version 0.1
 * \date 2021-12-06
 *
 * \copyright Copyright (c) 2021
 * 
 * \details --
 * 
 * El tratamiento de la red está presentado según los 5 niveles de Tanembaum y serán tratados de la siguiente manera
 *     _____________________________
 *    |                             |
 *    |         app (task)          |
 *    |_____________________________|
 *    |                             |
 *    |    transport (TCP/UDP)      |
 *    |_____________________________|
 *    |                             |
 *    |          net (IP)           |
 *    |_____________________________|
 *    |                             |
 *    |     media access (mac)      |
 *    |_____________________________|
 *    |                             |
 *    |        phy (hardware)       |
 *    |_____________________________|
 *    
 * Estos nomencladores se utilizará para idOS amén de la forma que utilice uIP
 *    - net_frame Trama de capa de acceso al medio. Incluye el encabezado LLH (Low Layer Header)
 *    - net_packet Paquete de capa de red. Incluye solo en encabezado IP.
 *    - 
 *    - net_msg  Mensaje enviado o recibido por la red. Es el buffer que se le envía a la tarea. 
 *               No incluye ningún encabezado
 *    
 */

#ifndef _NETSTACK_H_
#define _NETSTACK_H_

#include "../../idos.h"

extern "C" {
   #include "ip/uip-conf.h"
   #include "ip/uip.h"
   #include "ip/uip_arp.h"
   #include "ip/uip_timer.h"
}


/** \brief Número IP en 4 bytes
 *  \details Diferencia a un IP formado por 4 optetos del 
 *          uIP que está formado por 2 palabras de 16 bits
 */
typedef uint8_t ip_address_t[4];

/** ---------- Transport ---------- */

#ifdef CONF_TRANSP
#include "netconf.h"

#if CONF_TRANSP == TRANSP_UDP
#include "transport/udp.h"
#include "transport/tcp_client.h"
#include "transport/tcp_server.h"
#elif CONF_TRANSP == TRANSP_TCP
#include "transport/tcp.h"
#else
#error Unknown transport layer configuration
#endif /* CONF_TRANSP == ? */

#endif /* CONF_TRANSP */

/** ----------- Routing ----------- */

#ifdef CONF_ROUTING


#endif /* CONF_ROUTING */

/** ------------- NET ------------- */

#ifdef CONF_NET

#include "ip/uip.h"

/* >>> Estas configuraciones tal vez esten mejor detro de IP */
#define UIPETHERNET_FREEPACKET 1
#define UIPETHERNET_SENDPACKET 2
#define UIPETHERNET_BUFFERREAD 4

#define uip_ip_addr(addr, ip) do { \
                     ((u16_t *)(addr))[0] = HTONS(((ip[0]) << 8) | (ip[1])); \
                     ((u16_t *)(addr))[1] = HTONS(((ip[2]) << 8) | (ip[3])); \
                  } while(0)

//#define ip_addr_uip(a) IPAddress(a[0] & 0xFF, a[0] >> 8 , a[1] & 0xFF, a[1] >> 8) //TODO this is not IPV6 capable

#define uip_seteth_addr(eaddr) do {uip_ethaddr.addr[0] = eaddr[0]; \
                              uip_ethaddr.addr[1] = eaddr[1];\
                              uip_ethaddr.addr[2] = eaddr[2];\
                              uip_ethaddr.addr[3] = eaddr[3];\
                              uip_ethaddr.addr[4] = eaddr[4];\
                              uip_ethaddr.addr[5] = eaddr[5];} while(0)



/** 
 * \brief Cada frame recibido de o enviado a la NIC para por uip 
 * \details uIP es quien conforma los frames que serán enviados a 
 *       a la capa MAC. El frame completo (incluido el encabezado
 *       MAC) entra y sale en el buffer uip_buf de uIP. Los siguientes
 *       macros separan los encabezados por protocolos a partir de
 *       estructuras y macros definidos en uIP. Entienda que:
 * \b UIP_LLH_LEN Tamaño del encabezado MAC (Low Layer Header) (14 bytes)
 * \b UIP_IPTCPH_LEN Tamaño del encabezado de IP y TCP conjunto (40 bytes)
 * \b UIP_IPH_LEN Tamaño del encabezado de IP (20 bytes)
 * \b UIP_TCPH_LEN Tamaño del encabezado de TCP (20 bytes)
 * 
 * \b uip_eth_hdr Estructura del encabezado de Ethernet
 * \b uip_tcpip_hdr Estructura del encabezado conjunto de TCP/IP
 * \b uip_udpip_hdr Estructura del encabezado conjunto de UDP/IP
 * 
 *    |<-LLH->|<-IPH->|<-TCP->|<-------------MSG--------------->|
 *            |<----IPTCPH--->|
 *                            |<-------- uip_appdata ---------->|
 *    |<------------------------ uip_buf ---------------------->|
 * 
 * \note Cuando se castea a la estructura de header el frame (que es un
 *       array) y se forman variables de más de un byte estas quedan 
 *       ordenadas alrevés, por ejemplo, la secuencia 00 05 (80) cuando
 *       se castea a un uint16_t queda 50 00 (20480). Si se espera leer 
 *       el número, hacer un cálculo o comparación utilizar htons()
*/
#define hdr_ip_tcp ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define hdr_ip_udp ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define hdr_icmp ((struct uip_icmpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define hdr_eth ((struct uip_eth_hdr *)&uip_buf[0])

/** 
 * \brief Aisla el mesaje de los protocolos 
 * \note Este macro devuelve un puntero de tipo uint8_t
 */
#define uip_tcp_msg &uip_buf[UIP_LLH_LEN + UIP_IPTCPH_LEN]
#define uip_udp_msg &uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN]

/** Mensajes desde net */
enum {
   NET_MSG_RECEIVED = 1,         /**< Mensaje recibido desde la red*/
   NET_MSG_SENDED,               /**< Mensaje enviado a la red */
   NET_MSG_ACK                   /**< Notificación de recepción del estremo */
};

/** 
 * \def     MSG_NET_SEND
 * \brief   Enviar mensaje desde la interface de red hacia una tarea
 * \param   net_event Evento de red 
 */
#define ipc_msg_net(net_event)                     \
            do {                                   \
               * listener->task->msg = {           \
                  MSG_NETWORK,                     \
                  net_event,                       \
                  uip_conn                         \
                };                                 \
               task_set_ready(listener->task);     \
            } while(0)

#endif /* CONF_NET */


/** ------------- MAC ------------- */

#include "mac/mac.h"
#ifdef CONF_MAC

#if CONF_MAC == MAC_ETH
#include "mac/eth/eth.h"
#else
#error Unknown NET configuration
#endif /* CONF_MAC == ? */

#endif /* CONF_MAC */

/** \brief Inicializa la NIC e IP si está definido */
void net_stack_init(void);

/** \brief Configura IP */
//void net_stack_configure(void);

/** \brief Envia, recibe y/o procesa paquetes IP 
 * \details Esta es la función que hace básicamente todo el trabajo en net y debe ser llamada
 * periódicamente. Consta de dos bloques, recibir y enviar demarcadas por las macros de uIP: 
 *    - uip_input(); Bloque recibir: Esta macro si se invoca si se ha recibido un paquete desde 
 *       MAC e invoca a uip_process(UIP_DATA). Esta recive el frame en uip_buf y lo desempaqueta
 *       identificando los protocolos y desencadenando los mecanismos de respuesta de TCP/IP/ICMP. 
 *       También invoca al appcall TCP u UDP en caso de un mensaje recibido para una app (task).
 *    - uip_periodic(conn); Bloque enviar: Esta macro carga la conexión conn en uip_conn y hace un
 *       poll a appcall TCP (en caso UDP se hace con uip_udp_periodic(i);) en busca de un mensaje
 *       que una app desee enviar. De ser así uip_buf se carga con el mensaje y se envía a MAC
 * \note net_tick no solo envia y recibe los mensajes de la app, si no que mantiene la vitalidad de
 * la conexión por lo que debe ser invocada periódicamente.
 * \note de haber algo disponible para la capa de app (una tarea que escucha) transport le enviará
 * un mensaje a dicha tarea para despertarla.
 * \note El net_tick es invocado por el planificador siempre, así se espera que si este despierta una
 * tarea el planificador correrá dicha tarea siempre antes de invocar nuevamente al tick o se puede 
 * perder el mensaje.
 * La gráfica siguiete muestra el flujo resumido de una llamada a net_tick cuando se recibe un mensaje
 * de una tarea. Cada uno de las columnas representa un archivo de códico. Cada acción puede ser la  
 * llamada a una función o conjuto de ellas. La línea de puntos (- - - -) significa el pase de un mensaje
 * 
 * |     task     |     sys      |     tcp      |     netstat     |     uip      |     mac      |     phy      |
 * |              |  net_tick ---|--------------|---->received    |              |              |              |
 * |              |              |              |        +--------|--------------|-->received   |              |
 * |              |              |              |                 |              |       +------|--->driver    |
 * |              |              |              |                 |              |  return ok<--|-------+      |
 * |              |              |              |   uip_process<--|--------------|-------+      |              |
 * |              |              |              |        +--------|-->appcall    |              |              |
 * |              |              |   msg_task---|-----------------|------+       |              |              |
 * |  listener<- -|- - - - - - - |- - - +-------|-----------------|-->renspoce   |              |              |
 * |              |              |              |                 |      +-------|--->mac_send  |              |
 * |              |              |              |                 |              |       +------|--->driver    |
 * |              |              |              |                 |              |  return ok<--|-------+      |
 * |              |              |              |                 |--------------|-------+      |              |
 * |              |              |              |    return ok<---|              |              |              |
 * |              |    return<---|--------------|---------+       |              |              |              |

*/
void net_tick(void);


/* _--------------TCP---------------_ */



#endif /* _NETSTACK_H_ */
