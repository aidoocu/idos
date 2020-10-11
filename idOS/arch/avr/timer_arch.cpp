/** Inicializamos desde el hardware específico los dos timers */

#include "timer_arch.h"


void rtimer_init_arch(void){

    /* Limpiar por completo  los registros de configuración de Timer1 TCCR1A/B */
    TCCR1A = 0x00;
    TCCR1B = 0x00;

    /* Incializamos el comparador TIMER_COMP y temporizador TIMER_TEMP */
    OCR1A = 0x0000;
    TCNT1 = 0x0000;

    /* Habilitar el modo CTC, (bit WGM12) pone a 0 (limpia) el temporizador (TIMER_TEMP) ante una 
    coincidencia con OCR1A */
    //TCCR1B = TCCR1B | 8;
    TCCR1B |= (1 << WGM12);


    /** \def Configurar el prescalador             CS12 CS11 CS10 */
    #if TIMER_PRESCALER == 1
    TCCR1B = TCCR1B | 1;                /**< TCCR1B == b0000 0001 */
    #endif
    #if TIMER_PRESCALER == 8
    TCCR1B = TCCR1B | 2;                /**< TCCR1B == b0000 0010 */
    #endif
    #if TIMER_PRESCALER == 64
    TCCR1B = TCCR1B | 3;                /**< TCCR1B == b0000 0011 */
    #endif
    #if TIMER_PRESCALER == 256
    TCCR1B = TCCR1B | 4;                /**< TCCR1B == b0000 0100 */
    #endif
    #if TIMER_PRESCALER == 1024
    TCCR1B = TCCR1B | 5;                /**< TCCR1B == b0000 0101 */
    #endif

    /* Nos aseguramos que la interrupción comience desactivada */
    TIMSK1 = 0;

}



