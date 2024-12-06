/**
 * \file    auto.h
 * \brief   Header file para las definiciones de automatismos en idOS.
 * \author  Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 *
 */

/**
 * \brief   Definiones de los automatismos del sistema
 * 
 *          Los automatismos son basicamente estructuras que sirven para
 *          autodespertar a las tareas. Segun la estructura de idOS, las
 *          tareas no pueden despertarse a si mismas, por lo que se necesita
 *          un mecanismo para que una tarea se autodespierte. Este mecanismo
 *          lo unico que hace es poner la tarea en una lista que sera leida
 *          por el planificador del sistema y activará la tarea en el momento.
*/

#ifndef _AUTOSTR_H_
#define _AUTOSTR_H_


/** 
 * \brief   Estructura del auto
 */
struct autostr_st {
    bool active;                            /** < Indica si este autostr está activo o no */
    struct autostr_st * next;               /** < Próximo autostr en la lista */
    struct task_st * task;                  /** < Puntero a la tarea que seteó el auto */
};

/* --------------------------------------------------------------------------------- */

/** \def    autostr_set
 *  \brief  Crea y setea un autostr (autostr_name) para que dispare dentro de msec
 *  \param  autostr_name  El nombre del autostr
 *          
*/
#define autostr_set(autostr_name)                      \
                static struct autostr_st autostr_name; \
                autostr_set_sys(&autostr_name, task)   \


/* --------------------------------------------------------------------------------- */

/** 
 *  \brief Desabilitar auto
 *  \details El auto es una variable que persiste (y ¿es muy costoso sacarlo de la lista?)
 *  por lo que esta función lo que hace es poner el mensaje a null y desabilitarlo. 
 */
void autostr_disable(struct autostr_st * autostr);

/* --------------------------------------------------------------------------------- */


/** \brief  Crea y setea el auto 
 *  \param  autostr_name  El nombre del auto
 *  \param  task        La tarea que setea el auto
*/
void autostr_set_sys(struct autostr_st * autostr, struct task_st * task);

/** \brief   Resetear el auto */
void autostr_reset(struct autostr_st * autostr);

/** \brief      Función que verifica si hay un autostr activo y está listo para correr. 
 * 
 *  \note       Si ya hay un autostr listo se le pasa un mensaje a la tarea que lo invocó.
 */ 
void autostr_exec(void);

#endif  //_AUTOSTR_H_