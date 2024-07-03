/** 
 * @file sleep_arch.cpp
 * @brief Implementation of sleep architecture functions for AVR ATmega328P.
 * 
 * Each sleep/wake cycle will be considered a system tick. Remember that normal 
 * timers work by time expiration detection so not real time. To sleep/wake up, 
 * the WDT is activated for the length of time you want the cycle to last and 
 * you go to sleep. Once the WDT expires and causes an interruption, this is 
 * attended to in the WDT call back and disables it so that mote does what it 
 * needs to do until the scheduler voluntarily puts it to sleep.
 * 
 * The 
 */

#include "sleep_arch.h"
#include "../lib/low_power/LowPower.h"

/**
 * @brief External variable representing the value of the timer0 milliseconds counter.
 * 
 * This variable is used to keep track of the number of milliseconds elapsed since the program started.
 * It is updated by the timer0 interrupt handler. This variable is declared as extern because the original
 * ones is implemented in the Arduino framework, and we don't want to modify it.
 */
extern unsigned long timer0_millis;

#define SLEEP_TIME SLEEP_15MS

/**
 * @brief Initializes the sleep architecture.
 * 
 * This function sets the sleep mode to SLEEP_MODE_IDLE.
 * 
 * @note This function needs to be reconfigurable for testing purposes.
 */
void sleep_init_arch(void){

    /*  SLEEP_MODE_PWR_DOWN
        SLEEP_MODE_PWR_SAVE
        SLEEP_MODE_STANDBY
        SLEEP_MODE_EXT_STANDBY
        SLEEP_MODE_IDLE
        SLEEP_MODE_ADC
    */
    //set_sleep_mode(SLEEP_MODE_IDLE);

}

/**
 * @brief Puts the CPU into sleep mode.
 * 
 * This function disables power to all peripherals except Timer 0,
 * enables the Watchdog Timer to wake up the CPU after a specified time,
 * and puts the CPU into sleep mode.
 * 
 * @note The Watchdog Timer generates an interrupt and does not reset the system.
 */
void sleep_arch(void){

    //Serial.print("S: ");
    //Serial.println(millis());

    //delay(10);

    LowPower.powerDown(SLEEP_TIME, ADC_OFF, BOD_OFF);

    /** Cuando se va a dormir los temporizadores se duermen y se deja de contar 
     * el tiempo por lo que hay que "poner en hora" los registros de millis().
     * para ello se ha declado como extern en este archivo la variable timer0_millis
     * que es la que lleva la cuenta de los milisegundos del sistema.
    **/
    uint8_t oldSREG = SREG;
	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
    
    /** @todo esto hay que hacerlo funcionar bien    */
    /*#if SLEEP_TIME == SLEEP_15MS
    timer0_millis = timer0_millis + 15;
    #elif SLEEP_TIME == SLEEP_30MS
    timer0_millis = timer0_millis + 30;
    #elif SLEEP_TIME == SLEEP_60MS
    timer0_millis = timer0_millis + 60;
    #elif SLEEP_TIME == SLEEP_120MS
    timer0_millis = timer0_millis + 120;
    #elif SLEEP_TIME == SLEEP_250MS
    timer0_millis = timer0_millis + 250;
    #elif SLEEP_TIME == SLEEP_500MS
    timer0_millis = timer0_millis + 500;
    #elif SLEEP_TIME == SLEEP_1S
    timer0_millis = timer0_millis + 1000;
    #elif SLEEP_TIME == SLEEP_2S
    timer0_millis = timer0_millis + 2000;
    #elif SLEEP_TIME == SLEEP_4S
    timer0_millis = timer0_millis + 4000;
    #elif SLEEP_TIME == SLEEP_8S
    timer0_millis = timer0_millis + 8000;
    #endif */

    if(SLEEP_TIME == SLEEP_15MS)
        timer0_millis = timer0_millis + 15;
    else if(SLEEP_TIME == SLEEP_30MS)
        timer0_millis = timer0_millis + 30;
    else if(SLEEP_TIME == SLEEP_60MS)
        timer0_millis = timer0_millis + 60;
    else if(SLEEP_TIME == SLEEP_120MS)
        timer0_millis = timer0_millis + 120;
    else if(SLEEP_TIME == SLEEP_250MS)
        timer0_millis = timer0_millis + 250;
    else if(SLEEP_TIME == SLEEP_500MS)
        timer0_millis = timer0_millis + 500;
    else if(SLEEP_TIME == SLEEP_1S)
        timer0_millis = timer0_millis + 1000;
    else if(SLEEP_TIME == SLEEP_2S)
        timer0_millis = timer0_millis + 2000;
    else if(SLEEP_TIME == SLEEP_4S)
        timer0_millis = timer0_millis + 4000;
    else if(SLEEP_TIME == SLEEP_8S)
        timer0_millis = timer0_millis + 8000;
        

	SREG = oldSREG;

    //delay(10);

    //Serial.print("W: ");
    //Serial.println(millis());
}

// Temporary clock source variable ¿volatile?
unsigned char clockSource = 0;

/**
 * @brief Enables power to all peripherals.
 * 
 * This function restores the previous clock source setting for Timer 2,
 * enables power to Timer 2, ADC, Timer 1, SPI, USART0, and TWI.
 * 
 * @note This function enables all peripherals without exception.
 */
void power_enable(void){
    // Restore previous setting
    TCCR2B = clockSource;
    
    power_timer2_enable();

    /* habilitar ADC */
    power_adc_enable();
    ADCSRA |= (1 << ADEN);

    power_timer1_enable();
    power_spi_enable();
    power_usart0_enable();
    power_twi_enable();    
}

/**
 * @brief Disables power to all peripherals.
 * 
 * This function stores the current clock source setting for Timer 2,
 * removes the clock source to shutdown Timer 2, disables power to Timer 2,
 * ADC, Timer 1, SPI, USART0, and TWI.
 * 
 * @note This function disables all peripherals without exception.
 */
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
	power_spi_disable();
	power_usart0_disable();
	power_twi_disable();
}

/**
 * @brief Watchdog Timer interrupt service routine.
 * 
 * This function is executed when the Watchdog Timer times out.
 * It disables the Watchdog Timer.
 */
/* ISR (WDT_vect) {
	// WDIE & WDIF is cleared in hardware upon entering this ISR
	wdt_disable();
    //printf("WDT\n");
    Serial.println("WDT");
} */