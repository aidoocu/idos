/** 
 * 
 *  \note Este archivo se separa del arch devido a que es llamado desde
 *  uno o varios archivos .c y pueden entrar en conflito con otros drivers
 *  escritos en cpp.
 */

#ifndef _ARCH_TIMER_H_
#define _ARCH_TIMER_H_

#ifdef ARDUINO
#include "board/arduino/timer.h"
#endif /* ARDUINO */

#ifdef ARDUINO_ARCH_ESP8266
#include "board/esp/timer.h"
#endif

#ifdef NATIVE
#include "board/native/timer.h"
#endif

#endif /* _ARCH_TIMER_H_ */