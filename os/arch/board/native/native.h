/** 
 * Punto de entrada de native
 * \author Bernardo
 * \date
 */

#ifndef _ARCH_NATIVE_H_
#define _ARCH_NATIVE_H_

#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif
 
    #include "arch_timer.h"
    #include "arch_sleep.h"
    #include "arch_uart.h"
    #include "arch_net.h"

#ifdef __cplusplus
}
#endif

/* Definiciones globales */
//#define NULL __null

#endif /* _ARCH_NATIVE_H_ */