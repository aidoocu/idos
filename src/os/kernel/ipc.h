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
 * \addtogroup sys
 * @{
 *
 * \defgroup IPC InterProcesss Comunications
 * @{
 * 
 * \brief   El IPC (InterProcesss Comunications) se implementa con pase de 
 *          mensajes. Aquí están las definiciones de los mensajes.
 * 
 *          Como se trata de un sistema completamente cooperativo, se asume
 *          que cuando hay una tarea corriendo es porque el resto ha cedido
 *          voluntariamente la CPU, en cuyo caso, la tarea espera a que alguien
 *          la despierte y la forma prevista es enviarle un mensaje. Así siempre
 *          que cualquier tarea despierte podrá conocer el motivo consultando
 *          task.msg......  
 *          El mensaje puede enviarlo una tarea o cualquier otro evento calificado
 *          como un timer, interrución externa, etc y deberá ser la única forma
 *          de enviarle datos a una tarea. Así, cuando se genera un evento, la
 *          forma de notificarselo a la tarea que espera por él es mediante un
 *          mensaje.
 * 
 * \file
 * Header file para las definiciones de mensajes y eventos en idOS.
 * \author
 * Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 * \date 2020/09/24 
 *
 */


#ifndef _IPC_H_
#define _IPC_H_

#include <idos.h>

/** 
 * \enum    msg_src
 * \brief   Tipo de mensaje. Es una bandera que permite que una tarea reciba
 *          más de un mensaje de distintas fuentes únivas.
 * \anchor  msg_src
 * */
enum {
    MSG_INIT = 0,       /**< Inicializado cuando se llama desde auto_start */
    MSG_TASK = 1,       /**< Desde una tarea */
    MSG_TIMER = 2,      /**< Desde un timer */ 
    MSG_UART = 4,       /**< Desde el puerto serie */
    MSG_SPI = 8,        /**< Desde el SPI */
    MSG_I2C = 16,       /**< Desde I2C */
    MSG_EXT_INT = 32,   /**< Desde una interrupción externa */
    MSG_NETWORK = 64    /**< Desde la red. Este mensaje lo envía TCP o UDP */
};


/** 
 *  \enum  status
 *  \brief Estado del mensaje
 */
enum {
    MSG_NULL = 0,           /**< No hay mensaje disponible */
    MSG_AVAILABLE,          /**< Hay un mensaje que debe ser leido */
    MSG_PENDING             /**< Hay un mensaje pendiente de leer */
};

/** 
 * \struct  msg_st    
 * \brief   Contenido del mensaje.
 * \note    En un mensaje que no se informe de un evento event = 0
 * \todo La fuente no va a se necesaria, hay que quitarla
  */
struct msg_st {
    uint8_t status;         /**< Estado del mensaje */
    uint8_t msg_src;        /**< Tipo de mensaje, ver "origen del mensaje" */
    uint8_t event;          /**< Manipulador (handle) del evento */
    void * data;            /**< Payload del mensaje */
};


/* ------ Macros ------- */
/**
 * \def     task_ipc
 * \brief   Macro para que una tarea envie un mensaje a otra tarea
 * \param   msg_send_to Tarea a la cual se le enviará el mensaje 
 * \param   msg_data    Cuerpo del mensaje
 * \note    Cada vez que se invoque a la macro el mensaje será reecrito
 * \note    Ya que enviar un mensaje a una tarea implica despertarla, si
 *          se envía un mensaje a una tarea que ya esté despierta y esperando
 *          en la cola, el mensaje se pierde.
 */
#define task_ipc(msg_send_to, msg_data)                                         \
            msg_send_to->msg->status = MSG_AVAILABLE;                           \
            msg_send_to->msg->msg_src =  MSG_TASK | msg_send_to->msg->msg_src;  \
            msg_send_to->msg->data = msg_data;                                  \
            task_set_ready(msg_send_to)




/* ---------------- Funciones callback de los mensajes ---------------- */

/** 
 * \brief   Esta función se llama cuando una tarea le quiere mandar un msg
 * \param   task
 * */
void send_smg(struct task_st * task_dst, struct msg_st * msg);

#endif //_ICP_H_

/**
 * @}
 * @}
 */