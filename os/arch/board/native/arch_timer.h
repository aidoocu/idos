/** 
 * 
 * 
 * 
 */

#ifndef _ARCH_TIMER_H_
#define _ARCH_TIMER_H_
/* Tipo de dato para guardar tiempo, sea en mseg o usec */
typedef unsigned long time_t; 


/** 
 * \brief   Devuelve los milisegundos del sistema 
 * */
time_t  msec_now();

/** 
 * \brief   Devuelve los microsegundos del sistema 
 * */
time_t usec_now();

/////// Esto no deberia estar definido
void timer_sys_init_arch(void);

#endif