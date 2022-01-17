/** 
 * 
 * 
 */

#ifndef _NET_NATIVE_H_
#define _NET_NATIVE_H_

#include <stdbool.h>
#include <stdint.h>

/** 
 *  \brief Inicializar la interface 
 *  \param mac Dirección (máscara) MAC que deberá tener la interface
*/
bool mac_init_arch(uint8_t * mac);

/** 
 *  \brief Tamaño del frame 
 *  \details El frame puede estar en un buffer en la interface en caso de que aún 
 *          no se hubiera leido o en un bloque de memoria si ya fue leido
 */
uint16_t frame_size_arch(void);


/** 
 * 
 * 
 */
bool receive_frame_arch(void);

/** 
 *  \brief Leer el frame desde la NIC
 *  \details Traer el frame desde la NIC y copiarlo en un buffer
 *  \param buffer Puntero al buffer donde será copiado el frame
 *  \param len Tamaño del frame que efectivamente será leido
 *  \return Tamaño del buffer leido dla interface
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
 *          donde write_frame pone el frame en la NIC y send le dice a la 
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


#endif /* _NET_NATIVE_H_ */