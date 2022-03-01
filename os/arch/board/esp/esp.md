# ESP

Para incorporar al ESP8266 y al ESP32 a idOS utilizaremos el framework para Arduino, documentado en la documentación del [ESP8266 para Arduino](https://arduino-esp8266.readthedocs.io) y del [ESP32 para Arduino](https://github.com/).

## ESP8266 WiFi

### Modos
Los modos son la forma en que operará el ESP con respecto a como se conecta con otros dispositivos WiFi. Se debe ser explísito en la inicilización y se hace con el procedimiento *WiFi.mode(WIFI_MODE)*:
- WIFI_STA: Modo de estación, cuando es solo cliente del WiFi.
- WIFI_AP: Access Point o Punto de Acceso.
- WIFI_AP_STA: Ambos modos, puede ser utilizado para hacer relay o mallas.

