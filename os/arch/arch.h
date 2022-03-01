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

/** Tipos de plataformas (boards) disponibles */
#define NATIVE 1
#define ARDUINO_NANO 2
//#define ESP_ESP8266

/** Definiciones de Arduino */
#ifdef ARDUINO
#include <Arduino.h>


/* Arduino hasta ahora es definido por el platformio así que BOARD no será definida */
#ifdef ARDUINO_AVR_NANO
#define BOARD ARDUINO_NANO
#include "board/arduino/arduino.h"
#endif

#ifdef ARDUINO_ARCH_ESP8266
#include "board/esp/esp8266.h"
#endif

#endif /* ARDUINO */

#if BOARD == NATIVE
#include "board/native/native.h"
#endif

/* ---------------------------- / board ---------------------------------- */



/* ------------- Compilación condicional según el micro (cpu) ------------ */

#ifdef __AVR_ATmega328P__
#include <avr/io.h>
#include "cpu/avr/atmega328p/sleep_arch.h"

/* Si estamos usando el framework de arduino se tomarán de ahí las siguientes 
definiciones */
#ifndef ARDUINO
#include "cpu/avr/atmega328p/timer_arch.h"
#endif /* ARDUINO */

#endif /* __AVR_ATmega328P__ */


/* ----------------------------- / cpu ----------------------------------- */



/* -------- Compilación condicional según los dispositivos (dev) --------- */

/** A partir de aquí serán incorporados los drivers de los dispositivos que 
 * se le conecten a la board.  
*/

/** \note En caso de que la BOARD sea NATIVE se asume que no tendrá dev  */
#if BOARD != NATIVE

#ifdef __ENC28J60__
#include "dev/net/enc28j60/enc28j60_arch.h"
#endif


#include "dev/sensor/new_ping/NewPing.h"

#endif /* BOARD == NATIVE */

/* ----------------------------- / dev ----------------------------------- */




/** Se incluye el resto de idOS al final */
#include "../idos.h"

//#endif

#endif /* _ARCH_H_ */