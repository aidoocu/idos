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
#endif

#ifdef ARDUINO_ARCH_AVR
#include "arduino_avr/timer_arch.h"
#include "arduino_avr/int_arch.h"
#endif

#endif //_ARCH_H_