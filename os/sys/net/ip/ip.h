/** 
 * 
 * 
 */

#ifndef _IP_H_
#define _IP_H_


/** \brief Número IP en 4 bytes. Forma global para IdOS
 *  \details Diferencia a un IP formado por 4 optetos del 
 *          uIP que está formado por 2 palabras de 16 bits
 */
typedef uint8_t ip_address_t[4];


/* Definiciones para uIP, principalmente para
placas de 8 bits como Arduino Nano */
#ifdef UIP_STACK

#ifdef __cplusplus
extern "C" {
#endif

   #include "uip/uipopt.h"
   #include "uip/uip-conf.h"
   #include "uip/uip.h"
   #include "uip/uip_arp.h"
   #include "uip/uip_timer.h"
   
#ifdef __cplusplus
}
#endif

/* Redefiniendo macros a formas globales */
#define LLH_LEN      UIP_LLH_LEN 
#define IPUDPH_LEN   UIP_IPUDPH_LEN
#define TCPIP_HLEN   UIP_TCPIP_HLEN
#define NET_BUFSIZE  UIP_BUFSIZE

/* Homogenizando el tipo ip_addr */
#define ipaddr_t uip_ipaddr_t


#define uip_ip_addr(addr, ip) do { \
                     ((u16_t *)(addr))[0] = ip_htons(((ip[0]) << 8) | (ip[1])); \
                     ((u16_t *)(addr))[1] = ip_htons(((ip[2]) << 8) | (ip[3])); \
                  } while(0)


#define uip_seteth_addr(eaddr) do {uip_ethaddr.addr[0] = eaddr[0]; \
                              uip_ethaddr.addr[1] = eaddr[1];\
                              uip_ethaddr.addr[2] = eaddr[2];\
                              uip_ethaddr.addr[3] = eaddr[3];\
                              uip_ethaddr.addr[4] = eaddr[4];\
                              uip_ethaddr.addr[5] = eaddr[5];} while(0)

/* Homogenizando el htons */
#define ip_htons uip_htons
//#define IP_HTONS UIP_HTONS

#endif /* UIP_STACK */

/* Definiciones para LowIP placas como ESP8266 o ESP32 */
#ifdef LWIP_STACK

#define LLH_LEN      12 
#define IPUDPH_LEN   12
#define TCPIP_HLEN   40
#define NET_BUFSIZE  128

/* Homogenizando el tipo ip_addr */
typedef uint16_t lwipv4_addr_t[2];
#define ipaddr_t lwipv4_addr_t

/* Homogenizando el htons */
#define ip_htons lwip_htons
//#define IP_HTONS LWIP_HTONS

#endif /* LWIP_STACK */

#endif /* _IP_H_ */