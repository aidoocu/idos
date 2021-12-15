/** 
 * 
 * 
 * \date diciembre 2021
 */

#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "../netstack.h"

#define UIP_SOCKET_DATALEN UIP_TCP_MSS

#define UIP_CLIENT_CONNECTED 0x10
#define UIP_CLIENT_CLOSE 0x20
#define UIP_CLIENT_REMOTECLOSED 0x40
#define UIP_CLIENT_RESTART 0x80
#define UIP_CLIENT_STATEFLAGS (UIP_CLIENT_CONNECTED | UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED | UIP_CLIENT_RESTART)
#define UIP_CLIENT_SOCKETS ~UIP_CLIENT_STATEFLAGS

typedef uint8_t uip_socket_ptr;


/*  */
typedef struct {
    uint8_t state;
    mem_handle_t packets_in[UIP_SOCKET_NUMPACKETS];
    uint16_t lport;                                     /**< The local TCP port, in network byte order. */
} uip_userdata_closed_st;

/* Guarda la conexión de una app. Apunta a sus buffers de in-out */
typedef struct {
    uint8_t conn_index;
    uint8_t state;                                                  /**< The state of the connexion */
    mem_handle_t packets_in[UIP_SOCKET_NUMPACKETS] = {NOBLOCK};     /**< Packets received */
    mem_handle_t packets_out[UIP_SOCKET_NUMPACKETS] = {NOBLOCK};    /**< Packets to send */
    mem_address_t out_pos;
} uip_user_st;



/** \brief Conecta a un host remoto usando TCP
 *  \param ip_dst IP al que nos queremos conectar
 *  \param por_dst Puerto al que nos queremos conectar
 */
int client_connect(ip_address_t ip_dst, uint16_t port_dst);


#endif /* _TCP_CLIENT_H_ */