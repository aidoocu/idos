/**
 * 
 * 
 * */
#include "sch.h"
#include "../idos.h"

/**
 * \brief Implementación de la cola de proridad normal. 
 * \note  La cola se inicializa vacía 
 */
static struct sch_st sch_normal{
  NULL,
  NULL
};


/**
 * \brief Implementación de la cola de proridad prioritaria. 
 * \note  La cola se inicializa vacía 
 */
static struct sch_st sch_preem{
  NULL,
  NULL
};

/**
 *  \brief Mensaje que se le pasa a una tarea cuando inicia por auto_start 
 */
//static struct msg_st msg_init  

/* ------------------ Implementación de funciones del sch ------------- */

void task_autostart(struct task_st * const task[]){

  uint8_t i;
  
  for(i = 0; task[i] != NULL; i++) {
    /* Se le envía el mensaje de inicialización a la tarea
    */
    * task[i]->msg = { MSG_INIT, 0, NULL };
    task_set_ready(task[i]);
  }
  return;  
}


void task_set_ready(struct task_st * task){

  /*  Si de hecho la tarea ya está en cola o de hecho ya está corriendo (se llama así 
      misma)no hay nada que hacer 
  */
  if (task->status == READY){
    return;
  }

  /*  Si task opta por ingresar en la cola, pues no aseguramos que 
      no tenga a nadie detrás
  */
  task->next = NULL;

  /*  Si la tarea tiene preferencia, hará la cola preferente.
  */
  if (task->priority == PREEM){
    /* Si no hay nadie (no hay último) en la cola preferente,
      pues task será la próxima tarea es ser ejecutada. */
    if(sch_preem.first == NULL){
      sch_preem.first = task;
      sch_preem.last = task;
    } else {
      /* La última tarea apunta a task como nueva última */
      sch_preem.last->next = task;
      /* task pasa a ser oficialmente la última */
      sch_preem.last = task;
    }
    task->status = READY;
    return;
  }

  /* Si no es preferente, hace la cola normal. Si no hay último, 
    pues task será la próxima en ser ejecutada de no haber preem*/
  if(sch_normal.first == NULL){
    sch_normal.first = task;
    sch_normal.last = task;
    } else {
      /* La última tarea apunta a task como nueva última */
      sch_normal.last->next = task;
      /* task pasa a ser oficialmente la última */
      sch_normal.last = task;
    }
    task->status = READY;
    return;
}

uint8_t task_runing(void){
  
  /* Si hay algo en la cola preferente se corre */
  if(sch_preem.first != NULL){

    /* \TODO!! falta el mecanismo para pasar el evento y sus datos (poner en la misma estructura del proceso) */
    sch_preem.first->pt(sch_preem.first);

    /* Si hay una próxima tarea preferente se planifica */
    if(sch_preem.first->next != NULL){
      sch_preem.first = sch_preem.first->next;
      return 0x11;
    }
    sch_preem.first = NULL;
    /* Si no hubo una nex preem, se verifica si hay una normal */
    if(sch_normal.first != NULL){
      return 0x11;
    }

    /* Si no hay next, se informa */
    sch_normal.first = NULL;
    sch_normal.last = NULL;
    return 0x10;
  }
  if(sch_normal.first != NULL){
    sch_normal.first->pt(sch_normal.first);

    /* Si hay una próxima tarea normal se planifica */
    if(sch_normal.first->next != NULL){
      sch_normal.first = sch_normal.first->next;
      return 0x11;
    } else {
      sch_normal.first = NULL;
      sch_normal.last = NULL;
      return 0x10;
    }
  }

  /* No hay tarea alguna en la cola lista para correr */
  return 0x00;
}

void schedule(){
  /* Mientras exista una tarea en la cola */
  while(task_runing() != 0x00){

  }
  //deep_sleep();
}