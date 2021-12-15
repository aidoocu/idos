/** 
 * 
 * 
 */

#ifndef _SPI_ARCH_H_
#define _SPI_ARCH_H_

#include <Arduino.h>
#include <SPI.h>

/** Pines para SPI según micro
 * 
 * | ENC28J60 | Arduino Uno | Arduino Mega |
 * |----------|-------------|--------------|
 * | SCK      | Pin 13      | Pin 52       |
 * | MISO     | Pin 12      | Pin 50       |
 * | MOSI     | Pin 11      | Pin 51       |
 * | CS       | Pin 10      | Pin 53       |
 */
/* CS es configurable */
#ifdef __AVR_ATmega328P__
#define SS_PIN SS
#endif

void spi_init(void);

#endif /* _SPI_ARCH_H_ */