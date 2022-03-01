/** 
 * 
 * 
 */

#include <Arduino.h>
#include <SPI.h>

#include "spi_esp.h"

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
	return 2;
}

