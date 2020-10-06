/**
 * \file    timer.h
 * \brief   Header file para las definiciones del timer en idOS.
 * \author  Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 *
 */

/**
 * \brief   Definiones de los timers del sistema
 * 
 *          Los timers están basados en el conteo de ticks. Un tick es el desvorde 
 *          de un cpu_timer. El timer y el rtimer tendrán un cpu_timer distinto, por
 *          lo que la duración del tick es distindo y define su resolución.
 * 
 * \note    Se implementar dos timer, uno normal que cuando expira envía un
 *          mensaje a la tarea que lo solicitó, y otro, rtimer, que llama a
 *          una función callback en cuanto es llamdo (modo inmediato).
*/

#ifndef _TIMER_H_
#define _TIMER_H_

#include "../idos.h"

/** \typedef    time_usec_t
 *  \brief      Guarda tiempo en usegundos
 */
typedef unsigned long time_usec_t;

/** ----------------- Definiciones de rtimer ------------------ */







/** ----------------- Definiciones de timer ------------------ */


/** \def    TIMER_SECOND    
 *  \brief  Cuantos desvordes del timerX (system ticks) se necesitan para alcanzar 
 *          un segundo. 
 */
#define TIMER_SECOND 66



/** ----------------- Definiciones de tipos de rtimer ------------------ */

/** \struct rtimer_st
 *  \brief  Definición de la estructura que recoge el evento completo de timer
 *  \note   La variable time se debe entender como "en el usec tal" y no como "dentro de time usec"
 */
struct timer_st {
    time_usec_t usec_time;               /**< Momento (en usec) en que ocurrirá la interrupción del rtimer */
    struct timer_st * next;
    void (* rtimer_call)(void *);
};

/** \struct timer_wait_list
 *  \brief  La lista de espera de los timers es una lista no ordenada donde se van poniendo
 *          los timers hasta que sen activados. 
 *  \note   Tener en cuenta que solo está activo un timer a la vez
 */
struct timer_wait_list_st {
    timer_st * begin;
    timer_st * end;
    };

/** ----------------- Definiciones de funciones de rtimer ------------------ */

/** \brief  Inicializar el timer  
 * 
*/
void timer_init(void);


/** \brief  Función que setea un rtimer
 * 
 *          Esta función verifica que exista un rtimer activo (!!!!!!!):
 *          Si rtimer no está activo, pues se activa con el valor USEC_TO_TIME(usec_time).
 *  \note   En dependencia del prescalador activo, a partir de 64 se introducira un error, 
 *          ver RTIMER_PRESCALER
 *          
 *          Si rtimer está activo, si comparará el usec_time. Si usec_time es mayor que el 
 *          activo se agrega a la lista de espera el nuevo rtimer. Si es menor, el rtimer
 *          activo se dehabilita y se agrega a la lista de espera y se activa el nuevo rtimer
 *  \return !!!!!!!!!!!
 */
uint8_t timer_set(timer_st * timer, time_usec_t time_usec);


#endif  //_TIMER_H_