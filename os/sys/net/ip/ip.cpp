/** 
 * 
 * 
 */

#include "ip.h"

void ip_config(uint8_t * host, uint8_t * gateway, uint8_t * subnet){

    /* ------------------------------ STACK UIP ------------------------------ */

    #ifdef UIP_STACK

    /** 
     * Se configura la capa de red a partir de la implementación de uIP de Adam Dunkels. 
     *  Esta utiliza variables de 16 bits para representarlas direcciones IP así que hay 
     *  que llevarlas de 8 bits a 16 e inicializar. 
     */

    /* variables de 16 bits para direcciones IP de uIP */
    ipaddr_t uipaddr;

    /* hots */
    uip_ip_addr(uipaddr, host);
    uip_sethostaddr(uipaddr);

    /* gateway */
    uip_ip_addr(uipaddr, gateway);
    uip_setdraddr(uipaddr);

    /* subnet */
    uip_ip_addr(uipaddr, subnet);
    uip_setnetmask(uipaddr);
    
    #endif /* UIP_STACK */

    /* ----------------------------- ESP_NET_STACK --------------------------- */

    #ifdef ESP_NET_STACK

    IPAddress ip_host(host);
    IPAddress ip_gateway(gateway);
    IPAddress mask_subnet(subnet);
    
    WiFi.config(ip_host, ip_gateway, mask_subnet);

    #endif /* ESP_NET_STACK */

}


void ip_get_address(uint8_t * address, uint8_t option){
    
    
    /* ------------------------------ STACK UIP ------------------------------ */
    #ifdef UIP_STACK

    ipaddr_t uip_address;

    switch (option) {
    case ADDR_HOST:
        uip_gethostaddr(&uip_address);
        break;
    case ADDR_GATEWAY:
        uip_getdraddr(&uip_address);
        break;
    case MASK_SUBNET:
        uip_getnetmask(&uip_address);
        break;      
    default:
        break;
    }

    address[0] = (uint8_t)uip_address[0];
    address[1] = (uint8_t)ip_htons(uip_address[0]);
    address[2] = (uint8_t)uip_address[1];
    address[3] = (uint8_t)ip_htons(uip_address[1]);

    #endif /* UIP_STACK */

    /* ------------------------------ STACK LWIP ----------------------------- */

    #ifdef LWIP_STACK

    #endif /* LWIP_STACK */

    /* ---------------------------- ESP_NET_STACK ---------------------------- */
    
    #ifdef ESP_NET_STACK

    /* Obtener la estructura con la configuración de IP */
    struct ip_info esp_ip_address;
    wifi_get_ip_info(STATION_IF, &esp_ip_address);

    switch (option) {
        case ADDR_HOST:
            address = (uint8_t *)&esp_ip_address.ip;
            break;
        case ADDR_GATEWAY:
            address = (uint8_t *)&esp_ip_address.gw;
            break;
        case MASK_SUBNET:
            address = (uint8_t *)&esp_ip_address.netmask;
            break;      
        default:
            break;
    }

    #endif /* ESP_NET_STACK */



}