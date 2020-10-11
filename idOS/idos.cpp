
#include "idos.h"

/** \brief  Punto de entrada para idOS */
int main(void){

    /* Inicializamos idOS */
    idos_init();

    /* Arrancamos las tareas que inician al principio */
    task_start();
    
    while(1){
        /* Correr el planificador continuamente */
        schedule();
    }
    return 0;
    }


uint8_t idos_init(void){

    /* La inicialización del idOS no debe ser interrumpida */
    char cSREG;
    cSREG = SREG;
    cli();

    timer_init();

    /* Una vez iniciado todo se reactivan las interrupciones y todo el SREG */
    SREG = cSREG;
    return INIT_SUCCESS;
}