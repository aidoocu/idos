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