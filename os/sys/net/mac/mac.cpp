/** 
 * 
 * 
 */

#include "../../../idos.h"
#include "mac.h"


bool mac_init(uint8_t * mac) {
    
    /* --------------------- ENC28J60 --------------------- */

    #ifdef ENC28J60
    return enc_init(mac);
    #endif


    /* --------------------- NRF24L01 --------------------- */

    #ifdef NRF24L01
    return nrf_init(mac);
    #endif

}



uint16_t mac_poll(uint8_t * frame) {
    
    /* --------------------- ENC28J60 --------------------- */

    #ifdef ENC28J60
    return enc_poll(frame);
    #endif

    /* --------------------- NRF24L01 --------------------- */

    #ifdef NRF24L01
    return nrf_poll(frame);
    #endif

}


bool mac_send(uint8_t * frame, uint16_t len){
    
    /* --------------------- ENC28J60 --------------------- */
    
    #ifdef ENC28J60
    return enc_send(frame, len);
    #endif

    /* --------------------- NRF24L01 --------------------- */

    #ifdef NRF24L01
    return nrf_send(frame, len);
    #endif

}


uint8_t nic_is_active(void) {
    
    /* --------------------- ENC28J60 --------------------- */
    
    #ifdef ENC28J60
    return enc_is_active();
    #endif

    /* --------------------- NRF24L01 --------------------- */

    #ifdef NRF24L01
    return nrf_is_active();
    #endif

}
bool nic_link_status(void) {
    
    /* --------------------- ENC28J60 --------------------- */

    #ifdef ENC28J60
    return enc_link_status();
    #endif

    /* --------------------- NRF24L01 --------------------- */

    #ifdef NRF24L01
    return nrf_link_status();
    #endif

}

void nic_power_off(void) {
 
    /* --------------------- ENC28J60 --------------------- */

    #ifdef ENC28J60
    enc_power_off();
    #endif

    /* --------------------- NRF24L01 --------------------- */

    #ifdef NRF24L01
    nrf_power_off();
    #endif

}
void nic_power_on(void) {

    /* --------------------- ENC28J60 --------------------- */
    
    #ifdef ENC28J60
    enc_power_on();
    #endif

    /* --------------------- NRF24L01 --------------------- */

    #ifdef NRF24L01
    return nrf_power_on();
    #endif

}
