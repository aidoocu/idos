/** 
 * 
 * 
 */

#ifdef ARDUINO_AVR_NANO

#include <Arduino.h>
#include "avr_nano.h"

/* ----------------------- Interrupts ----------------------- */

void inte_raise(uint8_t creg){
    SREG = creg;
    sei();
}


uint8_t inte_fall(void) {
    uint8_t csreg = SREG;
    cli();
    return csreg;
}


/* --------------------------- SPI -------------------------- */

#include <SPI.h>

void spi_init(void) {

	/* Ponemos el slave pin como salida */
	pinMode(SS_PIN, OUTPUT);
	/* Deseleccionar el chip */
	digitalWrite(SS_PIN, HIGH);

	/* Inicializamos SPI usando el framework Arduino*/
	SPI.begin();

}


void spi_begin_transaction(uint32_t clock, uint8_t bitOrder, uint8_t dataMode){
  	SPI.beginTransaction(SPISettings(clock, bitOrder, dataMode));
}


void spi_end_transaction(void){
	SPI.endTransaction();
}


uint8_t spi_transfer(uint8_t data){
	return SPI.transfer(data);
}



/* -------------------------- uart -------------------------- */


/** \brief  Función callback invocada por printf como salida 
 *  estándar    
*/
int serial_print(char c, FILE *){
  Serial.write(c);
  return c;
}

/** \brief  Configuración de UART como salida estándar
 */
void uart_init_arch(void) {
    /* Se inicializa el UART */
    Serial.begin(SERIAL_RATE);
    /* Redirige la salida estandar a la función serial_print */   
    fdevopen(&serial_print, 0);  
}

#endif /* ARDUINO_AVR_NANO */