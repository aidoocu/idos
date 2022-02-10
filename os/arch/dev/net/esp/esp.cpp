/** 
 * 
 * 
 */

#include "esp.h"

bool esp_wifi_begin(void){

    WiFi.begin(WIFI_SSID, WIFI_PASS);   

    return;
}