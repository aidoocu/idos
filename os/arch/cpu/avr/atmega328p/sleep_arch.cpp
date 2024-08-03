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
/**
 * @brief External variable representing the value of the timer0 milliseconds counter.
 * 
 * This variable is used to keep track of the number of milliseconds elapsed since the program started.
 * It is updated by the timer0 interrupt handler. This variable is declared as extern because the original
 * ones is implemented in the Arduino framework, and we don't want to modify it.
 */
extern unsigned long timer0_millis;

volatile unsigned long wdt_duration = 0;
volatile unsigned long wdt_init_time = 0;

/**
 * @brief Initializes the sleep architecture.
 * 
 * This function sets the sleep mode to SLEEP_MODE_IDLE.
 * 
 * @note This function needs to be reconfigurable for testing purposes.
 */
void sleep_init_arch(void){

   // Disable interrupts
    cli();

    wdt_init_time = millis();
    
    /* start wdt */
    wdt_enable(WDT_TIME);
    /* Enable the WDT interrupt (no reset). */
	WDTCSR |= (1 << WDIE);
    WDTCSR |= ~(1 << WDE);

    //Enable global interrupts
    sei();

    wdt_duration = 0;

    int i = 0;
    
    while (1) {
        if (wdt_duration == 1) {
            break;
        }
    }
    
    // wait for interrupt
    wdt_duration = millis() - wdt_init_time;
    
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

    /* Disable ADC */
    ADCSRA &= ~(1 << ADEN);

    /* Enable the sleep mode */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    cli();
    sleep_enable();
	sleep_bod_disable();

    /* Enable int... */
    sei();
    /* ...and go to sleep */
    sleep_cpu();

    /* The program will continue from here after the WDT timeout */
    sleep_disable();
    /* Enable ADC */
    ADCSRA |= (1 << ADEN);

    /** Cuando se va a dormir los temporizadores se duermen y se deja de contar 
     * el tiempo por lo que hay que "poner en hora" los registros de millis().
     * para ello se ha declado como extern en este archivo la variable timer0_millis
     * que es la que lleva la cuenta de los milisegundos del sistema.
    **/
   	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
    uint8_t oldSREG = SREG;
	cli();

    timer0_millis = timer0_millis + wdt_duration;
        
	SREG = oldSREG;
    sei();
}

ISR (WDT_vect){
	// WDIE & WDIF is cleared in hardware upon entering this ISR
	//wdt_disable();

    if(wdt_duration == 0)
        wdt_duration = 1;

    wdt_enable(WDT_TIME);
    /* Enable the WDT interrupt (note no reset). */
	WDTCSR |= (1 << WDIE);
}