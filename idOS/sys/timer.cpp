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

//#define _DEBUG_TIMER_
/* 
static struct timer_wait_list_st timer_wait_list {
    NULL,
    NULL
}; */

/** \brief  Puntero al timer activo */
static timer_st * timer_active = __null;


void timer_init(void){

    rtimer_init_arch();
    
}


uint8_t timer_set(timer_st * timer, time_usec_t time_usec) {
    
    /* Si el tiempo es mayor que el desvorde del timer */
    /** \todo   no devolver un error y ser capaz de programar un timer mayor que MAX_USEC */
    if(time_usec > MAX_USEC) {
        return 1;
    }

    if(!IS_TIMER_SET){
    
        /* Se setea el timer el que entró como parámentro */
        timer_active = timer;

        /* Activa la interrupción por comparación del Timer */
        SEI_TIMER();
    
        return 0;
//    }

    /* Si hay un timer programado, poner en la lista de espera de timers */
//    if (timer_wait_list.begin == NULL) {
//        timer_wait_list.begin = timer;
//    } else {
//        /* Poniendo a timer como último elemento de la lista */
//        timer_wait_list.end->next = timer;
//        timer_wait_list.end = timer;
//        timer_wait_list.end->next = NULL;
//    }

    return 0;
}

ISR(TIMER1_COMPA_vect){
    //time_usec_t tiempo = MICROS;
    if (timer_active->rtimer_call == __null){
        MSG_TIMER_SEND(timer_active->task, __null);
        CLI_TIMER();
    } else {
        timer_active->rtimer_call();
        CLI_TIMER();
    }
}