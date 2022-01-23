/** 
 * 
 * \details Una interfaz TAP es un dispositivo virtual de la capa de enlace. La
 *  capa física se maneja con un descriptor de archivo (fd) que la representa.
 *  Luego se escriben datos de frame Ethernet sin procesar en el fd que serán 
 *  tratados como cualquier frame Eth. 
 * */

#ifndef _NET_NATIVE_H_
#define _NET_NATIVE_H_

#include "arch_sys.h"

#ifdef Darwin
#define DEVTAP "/dev/tap0"
#elif Linux
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#define DEVTAP "/dev/net/tun"
#else 
#define DEVTAP "/dev/null"
#endif

/** 
 *  \brief Inicializar la interface 
 *  \param mac Dirección (máscara) MAC que deberá tener la interface
*/
bool mac_init(uint8_t * mac);


/** 
 *  \brief Poll a la NIC para ver si ha llegado algo
 *  \param frame    Buffer donde será copiada la frame de haber recibido una
 *  \param max_len  Maximum length of the buffer
 */
uint16_t mac_poll(uint8_t * frame, uint16_t max_len);


/** 
 * \brief Enviar a la MAC en frame que será puesto en la red
 */
bool mac_send(uint8_t * frame, uint16_t len);




///// Quitar !!!!
/** \brief  Liberar el paquete leido
 *  \note   Esto reutiliza o libera la memoria que fue leida antes de
 *          llamar esta función 
 */
void free_frame_arch(void);



/** 
 *  \brief Estado de la interface
 */
uint8_t nic_is_active(void);
bool nic_link_status(void);

/** 
 *  Control de encendido y apagado
 */
void nic_power_off(void);
void nic_power_on(void);


#endif /* _NET_NATIVE_H_ */