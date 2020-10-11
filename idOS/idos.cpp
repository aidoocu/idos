
#include "idos.h"


uint8_t idos_init(void){

    /* La inicialización del idOS no debe ser interrumpida */
    char cSREG;
    cSREG = SREG;
    cli();

    timer_init();
    //task_autostart(tasks_auto_start);

    /* Una vez iniciado todo se reactivan las interrupciones y todo el SREG */
    SREG = cSREG;
    return INIT_SUCCESS;
}