/** 
 * 
 * 
 * 
 * 
 */

#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "RF24/nRF24L01.h"
#include "RF24/RF24.h"
#include "RF24/RF24_config.h"

/* Pone el pin CS del SPI por defecto. Cambiar esta línea en caso de utilizar otro */
#define NRF24L01_CSN SS
#define NRF24L01_CE 9

/* MAC address */
#define ADDR_5 0x49     /* I */
#define ADDR_4 0x44     /* D */
#define ADDR_3 0x4F     /* O */
#define ADDR_2 0x53     /* S */
#define ADDR_1 0x00     /* 0 */
#define ADDR_0 0x00     /* 0 */

#define NRF24L01_CHANNEL 90
#define NRF24L01_PAYLOAD_SIZE 32
#define NRF24L01_RETR_DELAY 5
#define NRF24L01_RETR_COUNT 3
#define NRF24L01_SPEED RF24_250KBPS
#define NRF24L01_POWER RF24_PA_HIGH


#define TX_COLLISION_RETRY_COUNT 3

/** Macros para SPI */

/** \brief Seleccionar el ENC */
#define nrf_select() digitalWrite(NRF24L01_CSN, LOW)

/** \brief Deseleccionar el ENC */
#define nrf_deselect() digitalWrite(NRF24L01_CSN, HIGH)


/* ------------------------------- arch block -------------------------------- */

/** 
 *  \brief Inicializar el NRF24L01 
 *  \param mac Dirección (máscara) MAC que deberá tener el NRF24L01
 */
bool nrf_init(uint8_t * mac);

/** 
 *  \brief Escribir  
 *  \param mac Dirección (máscara) MAC que deberá tener el NRF24L01
*/
void nrf_get_address(uint8_t * mac);

/** 
 *  \brief  Poll a la NRF para ver si ha llegado algo
 *  \param frame    Buffer donde será copiada la frame, de haber recibido una.
 *  \return Tamaño de la frame recibida. 0 si no se ha recibido nada.
 */
uint16_t nrf_poll(uint8_t * frame);


/** 
 *  \brief Escribir el frame en la NIC para que lo envie
 *  \details Funciona igual que la dupla recieve - read pero a la inversa 
 *  \param buffer Puntero al buffer donde está el frame en RAM
 *  \param len Tamaño del frame
 *  \return Tamaño del buffer enviado al ENC
 */
bool nrf_send(uint8_t * frame, uint16_t len);


/** 
 *  Estado de la interface
 */
uint8_t nrf_is_active(void);
bool nrf_link_status(void);

/** 
 *  Control de encendido y apagado
 */
void nrf_power_off(void);
void nrf_power_on(void);

#endif /* _NRF24L01_H_ */