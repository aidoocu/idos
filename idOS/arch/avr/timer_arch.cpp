/** Inicializamos desde el hardware específico los dos timers */

#include "timer_arch.h"

/* Cantidad de msec desde que el sistema arrancó */
static volatile uint32_t sys_msec;


void timer_sys_init_arch(void){
    
    /* Prescaler de 8 (bit 2) + Habilitar el modo CTC, (bit WGM12 (3)) pone a 0 (limpia) el temporizador (TIMER_TEMP) ante una 
    coincidencia con OCR0A */
    TCCR1B = TCCR1B | 10;                /**< TCCR1B == b0000 1010 */

    
    /* Incializamos el comparador 2000 - 1 (demora + 1 en volver a 0), y temporizador (cada 1ms) */
    OCR1A = 1999;
    TCNT1 = 0;

    /* Se activa la interrupción por comparación */
    TIMSK1 = 2;                         /**< TIMSK1 == b0000 0010 */

    //TIFR1 = 0;

    sys_msec = 0;

    return;
}
/** Interrupción para contar milisegundos */
ISR(TIMER1_COMPA_vect){
    sys_msec ++;
}

mtime_t m_now(void){
    
    /* Se guarda SREG */
    uint8_t s_reg = SREG;
    /* Para deshabilitar las int y falseen mtime */
    cli();

    /* Se leen los mseg acumulados, timer1 dispara cada 1ms */
    mtime_t mtime = sys_msec;
    
    /* Se reactivan las INT */
    SREG = s_reg;
    
    return(mtime);
}

/* utime_t u_now(void){
    
} */

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
    //TCCR1B |= (1 << WGM12);
    //TCCR0A |= (1 << WGM01);


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



