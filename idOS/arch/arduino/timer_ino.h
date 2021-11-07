#ifndef _TIMER_INO_H_
#define _TIMER_INO_H_

#include "../arch.h"

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

#endif //_TIME_INO_H_