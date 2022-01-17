/** 
 * Aqui añadimos las configuraciones de red  */

#ifndef _NETCONF_H_
#define _NETCONF_H_

/* Config MAC */
#define MAC_LEN 6
#define MAC_ADDRESS 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED

#define NET_STATIC

#if CONF_NET == NET_IPV4

#define IP_LENGT 4

#ifdef NET_STATIC
/* Config static by default*/
//#define IP_ADDRESS 192, 168, 1, 115
//#define IP_GATEWAY 192, 168, 1, 1
//#define IP_SUBNET 192, 168, 1, 0
//#define IP_MASK 255, 255, 255, 0

#define IP_ADDRESS 127, 18, 0, 5
#define IP_GATEWAY 127, 18, 0, 1
#define IP_SUBNET 127, 18, 0, 0
#define IP_MASK 255, 255, 0, 0

#endif /* NET_STATIC */


#elif CONF_NET == NET_IPV6
#define IP_LENGT 16
#else
#error No IP type defined
#endif






#endif  /* _NETCONF_H_ */