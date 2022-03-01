/** 
 * 
 * 
 * 
 */

#ifndef _TIMER_ESP_H_
#define _TIMER_ESP_H_

#include <Arduino.h>

/** 
 * \brief   Devuelve los milisegundos del sistema 
 * */
time_t  msec_now();

/** 
 * \brief   Devuelve los microsegundos del sistema 
 * */
time_t usec_now();

#endif /* _TIMER_ESP_H_ */