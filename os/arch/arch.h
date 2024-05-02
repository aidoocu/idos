/** \file   arch.h
 *  \brief  Selecciona la arquitectura para la cual se compilará
 * 
 *          Aquí se cargan todos los archivos head que contienen definiciones y configuraciones
 *          de hardware según la arquitectura definida por el entorno. Si se utiliza el framework
 *          de Arduino este cargará toda la abstracción del hardware posible
 *  
 *  \author Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 */

#ifndef _ARCH_H_
#define _ARCH_H_

/* --------- Compilación condicional según la plataforma (board) --------- */

#ifdef ARDUINO
#include "board/arduino/arduino.h"
#endif /* ARDUINO */

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#include "board/esp/esp.h"
#endif

#ifdef NATIVE
#include "board/native/native.h"
#endif

/* ---------------------------- / board ---------------------------------- */



/* ------------- Compilación condicional según el micro (cpu) ------------ */

#ifdef __AVR_ATmega328P__
#include <avr/io.h>
#include "cpu/avr/atmega328p/sleep_arch.h"
#endif /* __AVR_ATmega328P__ */


/* ----------------------------- / cpu ----------------------------------- */



/* -------- Compilación condicional según los dispositivos (dev) --------- */

/** A partir de aquí serán incorporados los drivers de los dispositivos que 
 * se le conecten a la board.  
*/

/** \note En caso de que la BOARD sea NATIVE se asume que no tendrá dev  */
#ifndef NATIVE

/* Interface Ethernet */
#ifdef ENC28J60
#include "dev/net/enc28j60/enc28j60_arch.h"
#endif /* ENC28J60 */

/* Interface RF24 */
#ifdef NRF24L01
#include "dev/net/nrf2401/nrf24l01_arch.h"
#endif /* NRF24L01 */

/* Sensor de distancia a partir de ultrasonido */
#ifdef ULTRASONIC
#include "dev/sensor/new_ping/NewPing.h"
#endif

/* Sensor de presión barométrica y temperatura */
#ifdef BMP180
#include "dev/sensor/bosh/bmp180/SFE_BMP180.h"
#endif

/* Sensor de humedad y temperatura */
#ifdef BME280
#include "dev/sensor/bosh/bme280/BME280I2C.h"
#include <Wire.h>
#endif

/* Sensor de temperatura */
#ifdef DS18B20
#include "dev/sensor/one_wire/OneWire.h"
#include "dev/sensor/one_wire/18x20/DallasTemperature.h"
#endif


#endif /* NATIVE */

/* ----------------------------- / dev ----------------------------------- */




/** Se incluye el resto de idOS al final */
//#include "../idos.h"

//#endif

#endif /* _ARCH_H_ */