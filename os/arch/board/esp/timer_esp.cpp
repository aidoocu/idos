/** 
 *  
 */
#include "timer_esp.h"

time_t  msec_now() {
    return millis();
}

time_t  usec_now() {
    return micros();
}