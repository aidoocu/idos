/** 
 * 
 * 
 */

#include "net_esp.h"

bool mac_init(uint8_t * mac){
    
    /* Modo de estación */
    WiFi.mode(WIFI_STA);

    #if NET_DEBUG >= 3
    printf("ESP WiFi starting \r\n");
    #endif

    /*  */
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    uint8_t wifi_conn_retry = 0;

    /* Verificamos conección al AP y si no fuera exitosa reintentamos
    hasta WIFI_CONN_RETRY veces */
    while (WiFi.waitForConnectResult() != WL_CONNECTED 
        || wifi_conn_retry <= WIFI_CONN_RETRY) {
        
        #if NET_DEBUG >= 3
        printf(".");
        #endif

        /* Esperamos 5 segundos */
        // No se si fuera mejor no esperar y arracar el sistema e intentar
        // directamente desde el tick, al final esto detiene el programa 
        // aqui hasta 3 segundos aproximadamente si no puede conectarse
        delay(500);

    }

    if (wifi_conn_retry == 6) {

        #if NET_DEBUG >= 3
        printf("WiFi connexion failed \r\n");
        #endif

        return false;
    }
    
    #if NET_DEBUG >= 3
    printf("WiFi connexion sucess \r\n");
    printf("IP address: %s\r\n", WiFi.localIP())
    #endif   

    return true;
}