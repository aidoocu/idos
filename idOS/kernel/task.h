/**
 * \file
 * Header file para las definiciones de tareas y protohilos idOS.
 * \author
 * Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 *
 */
/**
 * \note Important note!! Las tareas de idOS son manejadas como protohilos (pt) utilizando el mismo
 * concepto (simplificado) de Contiki/Contiki-ng. Esto implica que el pt es implementado como un 
 * switch/case donde case: 0 es el punto de entrada cada vez que se invoca TASK_YIELD el pt retorna 
 * (return) y cuando es invocado de nuevo conmuta a case __LINE__: (ver definición de las macros 
 * TASK_BEGIN, TASK_YIELD y TASK_END en este mismo archivo). Esto implica importantes restricciones
 * en el uso de estructuras de control de flujo que son tratadas en los ejemplos así como la necesidad
 * de declarar determinadas variables como static si las necesita utilizadar después de un YIELD
  */

#ifndef _TASK_H_
#define _TASK_H_

#include "../idos.h"

/**
 * \brief      Estados del protothread
 */
enum {
    READY = 1,      // Listo para correr, programado en la cola
    RUNNING,        // Corriendo en la CPU
    YIELDED,        // Inantivo por iniciativa propia (cede la PCU)
    INACTIVE        // Esperando a ser invocado. Corre desde el principio
};

/**
 * \brief       Prioridades
 *              Describe tres modos, Inmediato, Preferente y Normal
 *              Inmediato:  Es invocado por una interrupción por lo que debe utilizarse
 *                          con todas las consideraciones de la misma. 
 *              Prefente:   Una vez declarado READY correrá tan pronto como el pt actual en 
 *                          la CPU termine, a menos que exista uno más pt de la misma prioridad
 *                          esperando. En ese caso serán planificados en orde de llegada
 *              Normal:     Una vez READY piden la cola normalmente
 */
enum {
    IMMT = 0,               // Inmediatamente
    PREEM,                  // Preferente  
    NORMAL                  // Normal
};

/* Indentificando tipos */
typedef unsigned char   event_t ;
typedef unsigned int    data_event_t;

/* ---------------------- Estructura de un task ---------------------- */

/**
 * \brief   La tarea (process en Contiki) es el administrador de recursos
 *          incluye el nombre (name) y la descrición (task_strname) de la 
 *          tarea. ........
 * */
struct task_st{
    struct task_st * next;              /**< Puntero a próxima tarea en cola. Si no está en la cola se apunta a __null */
    const char * task_strname;          /**< Descripción de la tarea */
    const unsigned char priority;       /**< Prioridad de la tarea. Ver Prioridades */
    unsigned char status;               /**< Estado de la tarea. Ver Estados del protohilo */
    unsigned int pc;                    /**< Pseudo contador de programa. Guarda __LINE__ cuando la tarea cede (YIELD) el CPU o es 0 cuando comienza desde el principio */
    struct msg_st * msg;                /**< Mensaje que se envía a esta misma tarea */
    void (* pt)(struct task_st *);      /**< Puntero al callback del protohilo */
};

/* ------------- Macros de definicion de tareas y protohilos ---------------- */

/**
 * \def     
 * \brief   Expande la definición de la función que ejecuta task como protohilo
 * \param   task_name Recive el nombre de la estructura que contiene a la tarea 
 * \param   event decriptor del evento
 * \param   data_event datos del evento
 * \note    La función declarada se apunta desde la misma estructura.
 *          Si no existe un evento, o el evento no porta datos, se pasará __null
* */
#define TASK_PT(task_name)                              \
            void task_pt_##task_name(task_st *task)

/** \def    Expande la definición de la estructura en la que la tarea recibirá 
 *          los mensajes y eventos
 */
#define TASK_MSG(task_name) \
            static struct msg_st msg_from_##task_name = {0, 0, __null}

/**
 * \brief   Primero define el prototipo de la función cuerpo del pt con la macro 
 *          TASK_PT y define la tarea completa en una estructura tipo task.
 *          La tarea (task) utiliza como handle task_mane que es el monbre de la 
 *          estructura.
 * \param   task_name Nombre de la tarea. Refere a toda la tarea y el protohilo.
 * \param   task_strname Nombre humano o descripción de la tarea.
 * \note    La estructura incluye un puntero al protohilo.
* */
#define TASK(task_name, task_strmane)                   \
            TASK_PT(task_name);                         \
            TASK_MSG(task_name);                        \
            static struct task_st task_name = {         \
                __null,                                 \
                task_strmane,                           \
                NORMAL,                                 \
                INACTIVE,                               \
                0,                                      \
                & msg_from_##task_name,                 \
                task_pt_##task_name                     \
            }
    
/** \brief  Le dice al sistema cuales tareas de usuario deben ser arracados al 
 *          inicio.
 *  \param  __VA_ARGS__ Lista de direcciones a tareas. Ej (&task1, &task2) 
 *  \note   Esta macro debe ser llamada después de haber de haber definido todas
 *          las tareas de usuario.
  */
#define TASKS_AUTO_START(...)                                               \
        struct task_st * const tasks_auto_start[] = {__VA_ARGS__, __null};  \
        void task_start(void){ task_autostart(tasks_auto_start); }          \

/** 
 * \brief   El task es un switch/case, por lo que no se recomienda utilizar esta 
 *          forma de control de flujo dentro del task.
 *          TASK_BEGIN comienza poniendo el estado del task a RUNING e inicializar
 *          el switch y el primer case: 0
 */
#define TASK_BEGIN                                      \
        task->status = RUNNING;                         \
        switch(task->pc) {                              \
            case 0:

/** 
 * \brief   El proceso cede voluntariamente la CPU. Primero cambia el status a 
 *          YIELDED y guarda la linea de códico como marcador único. El número
 *          que hay en __LINE__ no es importante, solo debe ser único para case
 *          partiendo de la idea que la macro se expande en una sola linea.
 */
#define TASK_YIELD                              \
                task->status = YIELDED;         \
                task->pc = __LINE__;            \
                return;                         \
            case __LINE__:                      \
                task->status = RUNNING;         

/** 
 * \brief   Al final del task, se debe cerrar la llave del swithc. Como el proceso 
 *          habrá terminado por completo, el task, que nunca abandona memoria, queda 
 *          inactivo (INANTIVE) y listo para ser invocado desde el pricipio
 */
#define TASK_END                                \
        }                                       \
        task->status = INACTIVE;                \
        task->pc = 0;                           \
        return;

#endif //_TASK_H_