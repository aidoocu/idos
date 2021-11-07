/*  */
#include "sleep_arch.h"

void sleep_init_arch(void){
    /*  SLEEP_MODE_PWR_DOWN
        SLEEP_MODE_PWR_SAVE
        SLEEP_MODE_STANDBY
        SLEEP_MODE_EXT_STANDBY
        SLEEP_MODE_IDLE
        SLEEP_MODE_ADC
    */
    set_sleep_mode(SLEEP_MODE_IDLE);

    /* A partir de aquí hay que hacer reconfigurable esto para test */
 
    /* desabilitar el brown-out detection */
    sleep_bod_disable();

}

void sleep_arch(void){

    wdt_enable(WDTO_500MS);
    wdt_reset();
    /* El WDT genera una interrupción y no resetea el sistema */
	WDTCSR |= (1 << WDIE);

    cli();
    /* Set SE (sleep enable) bit */
    sleep_enable();
    sei();

    /* Sleep */
    sleep_cpu();

    /* Desabilitar los modos de sleep */
    sleep_disable();

    sei();
}


ISR (WDT_vect)
{
	// WDIE & WDIF is cleared in hardware upon entering this ISR
	wdt_disable();
    //printf("wdt");
}















// Temporary clock source variable ¿volatile?
unsigned char clockSource = 0;

/* Aquí se habilita todo sin exepción, habria que ver que conviene y que no en realidad */
void power_enable(void){
    // Restore previous setting
    TCCR2B = clockSource;
    
    power_timer2_enable();

    /* habilitar ADC */
    power_adc_enable();
    ADCSRA |= (1 << ADEN);

    power_timer1_enable();
    power_timer0_enable();
    power_spi_enable();
    power_usart0_enable();
    power_twi_enable();    
}


/* Aquí se desabilita todo sin exepción, habria que ver que conviene y que no en realidad */
void power_disable(void){

    // Store current setting
    clockSource = TCCR2B;

    // Remove the clock source to shutdown Timer2
    TCCR2B &= ~(1 << CS22);
    TCCR2B &= ~(1 << CS21);
    TCCR2B &= ~(1 << CS20);

    power_timer2_disable();

    /* Desabilitar ADC */
    ADCSRA &= ~(1 << ADEN);
    power_adc_disable();

    power_timer1_disable();
	power_timer0_disable();
	power_spi_disable();
	power_usart0_disable();
	power_twi_disable();
}