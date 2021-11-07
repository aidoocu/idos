/** \file       sleep_arch.h
 *  \brief      .
 *      
 *  \note       
 *  \attention  
 *
 *  \author     Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 */
/** 
 * */

#ifndef _SLEEP_ARCH_H_
#define _SLEEP_ARCH_H_

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include "../../../arch.h"

/* \brief   Inicializar el modo sleep */
void sleep_init_arch(void);

/* \brief   Poner a dormir el micro */
void sleep_arch(void);

#endif /* _SLEEP_ARCH_H_ */