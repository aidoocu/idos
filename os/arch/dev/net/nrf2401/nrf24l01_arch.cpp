/** 
 * 
 * 
 * 
 * 
 */


#include "../../../arch.h"
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

/**
 * @brief Initializes the NRF24L01 module with the given MAC address.
 * 
 * @param mac The MAC address to be used for communication.
 * @return True if the initialization is successful, false otherwise.
 */
bool nrf_init(uint8_t * mac) {

    /** Setting the MAC address, width first */
    radio.setAddressWidth(NRF_ADDR_WIDTH);
    /** A diferencia de ethernet, wifi, etc, nrf tiene solo 5 bytes,
     * por lo que hay que ajustar la dirección MAC a 5 bytes
     */
    uint8_t nrf_mac[NRF_ADDR_WIDTH];

    /* Si la MAC entra por parametro (que no creo correcto) */
    #ifdef MAC_ADDRESS

    for(uint8_t i = 0; i < NRF_ADDR_WIDTH; i++)
         nrf_mac[i] = mac[i];
    
    #else
    /* Si no hay que poner una nueva MAC */

    nrf_mac[0] = NRF_ADDR_0;
    nrf_mac[1] = NRF_ADDR_1;
    nrf_mac[2] = NRF_ADDR_2;
    nrf_mac[3] = NRF_ADDR_3;

    /** Si no esta definida en la configuracion se pondra un 
     * numero aleatorio */
    if (NRF_ADDR_0 == 0) {
        nrf_mac[4] = (uint8_t)analogRead(A0);
    } else {
        nrf_mac[4] = NRF_ADDR_4;
    }
                      
    #endif

    if(!radio.begin())
        return false;

    /** Abriendo el pipe de escucha, que no debe ser nunca el Pipe 0
     * porque lo utiliza el transmisor, y la funcion openWritePipe
     * lo sobreescribe cuando es llamada.
     */
    radio.openReadingPipe(NRF_PIPE_1, nrf_mac);
    
    /* y mantenerlo apagado */
    radio.stopListening();
    /* Aseguro el radio apagado */
    radio.powerDown();

    printf("NRF radio ok\n");

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
    
    for(uint8_t i = 0; i < NRF_ADDR_WIDTH; i++)
        nrf_dst_addr[i] = mac[i];

}

/* ------------------------------ nrf_send --------------------------------- */

bool  nrf_send(uint8_t * buffer, uint16_t len) {

    /** Power up will take up to 5ms for maximum compatibility, the delay is 
     * implicit in the write function
    */
    radio.powerUp();
    radio.openWritingPipe(nrf_dst_addr);

    printf("Sending -->\n");

    bool tx_sucess = radio.write(buffer, len);
    
    radio.powerDown();

    return tx_sucess;
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