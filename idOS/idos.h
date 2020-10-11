
/* definiciones globales y enlaces a las definiciones del kernell */

/* !!! poner #ifndef si se esta compilarndo para arduino.... */
#ifndef _IDOS_H_
#define _IDOS_H_

/** \brief Dentro de arch está la verificación de la arquitectura para la cuál se compilará */
#include "arch/arch.h"


/** Definiciones del kernel  */
#include "kernel/task.h"
#include "kernel/ipc.h"
#include "kernel/sch.h"

/** Definiciones del sistema */
#include "sys/timer.h"

/* ------------------------------------------------------------- */
/** \enum   INIT_STATE 
 *  \brief  Estado de inicialización de idOS
*/
enum {
    INIT_SUCCESS = 1,       /**< idOS inicializó exitosamente */
    INIT_ERR_TIMER,         /**< No fue posible inicializar por causa del timer */    
    INIT_ERR_INT
};

/** \brief  Inicializando idOS
 *  \return Estado de la inicialización
 *  \retval INIT_SUCCESS Inicialización exitosa
 * ........
 */
uint8_t idos_init(void);

#endif //_IDOS_H_