/** 
 * 
 * 
 */


#include <Arduino.h>
#include "esp.h"


/* -------------------------- Timer ------------------------- */

time_t  msec_now() {
    return millis();
}

time_t  usec_now() {
    return micros();
}

/* ----------------------- Interrupts ----------------------- */

void inte_raise(uint8_t creg){
//    SREG = creg;
    sei();
}

uint8_t inte_fall(void) {
//    uint8_t csreg = SREG;
    cli();
//    return csreg;
    return 1;
}

/* --------------------------- SPI -------------------------- */

void spi_init(void) {

	/* Ponemos el slave pin como salida */
	//pinMode(SS_PIN, OUTPUT);
	/* Deseleccionar el chip */
	//digitalWrite(SS_PIN, HIGH);

	/* Inicializamos SPI usando el framework Arduino*/
	//SPI.begin();

}

void spi_begin_transaction(uint32_t clock, uint8_t bitOrder, uint8_t dataMode){
  	//SPI.beginTransaction(SPISettings(clock, bitOrder, dataMode));
}

void spi_end_transaction(void){
	//SPI.endTransaction();
}

uint8_t spi_transfer(uint8_t data){
	//return SPI.transfer(data);
	return 0;
}

/* -------------------------- uart -------------------------- */

void uart_init_arch(void) {
    /* Se inicializa el UART */
    Serial.begin(SERIAL_RATE); 
}

/* -------------------------- sleep ------------------------- */

void sleep_init_arch(void){

}

void sleep_arch(void){

}

