/*
 clock-arch.c - Arduino implementation of uIP clock device.
 Copyright (c) 2010 Adam Nielsen <malvineous@shikadi.net>
 All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/////// No es correcto, hay que hacerlo maas global
#ifdef ARDUINO
#include <Arduino.h>
#endif

/////// El timer debe partir de selecionado en el arch y no aquii
#ifdef ARDUINO_AVR_NANO
#include "../../../../arch/timer.h"
#endif

#ifdef ARDUINO_AVR_PRO
#include "../../../../arch/timer.h"
#endif

/* #ifdef ARDUINO_ARCH_ESP8266 
#include "../../../arch/board/esp/timer_esp.h"
#endif  */

#ifdef NATIVE
#include "../../../../arch/board/native/arch_timer.h"
#endif

#include "clock-arch.h"

clock_time_t
clock_time(void)
{
  return (clock_time_t)msec_now();
}
