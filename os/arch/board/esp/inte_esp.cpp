/** 
 * 
 * 
 */

#include <Arduino.h>
#include "inte_esp.h"

void inte_raise(uint8_t creg){
//    SREG = creg;
    sei();
}


uint8_t inte_fall(void) {
//    uint8_t csreg = SREG;
    cli();
//    return csreg;
    return 1;
}