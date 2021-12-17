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

/** 
 *  \brief Guarda la dirección de memoria del paquete en el ENC y su tamaño
 */
struct received_frame_t {
    uint16_t size = 0;
    uint16_t begin = 0;
};

/** 
 *  \brief Inicializar el ENC 
 *  \param mac Dirección (máscara) MAC que deberá tener el ENC
*/
bool mac_init_arch(uint8_t * mac);

/** 
 *  \brief Tamaño del frame 
 *  \details El frame puede estar en un buffer en el ENC en caso de que aún 
 *          no se hubiera leido o en un bloque de memoria si ya fue leido
 */
uint16_t frame_size_arch(void);

/** \brief Recibir el frame 
 *  \details El datasheet trata al frame como frame y así es tratado por 
 *          los drivers consultados, por lo que también utilizaremos frame
 *          o frame. Ver el datasheet 7.2 para una completa descripción.
 *          Esta función chequea los buffers del ENC28j60 y si hay un frame
 *          copia su  bloque received_frame.
 *          
 *  \return UIP_RECEIVEBUFFERHANDLE (0xFF): Indica que hay un frame en el 
 *          buffer de la interface que es viable. La estructura received_frame
 *          contendrá la dirección del frame en el enc (.begin) y la logitud
 *          (.size). La variable next_frame_ptr será actualizada con la posición
 *          del próximo frame si hubiera.
 *          NOBLOCK (0x00): No hay frame disponible.
*/
bool receive_frame_arch(void);

/** 
 *  \brief Leer el frame desde la NIC
 *  \details Traer el frame desde la NIC y copiarlo en un buffer
 *  \param buffer Puntero al buffer donde será copiado el frame
 *  \param len Tamaño del frame que efectivamente será leido
 *  \return Tamaño del buffer leido del ENC
 */
uint16_t read_frame_arch(uint8_t* buffer, uint16_t len);


/** 
 *  \brief Escribir el frame en la NIC para que lo envie
 *  \details Funciona igual que la dupla recieve - read pero a la inversa 
 *  \param handle Índice del frame
 *  \param position Posición del frame en la NIC
 *  \param buffer Puntero al buffer donde está el frame en RAM
 *  \param len Tamaño del frame
 *  \return Tamaño del buffer enviado al ENC
 *
 */
void write_frame_arch(uint8_t* tx_buffer, uint16_t len);

/** 
 *  \brief Pedirle a ENC que envíe un frame
 *  \details Funciona igual que la dupla recieve - read pero a la inversa
 *          donde write_frame pone el frame en ENC y send le dice a la 
 *          NIC que lo envíe a la red
 *  \param handle frame que debe ser enviado
 */
bool send_frame_arch(void);


/** \brief  Liberar el paquete leido
 *  \note   Esto reutiliza o libera la memoria que fue leida antes de
 *          llamar esta función 
 */
void free_frame_arch(void);

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