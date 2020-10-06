/**
 * \file    timer.c
 * \brief   Implementación del rtimer en idOS.
 * \author  Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 *
 */
#include "timer.h"

//#define _DEBUG_TIMER_

static struct timer_wait_list_st timer_wait_list {
    NULL,
    NULL
};

/** \brief  Puntero al timer activo */
static timer_st * time_active = NULL;

void timer_init(void){

    rtimer_init_arch();
    
}


uint8_t timer_set(timer_st * timer, time_usec_t time_usec) {

    /* Guardamos en el timer cuando deberá activarse lo antes posible para minimizar errores*/
    timer->usec_time = time_usec + MICROS;

    #ifdef _DEBUG_TIMER__
    Serial.print("Timer en el usec: ");
    Serial.println(timer->usec_time);
    #endif

    /* Si el tiempo es mayor que el desvorde del timer */
    /** \todo   no devolver un error y ser capaz de programar un timer mayor que MAX_USEC */
    if(time_usec > MAX_USEC) {
        return 1;
    }

    /* Aqui es donde se verifica que no hay un timer corriendo */
    if(!IS_TIMER_SET){

        /* Si no hay un timer corriendo se setea el timer el que entró como parámentro */
        time_active = timer;
        TIMER_COMP = USEC_TO_CLK(time_usec);
        TIMER_TEMP = 0x0000;

        #ifdef _DEBUG_TIMER_
        Serial.print("Timer en CLK: ");
        Serial.println(TIMER_TEMP);
        Serial.print("Comp en CLK: ");
        Serial.println(TIMER_COMP);
        Serial.print("Ahora us: ");
        Serial.println(MICROS);
        #endif
        
        /* Activa la interrupción por comparación del Timer */
        SEI_TIMER();
        
    
        return 0;
    }

    /* Si hay un timer programado, poner en la lista de espera de timers */
    if (timer_wait_list.begin == NULL) {
        timer_wait_list.begin = timer;
    } else {
        /* Poniendo a timer como último elemento de la lista */
        timer_wait_list.end->next = timer;
        timer_wait_list.end = timer;
        timer_wait_list.end->next = NULL;
    }

    return 0;
}