
/** \file       uart_arch.h
 *  \brief      Definiciones de UART como salida estandar para el framework de Arduino.
 *      
 *  \note       La idea es que la función printf utilice el UART directmente como salida
 *              estándar. En este caso no será necesario utilizar Serial.xxx, pero tampoco
 *              está restringido. 
 *  \attention  ¡¿?!. 
 *
 *  \author     Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 */

#ifndef _UART_ARCH_H_
#define _UART_ARCH_H_


#include <Arduino.h>

#define SERIAL_RATE 9600

void uart_init_arch(void);
int serial_print(char c, FILE *);

#endif //_UART_ARCH_H_