/**
 * 
 * 
 *  */
#include <idos.h>
#include "arch.h"

/** 
 * \brief Inicializa idOS según la plataforma (board, cpu, devs)
 */
void dev_start(void){

    #ifdef BME_280
    bme280_init();
    #endif

}