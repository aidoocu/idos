
/**
 * \file    uart.c
 * \brief   Define el manejo del UART en idOS como salida estandar.
 * \author  Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 *
 */

#include "uart.h"

void uart_init(void){

    uart_init_arch();
    
}