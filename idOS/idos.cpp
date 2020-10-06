
#include "idos.h"


uint8_t idos_init(void){

    /* La inicialización del idOS no debe ser interrumpida */
    cli();

    timer_init();
    //task_autostart(tasks_auto_start);

    /* Una vez iniciado todo se reactivan las interrupciones */
    sei();
    return INIT_SUCCESS;
}


