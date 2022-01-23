/**
 * \file    timer.c
 * \brief   Implementación del rtimer en idOS.
 * \author  Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 *
 *          El timer se implementa como una estructura que debe ser declarada antes del pt, un
 *          buen momento es al momento de declarar la tarea. Un timer puede puede ser utilizado
 *          de forma inmedita (rtimer) o para disparar un evento del timer (timer). El timer 
 *          siempre es disparado por una interrupción. Cuando se ha seteado como timer, la interrupción
 *          le pasa un mensaje a la tarea que ha seteado el timer (simplemente la despierta) 
 *          pasándole MSG_TIMER como tipo de mensaje. Cuando se setea como rtimer, se llama a la
 *          función callback asociada al timer.
 */

#include "timer.h"

/** 
 * \brief   Apunta al primer timer de la lista
 */
static struct timer_st * timer_list = NULL;

/** 
 * \brief   Recorre la lista de timers y siempre se queda al final
 */
static struct timer_st * timer_index = NULL;


/* Si estamos con el framework Arduino ya este inicializa el timer */
#ifndef ARDUINO
/* Si la plataforma es native usamos el timestamp del sistema */
#if BOARD != NATIVE
void timer_sys_init(void){
    timer_sys_init_arch();
}
#endif /* BOARD != NATIVE */
#endif /* ARDUINO */

void timer_set_sys(timer_st * timer, time_t msec, task_st * task){
   
    /* Inicializamos el timer */

    timer->active = true;
    timer->msec = msec;
    timer->msec_time = msec + msec_now();    
    timer->next = NULL;
    timer->task = task;

    
    /* Se pone el timer en la lista de timers */
    if (timer_list == NULL){
        /* Si no ningún timer creado, hay que inicializar la lista */
        timer_list = timer;
        timer_index = timer;
    } else {
        /* Si hay un timer  */
    timer_index->next = timer;
    timer_index = timer;
    }
}

void timer_reset(timer_st * timer){

    /* Limpiar el mensaje */
    timer->msg.status = MSG_NULL;
    /* Reactivar el timer */
    timer->active = true;
    /* Reiniciar conteo */
    timer->msec_time = timer->msec + msec_now();

}

void timer_exec(void){

    /* Verificar si hay algun timer esperando y si no hay, salir */
    if (timer_list){

        /* Ir al principio de la lista de timers */
        timer_index = timer_list;
        /* Recorrer la lista timers buscado el timer activo */
        do {       
            if (timer_index->active == true){
                /* Si este timer está activo se verifica si su tiempo llegó */
                if(timer_index->msec_time <= msec_now()){
                    /* Le envío el mensaje a la tarea que setea el timer */
                    ipc_msg_timer(timer_list);
                                        
                    /* Se baja la bandera de active a este timer que se acaba de ejecutar */
                    timer_index->active = false;
                }
            }
            /* Cargo el próximo timer si existe */
            if (timer_index->next){
                timer_index = timer_index->next;
            }
        } while (timer_index->next != NULL);
    }

    return; 
}

/** 
 * 
 */
bool timer_expired(timer_st * timer) {
    
    if(timer->msg.status == MSG_AVAILABLE)
        return true;
    return false;
}

void timer_disable(timer_st * timer) {

    /* Quitamos el último estado del mensaje */
    timer->msg.status = MSG_NULL;
    
    /* Desactivamos el timer*/
    timer->active = false;
}