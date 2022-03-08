/** 
 * 
 *
 * \note Por defecto, ESP intentará reconectarse a la red Wi-Fi cada vez 
 *  que se desconecte. No hay necesidad de manejar esto por un código separado.
 *  Una buena manera de simular la desconexión sería reiniciar el punto de 
 *  acceso. El ESP informará de la desconexión y luego intentará reconectarse 
 *  automáticamente.
 * 
 * 
 */

#ifndef _ESP8266_WIFI_H_
#define _ESP8266_WIFI_H_

#include "../../arch.h"


#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
//#include <user_interface.h> /* sdk de base */
#endif

/* Cantidad de intentos de reconectarse */
#ifndef WIFI_CONN_RETRY
#define WIFI_CONN_RETRY 5
#endif

#define AUTO_RECONNECT

#define WIFI_SSID "TP-LINK_CC7300"
#define WIFI_PASS "60774774"

/** 
 * \brief Arrancar la WiFi 
 * \note Esta función arranca la WiFi en modo estación para conectarse
 * como a un AP que tiene como credenciales WIFI_SSID y WIFI_PASS.
 * \todo Hay que valorar si fuera necesario que el modo sea AP_STA o hacer
 * que sea configurable o .......
*/
bool mac_init(uint8_t * mac);


void mac_get_address(uint8_t * mac);


#endif /* _ESP8266_WIFI_H_ */