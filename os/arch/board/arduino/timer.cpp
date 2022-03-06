

/* Solo se compila si es un Arduino AVR */

#ifdef ARDUINO_ARCH_AVR

#include <Arduino.h>
#include "timer.h"

time_t  msec_now() {
    return millis();
}

time_t  usec_now() {
    return micros();
}

#endif /* ARDUINO_ARCH_AVR */