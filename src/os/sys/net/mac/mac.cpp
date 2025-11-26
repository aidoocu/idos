/** 
 * 
 * 
 */

#include <idos.h>
#include "mac.h"


bool mac_init(void) {
    
    /* --------------------- ENC28J60 --------------------- */

    #ifdef ENC28J60
    /** Si la MAC entra por parametro (que no creo correcto) */
    /* pero por ahora se hará así, hay que arreglarlo */
    #define MAC_ADDRESS 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed
    return enc_init((uint8_t *) &MAC_ADDRESS); //esto no parece correcto, revisar
    #endif


    /* --------------------- NRF24L01 --------------------- */

    #ifdef NRF24L01
    return nrf_init();
    #endif

}

void mac_get_address(uint8_t * mac) {
    
    /* --------------------- ENC28J60 --------------------- */

    #ifdef ENC28J60
    enc_get_address(mac);
    #endif

    /* --------------------- NRF24L01 --------------------- */

    #ifdef NRF24L01
    nrf_get_address(mac);
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
