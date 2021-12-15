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



#ifdef ARDUINO
#include <Arduino.h>
#include "arduino/uart_ino.h"
#include "arduino/timer_ino.h"
#include "arduino/spi_ino.h"
#endif

#ifdef __AVR_ATmega328P__
#include <avr/io.h>
#include "cpu/avr/atmega328p/sleep_arch.h"

/* Si estamos usando el framework de arduino se tomarán de ahí las siguientes definiciones */
#ifndef ARDUINO
#include "cpu/avr/atmega328p/timer_arch.h"
#endif /* ARDUINO */

#endif /* __AVR_ATmega328P__ */

/** A partir de aquí serán incorporados los drivers de los dispositivos que se le conecten al MCU.
 *   
*/

//#ifdef __ENC28J60__

#include "dev/mem_pool.h"
#include "dev/net/enc28j60/enc28j60_arch.h"



/** Se incluye el resto de idOS al final */
#include "../idos.h"

//#endif

#endif /* _ARCH_H_ */