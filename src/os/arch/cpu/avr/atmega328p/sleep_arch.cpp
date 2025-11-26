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
 * @warning This implementation is only for the ATmega328P microcontroller.
 * El Watchdog Timer (WDT) en el ATmega328 utiliza un oscilador RC interno de 
 * baja frecuencia (128 kHz) como su fuente de reloj. La frecuencia del oscilador 
 * RC puede variar significativamente. Según las especificaciones del ATmega328, 
 * la frecuencia del oscilador RC interno puede variar en un rango amplio, típicamente 
 * ±10% a ±20%. Esto significa que los tiempos del WDT pueden variar en un rango 
 * similar. Debido a esto no se puede confiar en absoluto en los tiempos del WDT.
 * Debido a esto en la función sleep_init_arch() se ha implementado un conteo para
 * determinar aproximadamente el tiempo en milisegundos que dura el WDT, que luego
 * se sumaran una vez espierte el micro.
 * @note Como es de arch para el framework se pueden usar funciones como micro y millis
 * aunque seran utilizadas usec_now y msec_now para ser mas generico.
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
volatile unsigned long wdt_restart = 0;

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


    wdt_restart = msec_now();
    
    /* Clear WDRF in MCUSR */
    MCUSR &= ~(1<<WDRF);
    
    /* start wdt */
    wdt_enable(WDT_TIME);
    /* Enable the WDT interrupt (no reset). */
	WDTCSR |= (1 << WDIE);
    WDTCSR |= ~(1 << WDE);

    //Enable global interrupts
    sei();

    wdt_duration = 0;
    
    /** Aqui es donde se deja que corra en activo todo un WDT para
     *  medir cuanto dura 
     * */
    while (1) {
        if (wdt_duration == 1) {
            break;
        }
    } // wait for interrupt

    /* se optiene el tiempo de duracion del WDT */
    /* ========= en el caso del micro que estoy uilizando 8 MHz se pierden 
    sobre los 2 milisegundos (proximo a 1.8) en despertar al micro, se observa perfectamente
    en el perfil de consumo =================================
    esto es un problema gordo para la portabilidad del codigo asi que esta 
    opcion de dormir y despertar con el WDT tendra que reevluada
    !!!!!!!!!!! revisa !!!!!!! */
    wdt_duration = (msec_now() - wdt_restart) + 2;

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

    /* --------------------------------------------------------- */
    /* The program will continue from here after the WDT timeout */
    /* --------------------------------------------------------- */

    /* Disable sleep mode */
    sleep_disable();
    /* Enable ADC */
    ADCSRA |= (1 << ADEN);

    /** Cuando se va a dormir los temporizadores se duermen y se deja de contar 
     * el tiempo por lo que hay que "poner en hora" los registros de msec_now().
     * para ello se ha declado como extern en este archivo la variable timer0_millis
     * que es la que lleva la cuenta de los milisegundos del sistema.
    **/
   	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
    uint8_t oldSREG = SREG;
	cli();

    /** --- @todo aqui se esta asumendo la varible que usa en framework de arduino 
     * para llevar la cuenta de los milisegundos, se debe cambiar por una variable
     * mas portable, o sea, independiente del framework.
    */
   /** el @param timer0_millis es el contador que usa el framework de arduino para
    * contar los milisegundos que han pasado desde que se inicio el programa. El 
    * problema es que este contador se detiene cuando el micro entra en modo sleep
    * así que hay que "poner en hora" el contador de milisegundos. Para ello sumamos
    * lo que demora el WDT menos lo que ya ha sido contado por el WDT. Como el contador
    * de millis se ha detenido el micro despiesta con el mismo valor de millis con el
    * que se durmió, contenido en la variable timer0_millis. Entonces:
    *   T_sleep = wdt_duration - T_active; donde T_sleep es el tiempo que el micro durmió.
    *   T_active = timer0_millis - wdt_start; donde timer0_millis es la variable timer
    *   debe ser actualizada ahora coincide con el momento en el que el micro durmió.
    *   T_sleep = wdt_duration - (timer0_millis - wdt_restart)
    * El momento actual se define como timer0_millis + T_sleep
    *   timer0_millis = timer0_millis + (wdt_duration - (timer0_millis - wdt_restart))
    *   timer0_millis = timer0_millis + (wdt_duration - timer0_millis + wdt_restart)
    *   timer0_millis = timer0_millis + wdt_duration - timer0_millis + wdt_restart
    *   timer0_millis = wdt_restart + wdt_duration
    *   
    */
    timer0_millis = wdt_restart + wdt_duration;

	SREG = oldSREG;
    sei();
}

ISR (WDT_vect){
	// WDIE & WDIF is cleared in hardware upon entering this ISR
	//wdt_disable();

    if(wdt_duration == 0)
        wdt_duration = 1;
    else
        wdt_restart = msec_now();
    
    wdt_enable(WDT_TIME);
    /* Enable the WDT interrupt (note no reset). */
	WDTCSR |= (1 << WDIE);
}