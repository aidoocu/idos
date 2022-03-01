/** 
 * \file    uart_arch.cpp
 * \brief   Utiliza el UART como salida estándar para printf.
 * \author  Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 * 
 */

#include "uart_esp.h"
//#include <printf.h>

/** \brief  Función callback invocada por printf como salida estándar 
 *  \note   
*/
/* int serial_print(char c, FILE *){
  Serial.write(c);
  return c;
} */

/** \brief  Configuración de UART como salida estándar
 */
void uart_init_arch(void) {
    /* Se inicializa el UART */
    Serial.begin(SERIAL_RATE);
    /* Redirige la salida estandar a la función serial_print */   
    //fdevopen(&serial_print, 0);  
}