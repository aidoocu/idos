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
    struct msg_st msg;                    /** < Mensaje a la tarea que seteó el timer */
};

/* --------------------------------------------------------------------------------- */

/** \def    timer_set
 *  \brief  Crea y setea un timer (timer_name) para que dispare dentro de msec
 *  \param  timer_name  El nombre del timer
 *  \param  msec        Dentro de cuantos milisegundo el timer debe disparar
 *          
*/
#define timer_set(timer_name, msec)                     \
                static struct timer_st timer_name;      \
                timer_set_sys(&timer_name, msec, task)  \


/* --------------------------------------------------------------------------------- */

/**
 * \def     ipc_msg_timer
 * \brief   Macro para que un timer envie un mensaje a una otra tarea
 * \details Solo las tareas setean timers, y cada timer contiene en su estructura un 
 *  puntero a la tarea que lo seteó. La única fúnción de un timer es despertar a su 
 *  tarea en un momento concreto. Si la tarea espera solo un evento que la despierte
 *  podría (aunque no sería una buena práctica) para por alto el origen y resetear el 
 *  timer.
 *  \param  timer El timer que debe pasar el mensaje a su tarea
 */
#define ipc_msg_timer(timer)                    \
            do {                                \
                timer->msg = {                  \
                    MSG_AVAILABLE,              \
                    MSG_TIMER,                  \
                    0,                          \
                    __null                      \
                };                              \
                task_set_ready(timer->task);    \
            } while (0)

/** 
 *  \brief Determina si el timer ha expirado o no.
 *  \note Con este macro se verifica el mensaje del timer pasivamente, no lo modifica
 *  \param timer El timer que se va a verificar
 *  \return true si el timer ha expirado, false si no.
 */
bool timer_expired(timer_st * timer);

/** 
 *  \brief Lee el timer para determinar si ha expirado
 *  \details Esta función hace lo mismo que timer_expired verificando que el timer ha expirado
 *  con la diferencia de que si determina que el efectivamente ha expirado, queda como "leido"
 *  por lo que el mensaje se limpia. 
 *  \note Este procedimiento modifica el mensaje desabilitandolo
 *  \param timer El timer que se va a leer
 *  \return true si el timer ha expirado, false si no.
 */
bool timer_read(timer_st * timer);

/* --------------------------------------------------------------------------------- */

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