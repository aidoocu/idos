/** 
 * 
 * Definiciones globales para la plataforma de Arduino
 * 
 * 
 */
#include <Arduino.h>

/* Arduino hasta ahora es definido por el platformio */
#ifdef ARDUINO_AVR_NANO
#include "avr_nano/avr_nano.h"
#endif

#ifdef ARDUINO_AVR_PRO
#include "avr_pro/avr_pro.h"
#endif