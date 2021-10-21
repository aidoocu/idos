
#include "idos.h"

/** \brief  Punto de entrada para idOS */
int main(void){

    /* Inicializamos idOS */
    idos_init();

    /* Arrancamos las tareas que inician al principio */
    task_start();

    /* Esto es básicamente el planificador */    
    while(1){
        /* Mientras exista una tarea en la cola será ivocado su pt */
        while(task_runing() != 0x00){
            ;
        }
        /* Cuando ya no queden tareas en la cola se va a dormir hasta una INT */
        //deep_sleep();
    }
    return 0;
    }


uint8_t idos_init(void){

    /* La inicialización del idOS no debe ser interrumpida */
    char cSREG;
    cSREG = SREG;
    cli();

    /* Inicializar el UART como salida estándar de printf */
    uart_init();

    //timer_sys_init_arch();
    //timer_init();

    /* Una vez iniciado todo se reactivan las interrupciones y todo el SREG */
    SREG = cSREG;
    sei();
    
    return INIT_SUCCESS;
}