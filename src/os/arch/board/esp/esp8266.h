/**
 * @file esp8266.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _ESP_8266_H_
#define _ESP_8266_H_


/* -------------------------- network ----------------------- */

/* El framework nombra de forma distinta al encabezado WiFi */
#include <ESP8266WiFi.h>



/* -------------------------- random ------------------------ */

#define idos_random() random()


#endif /* _ESP_8266_H_ */