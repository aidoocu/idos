#include "timer_ino.h"

time_t  msec_now() {
    return millis();
}

time_t  usec_now() {
    return micros();
}
