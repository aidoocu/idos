

#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "../../../arch.h"

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#endif

#define WIFI_SSID "TP-LINK_CC7300"
#define WIFI_PASS "60774774"

/* Arrancar la WiFi */
bool esp_wifi_begin(void);

#endif /* _ESP8266_H_ */