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
 *  \brief Indicador de un paquete recibido
 *  \details Puede contener 3 tipos de valores:
 *          - 0x00: No hay paquete alguno en ningún buffer
 *          - 0xFF: Hay un paquete en NIC listo para ser leído
 *          - Otro número: Hay un paquete en un bloque de RAM. En este caso
 *            el propio in_packet es el índice dentro del arreglo de bloques
 *            de RAM donde está el paquete.
 */
static mem_handle_t in_packet = NOBLOCK;

/** 
 * 
 */
static mem_handle_t uip_packet = NOBLOCK;

/* Longitud del paquete recibido */
static uint8_t uip_hdrlen = 0;

/* Estado del paque recibido */
static uint8_t packet_state = 0;


/* Conjunto de primitivas mac */


/** \brief Tamaño del bloque */
uint16_t block_size(mem_handle_t packet);

/** \brief Recibir paquetes TCP???? 
 * \return Bandera que indica que hay al menos un paquete en el buffer
 *          de la interface:
 *          UIP_RECEIVEBUFFERHANDLE (0xFF): Indica que hay un paquete en el 
 *          buffer de la interface que es viable. En este caso un bloque de 
 *          memoria definido en el driver apuntará al buffer y guardará otros
 *          datos
 *          NOBLOCK (0x00): No hay paquete disponible
*/
mem_handle_t receive_packet(void);

/** Leer el paquete desde la NIC
 *  \param handle Manejador del paquete
 *  \param position Posición del paquete en la NIC
 *  \param buffer Buffer de uip a donde va el paquete
 *  \param len Tamaño del paquete
 */
//uint16_t read_packet (mem_handle_t handle, mem_address_t position, uint8_t* buffer, uint16_t len);


/** \brief Inicializa la NIC 
 * \param mac Máscara MAC, para eth, un arreglo de 6 bytes
*/
bool mac_init(uint8_t * mac_address);



/** 
 * 
 */
bool mac_send(void);

#endif /* _MAC_H_ */