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
    
    /* -------------------- VAMP_STACK -------------------- */
    /* ToDo!! por un problema de orden es probable que pasar esta
    parte para otro fichero sea mejor */
    #ifdef VAMP_STACK

    /* Creamos la variable que tendra el ID local del cliente VAMP */
    uint8_t vamp_client_id[5] = {0};

    /* VAMP client id storage address in EEPROM (change if needed) */
    #ifndef VAMP_CLIENT_ID_EEPROM_ADDR
    #define VAMP_CLIENT_ID_EEPROM_ADDR 0x10
    #endif

    /* Read the 5-byte VAMP client id from MCU non-volatile storage */
    #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
        #include <avr/eeprom.h>
        eeprom_read_block((void *)vamp_client_id, (const void *)VAMP_CLIENT_ID_EEPROM_ADDR, 5);
    #else
        (void)vamp_client_id;
        return false; // EEPROM read not implemented for this platform  
    #endif /* AVR */

    /* ToDo esto no deberia ser asi porque pudiera ser una interface diferente */
    
    //#include "../../../lib/idos/src/os/arch/dev/net/nrf2401/nrf24l01_arch.h"

    printf("ID: \n");
    for (int i = 0; i < 5; i++) {
        printf("%02X ", vamp_client_id[i]);
    }
    printf("\n");
    delay(10);

    #include "../vamp/vamp_client.h"
    vamp_client_init(vamp_client_id);

    #endif /* VAMP_STACK */

    return false;

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
    
    return 0;

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

    return false;

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

    return 0;
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

    return false;

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
