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

#include "../netstack.h"


/* Encabezados de las frame */
/* !!!! ToDo el resto de los encabezados, CSMA...... */

/** 
 * 
 */
static mem_handle_t uip_packet = NOBLOCK;

/* Longitud del paquete recibido */
//static uint8_t uip_hdrlen = 0;

/* Estado del paque recibido */
static uint8_t packet_state = 0;


/* Conjunto de primitivas mac */


/** \brief Inicializa la NIC 
 * \param mac Máscara MAC, para eth, un arreglo de 6 bytes
*/
bool mac_init(uint8_t * mac_address);



/** 
 * 
 */
bool mac_send(void);

#endif /* _MAC_H_ */