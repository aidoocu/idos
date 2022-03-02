
/* definiciones globales y enlaces a las definiciones del kernell */

/* !!! poner #ifndef si se esta compilarndo para arduino.... */
#ifndef _IDOS_H_
#define _IDOS_H_

//#include <stdint.h>

/** Cargar configuraciones globales */
#include "config.h"

/** \brief  Dentro de arch está la verificación de la arquitectura para la cuál se compilará */
#include "arch/arch.h"

/** Definiciones del kernel  */
#include "kernel/task.h"
#include "kernel/ipc.h"
#include "kernel/sch.h"


/** Definiciones del sistema */
#include "sys/timer.h"
#include "sys/uart.h"
#include "sys/sleep.h"


#ifdef NET_STACK
#include "sys/net/netstack.h"
#endif

/* Apps */
#include "../apps/apps.h"

/* ------------------------------------------------------------- */
/** \enum   INIT_STATE 
 *  \brief  Estado de inicialización de idOS
*/
enum {
    INIT_SUCCESS = 1,       /**< idOS inicializó exitosamente */
    INIT_ERR_TIMER,         /**< No fue posible inicializar por causa del timer */    
    INIT_ERR_INT
};



#endif //_IDOS_H_