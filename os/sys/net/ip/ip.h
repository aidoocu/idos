/** 
 * 
 * 
 */

#ifndef _IP_H_
#define _IP_H_

#include "../../../arch/arch.h"

/* Definiendo el stack de IP según plataforma */

/** Interfaces que no implementan IP...
 * la gestion del MAC se hace por la 
 * abstración de la capa MAC + la interface
 * 
 * ENC28J60 implementa Ethernet
 * NRF24L01 implementa RF, el MAC se hace aparte
 */
#if defined(ENC28J60) || defined(NRF24L01)  
/* Si esta placa es de 8 bits se implementa uIP */
#ifdef ARDUINO_ARCH_AVR
#define UIP_STACK
#endif /* ARDUINO_ARCH_AVR */
#endif /* ENC28J60 */

/* Placa ESP8266 - LwIP incluido en el framework */
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#define LWIP_STACK
#define ESP_NET_STACK
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
//#include <user_interface.h> /* sdk de base */
#endif /* ESP32 */
//#define LWIP_INTERNAL
#endif /* ARDUINO_ARCH_ESP8266/ESP32 */

#ifdef NATIVE
/* Por ahora NATIVE usara uIP */
#define UIP_STACK 
#endif /* NATIVE */

/** 
 * 
 * 
 */
enum {
   ADDR_HOST = 0,
   ADDR_GATEWAY,
   ADDR_SUBNET,
   MASK_SUBNET
};


/** \brief Número IP en 4 bytes. Forma global para IdOS
 *  \details Diferencia a un IP formado por 4 optetos del 
 *          uIP que está formado por 2 palabras de 16 bits
 */
typedef uint8_t ip_address_t[4];


/** 
 * \
 * 
 */
void ip_config(uint8_t * host, uint8_t * gateway, uint8_t * subnet);


/** 
 * 
 * \param address Variable a donde se copiará la dirección IP
 * \param option Cual dirección IP se copiará:
 *    ADDR_HOST: IP del host
 *    ADDR_GATEWAY: IP del gateway
 *    ADDR_SUBNET: IP de la subnetwork
 */
void ip_get_address(uint8_t * address, uint8_t option);


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
#if defined(LWIP_STACK) || defined(ESP_NET_STACK)

#define LLH_LEN      12 
#define IPUDPH_LEN   30
#define TCPIP_HLEN   40
#define NET_BUFSIZE  512

/* Homogenizando el tipo ip_addr */
typedef uint16_t lwipv4_addr_t[2];
#define ipaddr_t lwipv4_addr_t

/* Homogenizando el htons */
#define ip_htons lwip_htons
//#define IP_HTONS LWIP_HTONS

#endif /* LWIP_STACK || ESP_NET_STACK */

/* Función que atiende udp tanto para recibir como transmitir */
#ifdef ESP_NET_STACK
void esp_net_udp_appcall(void);
#define udp_poll esp_net_udp_appcall
#endif /* ESP_NET_SATCK */


#endif /* _IP_H_ */