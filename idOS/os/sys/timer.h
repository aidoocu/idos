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

/** 
 * \brief   Estructura del timer
 */
struct timer_st {
    bool active;                            /** < Indica si este timer está activo o no */
    time_t  msec;                           /** < Guardamos el intervalo para poder hacer reset */
    time_t msec_time;                       /** < Momento (en msec) en que ocurrirá la interrupción del timer */
    struct timer_st * next;                 /** < Próximo timer en la lista */
    struct task_st * task;                  /** < Puntero a la tarea que seteó el timer */
};

/** ----------------- macros de timer ------------------ */

/** \def    TIMER_INIT
 *  \brief  Crea y setea un timer (timer_name) para que dispare dentro de msec
 *  \param  timer_name  El nombre del timer
 *  \param  msec        Dentro de cuantos milisegundo el timer debe disparar
 *          
*/
#define timer_set(timer_name, msec)                     \
                static struct timer_st timer_name;      \
                timer_set_sys(&timer_name, msec, task)  \



/** ----------------- Definiciones de funciones de timer ------------------ */

/* Si estamos con el framework Arduino ya este inicializa el timer */
#ifndef ARDUINO
/** \brief  Arranca el conteo de TICKs para los timer de sistema  
 * 
*/
void timer_sys_init(void);
#endif /* ARDUINO */

/** \brief  Crea y setea el timer 
 *  \param  timer_name  El nombre del timer
 *  \param  msec        Dentro de cuantos milisegundo el timer debe disparar
 *  \param  task        La tarea que setea el timer
*/
void timer_set_sys(timer_st * timer, time_t time, task_st * task);

/** \brief   Resetear el timer */
void timer_reset(timer_st * timer);

/** \brief      Función que verifica si hay un timer activo y está listo para correr. 
 * 
 *  \note       Si ya hay un timer listo se le pasa un mensaje a la tarea que lo invocó.
 */ 
void timer_exec(void);

#endif  //_TIMER_H_