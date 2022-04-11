/** 
 * 
 * Definiciones globales para la plataforma de Arduino
 * 
 * 
 */
#include <Arduino.h>

/* Arduino hasta ahora es definido por el platformio */
#ifdef ARDUINO_AVR_NANO
#include "nano/nano.h"
#endif


/* Algunas definiciones que pueden ser globales porque pertenecen 
framwork y no a un hardware en concreto */

/* ----------------------- Random ----------------------- */

#define idos_random() random()


