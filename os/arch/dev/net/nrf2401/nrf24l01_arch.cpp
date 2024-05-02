/** 
 * 
 * 
 * 
 * 
 */



#include "nrf24l01_arch.h"

/**
 * @brief Class representing the NRF24L01 radio module.
 *
 * This class provides an interface to control and communicate with the NRF24L01 radio module.
 * It uses the NRF24L01_CE and NRF24L01_CSN pins for communication.
 */
static RF24 radio(NRF24L01_CE, NRF24L01_CSN);

/**
 * @brief The destination address for the NRF24L01 module.
 *
 * This variable stores the destination address used by the NRF24L01 module.
 * The address width is defined by NRF_ADDR_WIDTH.
 */
static uint8_t nrf_dst_addr[NRF_ADDR_WIDTH];

bool nrf_init(uint8_t * mac) {

    if(!radio.begin())
        return false;

    uint8_t nrf_mac[5] = {NRF_ADDR_0, 
                            NRF_ADDR_1, 
                            NRF_ADDR_2, 
                            NRF_ADDR_3, 
                            NRF_ADDR_4};

/*     for(uint8_t i = 0; i <= NRF_ADDR_WIDTH; i++)
         nrf_mac[i] = mac[i]; */

    /* Abriendo el pipe de escucha, que no debe ser nunca el 0 */
    radio.setAddressWidth(NRF_ADDR_WIDTH);
    //radio.openReadingPipe(NRF_PIPE_1, nrf_mac);

    radio.openWritingPipe(0xE8E8F0F0E1LL);

    /* Aseguro el radio apagado */
    radio.stopListening();
    //radio.powerDown();

    printf("radio ok\n");
    
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

/* ---------------------------- nrf_dst_addr ------------------------------- */

void set_nrf_dst_addr(uint8_t * mac) {
    for(uint8_t i = 0; i <= NRF_ADDR_WIDTH; i++)
        nrf_dst_addr[i] = mac[i];
}

/* ------------------------------ nrf_send --------------------------------- */

bool  nrf_send(uint8_t * buffer, uint16_t len) {

    //radio.openWritingPipe(nrf_dst_addr);
    
    //radio.powerUp();

    printf("Sending -->\n");

    radio.write(buffer, len);
    
    //radio.powerDown();

    return true;
}

/* ----------------------------- nic_status ---------------------------------- */

uint8_t nrf_is_active(void) {
    return radio.available();
}

bool nrf_link_status(void) {
    return radio.isChipConnected();
}


/* ------------------------------- nrf_power ---------------------------------- */

void nrf_power_off(void){
    //
}

void nrf_power_on(void){
    //
}