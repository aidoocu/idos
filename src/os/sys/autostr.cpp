/**
 * \file    autostr.c
 * \brief   Implementación del autostr en idOS.
 * \author  Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 *
 *          El autostr se implementa como una estructura que debe ser declarada antes del pt, un
 *          buen momento es al momento de declarar la tarea.
 *          El autostr se activa con la función autostr_set_sys(autostr_st * autostr, task_st * task)
 *          Si el autostr esta activado se le envia un mensaje a la tarea que seteó el autostr despertandola, 
 *          y luego el autostr se desactiva con la función autostr_disable(autostr_st * autostr) o se resetea 
 *          con la función autostr_reset(autostr_st * autostr).
 */

#include <idos.h>
#include "autostr.h"

/** 
 * \brief   Apunta al primer autostr de la lista
 */
static struct autostr_st * autostr_list = NULL;

/** 
 * \brief   Recorre la lista de autostrs y siempre se queda al final
 */
static struct autostr_st * autostr_index = NULL;


void autostr_set_sys(autostr_st * autostr, task_st * task){
   
    /* Inicializamos el autostr */

    autostr->active = true;    
    autostr->next = NULL;
    autostr->task = task;

    
    /* Se pone el autostr en la lista de autostrs */
    if (autostr_list == NULL){
        /* Si no ningún autostr creado, hay que inicializar la lista */
        autostr_list = autostr;
        autostr_index = autostr;
    } else {
        /* Si hay un autostr  */
    autostr_index->next = autostr;
    autostr_index = autostr;
    }
}

void autostr_reset(autostr_st * autostr){
    /* Reactivar el autostr */
    autostr->active = true;
}

void autostr_exec(void){

    /* Verificar si hay algun autostr esperando y si no hay, salir */
    if (autostr_list){

        /* Ir al principio de la lista de autostrs */
        autostr_index = autostr_list;
        /* Recorrer la lista autostrs buscado el autostr activo */
        do {       
            if (autostr_index->active == true){

                /* Despertar a la tarea */
                task_set_ready(autostr_index->task);
                /* Desactivar el autostr */
                autostr_index->active = false;
                
            }
            /* Cargo el próximo autostr si existe, si no hay, termino el ciclo */
            if (autostr_index->next){
                autostr_index = autostr_index->next;
            } else {
                break;
            }
        } while (1);
    }

    return; 
}

void autostr_disable(autostr_st * autostr) {
    
    /* Desactivamos el autostr*/
    autostr->active = false;
}