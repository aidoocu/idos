/** 
 * 
 * 
 * 
 * 
 */

#include "../../../arch.h"
#include "nrf24l01_arch.h"

static RF24 radio(NRF24L01_CE, NRF24L01_CSN);

bool nrf_init(uint8_t * mac) {

    radio.begin();

    return true;
}

/* -------------------------- nrf_get_address ----------------------------- */
void nrf_get_address(uint8_t * mac) {
    
    //radio.read_register(RF24_RX_ADDR_P0,mac,5);
}


/* ------------------------------ nrf_poll --------------------------------- */
uint16_t nrf_poll(uint8_t * frame){
    //return radio.available();
    return 0;

}

/* ------------------------------ nrf_send --------------------------------- */

bool  nrf_send(uint8_t * buffer, uint16_t len) {
    //radio.write(buffer,len);
    return true;
}

/* ----------------------------- nic_status ---------------------------------- */

uint8_t nrf_is_active(void) {
    return radio.available();
}

bool nrf_link_status(void) {
    //
}


/* ------------------------------- nrf_power ---------------------------------- */

void nrf_power_off(void){
    //
}

void nrf_power_on(void){
    //
}