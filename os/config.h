/** 
 * Configuraciones globales
 * 
 * */

#ifndef _CONFIG_H_
#define _CONFIG_H_

/** Activa el modo de ahorro de energía */
#define SLEEP_MODE 0

/** Se utiliza el SPI */
#define SPI_ARCH

/** ------------------------ Debug Info ------------------------ */

#define NET_DEBUG_0 0
#define NET_DEBUG_1 1
#define NET_DEBUG_2 2
#define NET_DEBUG_3 3


#define NET_DEBUG   2


/** ----------------------- Dispositivos ----------------------- */
/** \brief Se defininen los dispositivos y forma de conectarse   */

/**                          Networkig                           */

/** \b ENC28J60  
 *  \brief      Microchip ENC28J60 Ethernet Interface
 *  \details    Se conecta por SPI y solo implementa el mecanismo de
 *              acceso al medio Ethernet. Deberá utilizarse uip
*/
#define __ENC28J60__

/** Configuraciones por defecto */
#ifdef __ENC28J60__

#ifndef SPI_ARCH
#define SPI_ARCH
#endif

#endif /* __ENC28J60__ */

/** ------------------- Definiciones de red -------------------- */

#define NET_STACK_NULL  0
#define NET_STACK_HW    1
#define NET_STACK_UIP   2


/* Comente la linea a continuación si no tiene ninguna interface de red */
#define NET_STACK NET_STACK_UIP


#ifdef NET_STACK





/*      Pontencialmente esto no sea necesario       */

/** TRANSPORT 
 * \brief Habilita el uso de la capa de tansporte. Puede comletar con:
 * \b TRANSP_UDP  Su interface será configurada para utilizar IPv4 
 * \b TRANSP_TCP  Su interface será configurada para utilizar IPv6 
 * \note puede comentar esta linea para no cargar la capa de red */
#define CONF_TRANSP TRANSP_UDP

#define TRANSP_UDP 1
#define TRANSP_TCP 2

/** Routing: 
 *  \todo Aun no implementado
*/

/** NET 
 * \brief Habilita el uso de la capa de red. Puede comletar con:
 * \b NET_IPV4  Su interface será configurada para utilizar IPv4 
 * \b NET_IPV6  Su interface será configurada para utilizar IPv6 
 * \note puede comentar esta linea para no cargar la capa de red */
#define CONF_NET NET_IPV4

#define NET_IPV4 1
#define NET_IPV6 2

/** MAC 
 * \brief Habilita el uso de la capa de enlace. Puede comletar con:
 * \b MAC_ETH   Su interface será configurada para utilizar ETH
 * \todo Falta incluir los mecanismos de radio CSMA, TSCH y BLE
 * \note puede comentar esta linea para no cargar la capa de acceso al medio*/
//#define CONF_MAC MAC_ETH

//#define MAC_ETH 1

#endif /* NET_SATCK */


#endif /* _CONFIG_H_ */