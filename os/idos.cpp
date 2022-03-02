/*
 * Copyright (c) 2022, Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup main
 * @{
 */
/*---------------------------------------------------------------------------*/
/**
 * \file idos.cpp
 *
 * Implementación de las funciones de entrada al sistema
 */
/*---------------------------------------------------------------------------*/

#include "idos.h"

/** \brief  Punto de entrada para idOS */


/** \brief  Inicializando idOS
 *  \return Estado de la inicialización
 *  \retval INIT_SUCCESS Inicialización exitosa
 * ........
 */
uint8_t idos_init(void){

    /* Si la plataforma es native no se inicializa hardware */
    #ifdef NATIVE

    #if NET_STACK
    net_stack_init();
    #endif

    #else /* NATIVE */
    /* La inicialización del idOS no debe ser interrumpida */
    uint8_t cSREG = inte_fall();

    /* Inicializar el UART como salida estándar de printf */
    uart_init();

    /* Inicializar el SPI */
    #ifdef SPI_ARCH
    spi_init();
    #endif

    #if NET_STACK
    net_stack_init();
    #endif

    /* Si estamos con el framework Arduino ya este inicializa el timer */
    #ifndef ARDUINO
    timer_sys_init();
    #endif /* ARDUINO */

    #if SLEEP_MODE
    sleep_mode_init();
    #endif

    /* Una vez iniciado todo se reactivan las interrupciones y todo el SREG */
    inte_raise(cSREG);

    #endif /* NATIVE */

    /* Levantar las apps. Cada app que se adicione debe ser incluida aquí */
    #ifdef BUILD_COAP
    #include "../apps/coap/coap.h"
    coap_start();
    #endif
    
    return INIT_SUCCESS;
}





#ifndef ARDUINO

/** 
 * \brief Punto de entrada de IdOS cuando NO se está usando Arduino
 */
int main(void){

    /* Inicializamos idOS según plataforma */
    idos_init();
    
    /* Arrancamos las tareas que inician al principio */
    task_start();

    /* Esto es básicamente el planificador */    
    while(1){
        /* Mientras exista una tarea en la cola será ivocado su pt */
        while(task_runing() != 0x00){
            ;
        }

        /* Verificar los timers */
        timer_exec();

        /* netstack */
        net_tick();

    }
    return 0;
}
#else

/** 
 * \brief Punto de entrada de IdOS cuando se está usando Arduino
 */
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

    #if NET_STACK
    net_tick();
    #endif

    /* !!!! Aquí Arduino verifica si hay algún evento serial diponibles !!!! */
}
#endif /* ARDUINO */


/*---------------------------------------------------------------------------*/
/**
 * @}
 */