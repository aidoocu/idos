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
#define UIP_RECEIVEBUFFERHANDLE 0xFF

#define UIP_SENDBUFFER_PADDING 7
#define UIP_SENDBUFFER_OFFSET 1

#define TX_COLLISION_RETRY_COUNT 3

/** Macros para SPI */
/** \brief Configurar el SPI para el ENC28J60 */
#define SPI_ETHERNET_SETTINGS SPISettings(20000000, MSBFIRST, SPI_MODE0)

/** \brief Seleccionar el ENC */
#define enc_select() digitalWrite(ENC28J60_CS, LOW)

/** \brief Deseleccionar el ENC */
#define enc_deselect() digitalWrite(ENC28J60_CS, HIGH)

/** \brief Esperar por resultado de ENC */
#define waitspi() while(!(SPSR&(1<<SPIF)))


/** 
 *  \brief Inicializar el ENC 
 *  \param mac Dirección (máscara) MAC que deberá tener el ENC
*/
bool mac_init_arch(uint8_t * mac);

/** 
 *  \brief Tamaño del paquete 
 *  \details El paquete puede estar en un buffer en el ENC en caso de que aún 
 *          no se hubiera leido o en un bloque de memoria si ya fue leido
 */
mem_address_t frame_size_arch(void);

/** \brief Recibir el frame 
 *  \details El datasheet trata al frame como packet y así es tratado por 
 *          los drivers consultados, por lo que también utilizaremos packet
 *          o paquete. Ver el datasheet 7.2 para una completa descripción.
 *          Esta función chequea los buffers del ENC28j60 y si hay un paquete
 *          copia su  bloque received_packet.
 *          
 *  \return UIP_RECEIVEBUFFERHANDLE (0xFF): Indica que hay un paquete en el 
 *          buffer de la interface que es viable. La estructura received_packet
 *          contendrá la dirección del paquete en el enc (.begin) y la logitud
 *          (.size). La variable next_packet_ptr será actualizada con la posición
 *          del próximo paquete si hubiera.
 *          NOBLOCK (0x00): No hay paquete disponible.
*/
uint8_t receive_packet_arch(void);

/** 
 *  \brief Leer el paquete desde la NIC
 *  \details Traer el paquete desde la NIC y copiarlo en un buffer
 *  \param handle Manejador del paquete
 *  \param buffer Puntero al buffer donde será copiado el paquete
 *  \param len Tamaño del paquete
 *  \return Tamaño del buffer leido del ENC
 */
uint16_t read_packet_arch(mem_handle_t handle, uint8_t* buffer, uint16_t len);


/** 
 *  \brief Escribir el paquete en la NIC para que lo envie
 *  \details Funciona igual que la dupla recieve - read pero a la inversa 
 *  \param handle Índice del paquete
 *  \param position Posición del paquete en la NIC
 *  \param buffer Puntero al buffer donde está el paquete en RAM
 *  \param len Tamaño del paquete
 *  \return Tamaño del buffer enviado al ENC
 *
 */
void write_packet_arch(uint8_t* tx_buffer, uint16_t len);

/** 
 *  \brief Pedirle a ENC que envíe un paquete
 *  \details Funciona igual que la dupla recieve - read pero a la inversa
 *          donde write_packet pone el paquete en ENC y send le dice a la 
 *          NIC que lo envíe a la red
 *  \param handle Paquete que debe ser enviado
 */
bool send_packet_arch(void);


/** 
 * 
 */
void free_packet_arch(void);

/** 
 * 
 */
void mempool_block_move_callback_arch(mem_address_t dest, mem_address_t src, uint16_t size);


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