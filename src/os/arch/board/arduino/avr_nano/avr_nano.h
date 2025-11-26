/** 
 * 
 * 
 */
/* Solo se compilan estas definiciones para Arduino Nano */
#ifdef ARDUINO_AVR_NANO

#include "../timer.h"

/* ----------------------- Interrupts ----------------------- */

/** 
 * \brief Levantar las interrupciones
 */
void inte_raise(uint8_t creg);

/** 
 * \brief Bajar las interrupciones
 */
uint8_t inte_fall(void);


/* --------------------------- SPI -------------------------- */


/** Pines para SPI según micro
 * 
 * | ENC28J60 | Arduino Uno | Arduino Mega |
 * |----------|-------------|--------------|
 * | SCK      | Pin 13      | Pin 52       |
 * | MISO     | Pin 12      | Pin 50       |
 * | MOSI     | Pin 11      | Pin 51       |
 * | CS       | Pin 10      | Pin 53       |
 */

#define SS_PIN SS

void spi_init(void);

void spi_begin_transaction(uint32_t clock, uint8_t bitOrder, uint8_t dataMode);

void spi_end_transaction(void);

uint8_t spi_transfer(uint8_t data);



/* -------------------------- uart -------------------------- */


#define SERIAL_RATE 9600

void uart_init_arch(void);


/* -------------------------- random ------------------------ */

#define idos_random() random()


#endif /* _ARDUINO_NANO_H_ */