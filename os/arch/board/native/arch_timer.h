/** 
 * 
 * 
 * 
 */

#ifndef _ARCH_TIMER_H_
#define _ARCH_TIMER_H_

//#include <time.h>
#include <sys/time.h>


/** 
 * \brief   Devuelve los milisegundos del sistema 
 * */
time_t  msec_now();

/** 
 * \brief   Devuelve los microsegundos del sistema 
 * */
time_t usec_now();


#endif