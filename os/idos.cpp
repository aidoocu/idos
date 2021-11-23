
#include "idos.h"

/** \brief  Punto de entrada para idOS */

#ifndef ARDUINO
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
#endif

#ifdef ARDUINO
void setup(){
    /* Inicializamos idOS */
    idos_init();
    /* Arrancamos las tareas que inician al principio */
    task_start();   
}

void loop(){
    /* Esto es básicamente el planificador */    
    /* Mientras exista una tarea en la cola será ivocado su pt */
    while(task_runing() != 0x00){
        ;
    }

    /* Cuando ya no queden tareas en la cola se va a dormir hasta una INT */
    #if SLEEP_MODE
    deep_sleep();
    #endif

    /* Cuando se despierte por el TICK verifico los timers */
    timer_exec();

    /* !!!! Aquí Arduino verifica si hay algún evento serial diponibles !!!! */
}
#endif


uint8_t idos_init(void){

    /* La inicialización del idOS no debe ser interrumpida */
    char cSREG;
    cSREG = SREG;
    cli();

    /* Inicializar el UART como salida estándar de printf */
    uart_init();

    /* Inicializar el SPI */
    #if SPI_ARCH
    spi_init();
    #endif

    /* Si estamos con el framework Arduino ya este inicializa el timer */
    #ifndef ARDUINO
    timer_sys_init();
    #endif /* ARDUINO */

    #if SLEEP_MODE
    sleep_mode_init();
    #endif

    /* Una vez iniciado todo se reactivan las interrupciones y todo el SREG */
    SREG = cSREG;
    sei();
    
    return INIT_SUCCESS;
}