/** 
 * ¡¡Estamos asumiendo el modo estación con un usuario y contraseña ya establecido, o sea
 * desde la programación. Aquí habría que explorar el uso de la memoria flash para configurar
 * el SSID, el usuario y la contraseña y así hacer un poco más plástico el comportamiento.
 * También hay que valorar en escaneo en busca de wifi y guardar la configuración en la flash,
 * y... !!
 * 
 */

#include "net_esp.h"

/** 
 * 
 */
bool mac_init(uint8_t * mac) {
    
    #if NET_DEBUG >= 2
    printf("ESP WiFi starting \r\n");
    #endif

    /* Modo de estación */
    WiFi.mode(WIFI_STA);

    /* Si hay una dirección mac declarada se inicializa */
    #ifdef MAC_ADDRESS
    wifi_set_macaddr(STATION_IF, mac);
    #endif

    /*  */
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    uint8_t wifi_conn_retry = 0;

    /* Verificamos conección al AP y si no fuera exitosa reintentamos
    hasta WIFI_CONN_RETRY veces */
    #if NET_DEBUG >= 2
    int8_t conn_result;
    while ((conn_result = WiFi.waitForConnectResult()) != WL_CONNECTED) {
    #else /* NET_DEBUG */
    while ((WiFi.waitForConnectResult()) != WL_CONNECTED) {
    #endif /* NET_DEBUG */

        /* Razones por la que la connexión no fue exitosa */
        #if NET_DEBUG >= 2
        printf(".");

        switch (conn_result){
        case WL_NO_SSID_AVAIL:
            printf("Configured SSID cannot be reached");
            break;
        case WL_CONNECT_FAILED:
            printf("Connection failed");
            break;
        case WL_WRONG_PASSWORD:
            printf("Password is incorrect");
            break;
        case -1:
            printf("connection failed");
            break;        
        default:
            break;
        }

        #endif

        /* Esperamos 5 segundos */
        // No se si fuera mejor no esperar y arracar el sistema e intentar
        // directamente desde el tick, al final esto detiene el programa 
        // aqui hasta 3 segundos aproximadamente si no puede conectarse
        delay(500);

        wifi_conn_retry ++;

        /* Si alcanza el número máximo de reintentos y no se conecta, falla... */
        if (wifi_conn_retry == WIFI_CONN_RETRY) {

            #if NET_DEBUG >= 2
            printf("WiFi connexion failed \r\n");
            #endif

            return false;
        }

    }

    /* Solicitarle a la NIC reconectarse en caso de perder la conexión */
    #ifdef AUTO_RECONNECT
    WiFi.setAutoReconnect(true);
    #endif /* AUTO_RECONNECT */

    #if NET_DEBUG >= 2
    printf("WiFi connexion success \r\n");
    #endif /* NET_DEBUG */

    return true;
}


void mac_get_address(uint8_t * mac) {

    WiFi.macAddress(mac);
   
}





