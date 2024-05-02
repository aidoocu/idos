/** 
 * 
 * 
 * \details El mecanismo de acceso al mendio puede estar definido por 
 *          hardware o por software en dependencia de la interface conque
 *          se cuente. En este archivo cargaran las definiciones adecuadas
 *          según que NIC haya sido configurada 
 * */

#ifndef _MAC_H_
#define _MAC_H_

#include "../../../arch/arch.h"

/* ------------------------------- arch block -------------------------------- */

/** 
 *  \brief Inicializar el ENC 
 *  \param mac Dirección (máscara) MAC que deberá tener el ENC
 */
bool mac_init(uint8_t * mac);

/** 
 *  \brief Escribir  
 *  \param mac Dirección (máscara) MAC que deberá tener el ENC
*/
void mac_get_address(uint8_t * mac);

/** 
 *  \brief  Poll a la ENC para ver si ha llegado algo
 *  \details El datasheet trata al frame como packet y así es tratado por 
 *          los drivers consultados. No obstante aquí le llamaremos frame.
 *          Ver el datasheet 7.2 para una completa descripción.
 *  \param frame    Buffer donde será copiada la frame, de haber recibido una.
 *  \return Tamaño de la frame recibida. 0 si no se ha recibido nada.
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

#endif /* _MAC_H_ */