/** 
 * 
 * 
 */

/** Este es un espejo de la estructura de memoria en el enc para poder 
 *  controlar los buffers de entrada y salida.
 *  Mapa de los buffers de entrada y salida:
 * 
 *                               ______________________
 *         RXSTART_INIT (0x00)  |                      |
 *                              |       RX Buffer      |
 *                              |                      |
 *         RXSTOP_INIT (0x1FFF) |______________________|
 *         TXSTART_INIT (0x1800)|                      |
 *                              |       TX Buffer      |
 *                              |                      |
 *         TXSTOP_INIT(0x1FFF)  |______________________|
 * 
*/

#ifndef _ENC28J60_H_
#define _ENC28J60_H_

#include "../../../arch.h"
#include "enc28j60.h"

/* Pone el pin CS del SPI por defecto. Cambiar esta línea en caso de utilizar otro */
#define ENC28J60_CS SS

#define TX_COLLISION_RETRY_COUNT 3

/** Macros para SPI */
/** \brief Configurar el SPI para el ENC28J60 */
#define SPI_ETHERNET_SETTINGS SPISettings(20000000, MSBFIRST, SPI_MODE0)

/** \brief Seleccionar el ENC */
#define enc_select() digitalWrite(ENC28J60_CS, LOW)

/** \brief Deseleccionar el ENC */
#define enc_deselect() digitalWrite(ENC28J60_CS, HIGH)


/* ------------------------------- arch block -------------------------------- */

/** 
 *  \brief Inicializar el ENC 
 *  \param mac Dirección (máscara) MAC que deberá tener el ENC
*/
bool mac_init(uint8_t * mac);

/** 
 *  \brief  Poll a la ENC para ver si ha llegado algo
 *  \details El datasheet trata al frame como packet y así es tratado por 
 *          los drivers consultados. No obstante aquí le llamaremos frame.
 *          Ver el datasheet 7.2 para una completa descripción.
 *  \param frame    Buffer donde será copiada la frame de haber recibido una
 */
uint16_t mac_poll(uint8_t * frame);


/** 
 *  \brief Escribir el frame en la NIC para que lo envie
 *  \details Funciona igual que la dupla recieve - read pero a la inversa 
 *  \param buffer Puntero al buffer donde está el frame en RAM
 *  \param len Tamaño del frame
 *  \return Tamaño del buffer enviado al ENC
 */
bool mac_send(uint8_t * frame, uint16_t len);


/** 
 *  Estado de la interface
 */
uint8_t nic_is_active(void);
bool nic_link_status(void);

/** 
 *  Control de encendido y apagado
 */
void nic_power_off(void);
void nic_power_on(void);

#endif /* _ENC28J60_H_ */