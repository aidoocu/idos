/** 
 * 
 * 
 */
#include "spi_ino.h"

void spi_init(void) {

	/* Ponemos el slave pin como salida */
	pinMode(SS_PIN, OUTPUT);
	/* Deseleccionar el chip */
	digitalWrite(SS_PIN, HIGH);

	/* Inicializamos SPI usando el framework Arduino*/
	SPI.begin();

}


void spi_begin_transaction(SPISettings settings){
  	SPI.beginTransaction(settings);
}


void spi_end_transaction(void){
	SPI.endTransaction();
}



uint8_t spi_transfer(uint8_t data){
	return SPI.transfer(data);
}

