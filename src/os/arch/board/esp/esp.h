/** 
 * 
 * 
 */

#ifndef _ESP_H_
#define _ESP_H_

#include <stdio.h>
#include <string.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif /* ARDUINO */

#ifdef ESP32
#include "esp32.h"
#else
#include "esp8266.h"
#endif /* ESP32 */

#include "net_esp.h"

/* -------------------------- Timer ------------------------- */

/** 
 * \brief Devuelve los milosegundos del sistema 
 * */
time_t  msec_now();

/** 
 * \brief Devuelve los microsegundos del sistema 
 * */
time_t usec_now();

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

#define SS_PIN SS

void spi_init(void);

void spi_begin_transaction(uint32_t clock, uint8_t bitOrder, uint8_t dataMode);

void spi_end_transaction(void);

uint8_t spi_transfer(uint8_t data);

/* -------------------------- uart -------------------------- */

#define SERIAL_RATE 115200

void uart_init_arch(void);

/* -------------------------- sleep ------------------------- */

void sleep_arch(void);

void sleep_init_arch(void);


#endif /* _ESP_H_ */