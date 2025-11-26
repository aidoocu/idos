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
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "../../../arch.h"

enum period_t
{
	WDT_15MS,
	WDT_30MS,
	WDT_60MS,
	WDT_120MS,
	WDT_250MS,
	WDT_500MS,
	WDT_1S,
	WDT_2S,
	WDT_4S,
	WDT_8S,
	WDT_FOREVER
};

#define WDT_TIME WDT_15MS

/** \brief   Inicializar el modo sleep */
void sleep_init_arch(void);

/** \brief  Poner a dormir el micro */
void sleep_arch(void);

#endif /* _SLEEP_ARCH_H_ */