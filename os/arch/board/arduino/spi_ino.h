/** 
 * 
 * 
 */

#ifndef _SPI_ARCH_H_
#define _SPI_ARCH_H_

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

void spi_begin_transaction(uint32_t clock, uint8_t bitOrder, uint8_t dataMode);

void spi_end_transaction(void);

uint8_t spi_transfer(uint8_t data);

#endif /* _SPI_ARCH_H_ */