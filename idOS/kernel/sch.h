/*---------------------------------------------------------------------------*/
/**
 * \addtogroup sch
 * @{
 */
/*---------------------------------------------------------------------------*/
/**
 * \file
 * Header file para las definiciones del planificador de idOS.
 * \author
 * Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 *
 * \brief
 * La planificación se basa en dos colas, una preferente y otra normal.
 * Las colas se implementan como una lista simplemente enlazada. Cada
 * tarea tiene un puntero a la próxima tarea en la cola o a NULL si es 
 * la última. La cola se mantiene con dos punteros, un al principio y 
 * otro al final de la cola.
 * \note
 * Cada tarea podrá ser parte de una sola cola
 */
/*---------------------------------------------------------------------------*/


#ifndef _SCH_H_
#define _SCH_H_

#include "../idos.h"


/**
 * \brief Estructura de punteros para mantener las pociciones de las tareas dentro 
 *        de las colas.
 * 
 *        Implementación de las colas de procesos. Las colas constan de dos punteros
 *        uno al principio y otro al final. El orden de la cola se establece a partir 
 *        el puntero next (next task) que apunta a la próxima tarea en la cola. Si no
 *        hay próxima, next == NULL. Si first == last == NULL la cola está vacía. 
 */
struct sch_st {
    struct task_st * first;
    struct task_st * last;    
};

/* -------------------- funciones ------------------- */

/** 
 * \brief   Marca como activas (pone en a cola) todas las tareas 
 *          que estén en el arreglo de punteros a task.
 * \param task[] arreglo de punteros a task
 * */
void task_autostart(struct task_st * const task[]);

/**
 * \brief   Marcar como activa la tarea pasada como parámetro y ponerla en 
 *          la cola para ser ejecutada
 * \param task  Un puntero a la tarea a activar 
 * */
void task_set_ready(struct task_st * task);

/**
 * \brief   Le otorga la CPU la primera tarea de la cola. Esta función llama
 *          directamente al protohilo de la tarea y espera su retorno, luego
 *          planifica la próxima tarea. 
 * \param   task  Un puntero a la tarea a correr
 * \return  Estado de la cola.
 * \retval  0x00 Si no hay ninguna tarea lista en la cola
 *          0x10 Si la tarea retornó correctamente y no hay una próxima tarea 
 *          0x11 Si la tarea retornó correctamente y hay una próxima tarea que ejecutar 
 * \note    Una vez que la tarea corre, sale de la cola, pero conserva el puntero
 *          a next. 
 */
uint8_t task_runing(void);

/** 
 * \brief Planificador gloval
 */
void schedule();

#endif //_SCH_H_

/*---------------------------------------------------------------------------*/
/**
 * @}
 */