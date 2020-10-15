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

/** \brief  Lista de espera de timers. Tiene un puntero al inicio y otro al final
 *          
 *          Si (.begin != __null) && (.end == __null) ilegal
 *          Si (.begin == __null) && (.end == __null) no hay ningún timer en la lista
 *          Si (.begin != __null) && (.begin == .end) hay un solo elemento en la lista
 *          Si (.begin != __null) && (.begin != .end) hay múltiples elementos en la lista
 */
/////////!!!!!!!! parece no hacer falta un puntero al final de la lista de espera
/* static volatile struct timer_wait_list_st timer_wait_list {
    __null,
    __null
}; */

/** \brief  La lista de espera es un puntero a su primer elemento
 * 
 *          La lista de espera se ordena en función del tiempo en el que debe disparar los timers que 
 *          la componen. Su primer elemento siempre es el próximo disparo
 */
static struct timer_st * timer_wait_list = __null;


/** \brief  Puntero al timer activo */
static volatile timer_st * timer_active = __null;


void timer_init(void){

    rtimer_init_arch();
    
}

void timer_sys_init(void){

    timer_sys_init_arch();

}

uint8_t rtimer_set(timer_st * timer, utime_t time_usec) {
    

    /* Si el tiempo es mayor que el desvorde del timer */
    /** \todo   no devolver un error y ser capaz de programar un timer mayor que MAX_USEC */
    if(time_usec > MAX_USEC) {
        return 1;
    }

    if(!IS_TIMER_SET){
    
        /* Se setea el timer el que entró como parámentro */
        timer_active = timer;

        TIMER_COMP = USEC_TO_CLK(time_usec);
        TIMER_TEMP = 0x0000;

        /* Activa la interrupción por comparación del Timer */
        SEI_TIMER();
    
        return 0;
    }

    return 0;
}

void timer_set(timer_st * timer){

    ////////////!!!!! hay que reevaluar todo este analisis porque el timer_wait_list puede ser modificado por la INT con resultado cagastroficos
    /* timer_wait_list es leida y modificada por una INT frecuente, así que debe ser protegida */
    uint8_t s_reg = SREG;
    cli();

    /* Establezco un indice temporal para recorrer la cola buscando donde va el timer, que estará ordenado
    en base al msec. */
    struct timer_st * timer_index = timer_wait_list;

    SREG = s_reg;

    /* Si no hay nigun timer en la lista de espera */
    if (timer_index == __null){
        /* timer es el primero de la cola */
        timer_wait_list = timer;
        return;
    }


    
    /* Mientras exista un próximo timer en la lista de espera */
    while (1) {
        /* Si el primero tiene un tiempo después de timer */
        if (timer_index->msec_time > timer->msec_time){
            /* timer va primero entonces */
            timer->next = timer_index;
            timer_index = timer;

            return;
        }
        /* Si este fuera ya el último elemento de la lista de espera */
        if (timer_index->next == __null){
            /* Entonces timer.msec_time ha sido mayor que todos y será el último entonces */
            timer_index->next = timer;
        }
        timer_index = timer_index->next;
    }
}

void timer_exec(void){

    /* Verificar si hay algun timer esperando y si no hay, salir */
    if (timer_wait_list == __null){
        return; 
    }

    /* Le envío el mensaje a la tarea que setea el timer */
    MSG_TIMER_SEND(timer_wait_list->task, __null);
    /* Paso al próximo elemento de la lista de espera */
    timer_wait_list = timer_wait_list->next;

}

ISR(TIMER0_COMPA_vect){
    //time_usec_t tiempo = MICROS;
    if (timer_active->rtimer_call == __null){
        MSG_TIMER_SEND(timer_active->task, __null);
        CLI_TIMER();
    } else {
        timer_active->rtimer_call();
        CLI_TIMER();
    }
}