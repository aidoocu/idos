/** \file       timer_arch.h
 *  \brief      En caso que sea un Arduino avr se configura el Timer1 como timer de sistema.
 *      
 *  \note       
 *  \attention  
 *
 *  \author     Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 */
/** 
 * */

#ifndef _TIMER_ARCH_H_
#define _TIMER_ARCH_H_

#include "../../../arch.h"

typedef uint32_t utime_t;
typedef uint32_t mtime_t;

/* -------------------------- Funciones ----------------------------------- */

/** \brief  Inicializamos el contador de TICKs del sistema  
 * 
*/
void timer_sys_init_arch(void);


#endif /* _TIMER_ARCH_H_ */