/**
 * \file
 * Header file para las definiciones de mensajes y eventos en idOS.
 * \author
 * Bernardo Yaser León Ávila <bernardoyla@gmail.com>
 * \date 2020/09/24
 *
 */

/**
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
 */

#ifndef _IPC_H_
#define _ICP_H_

#include "../idos.h"

/** 
 * \enum    msg_type
 * \brief   Tipo de mensaje 
 * \anchor  msg_type
 * */
enum {
    MSG_INIT = 0,           /**< Inicializado cuando se llama desde auto_start */
    MSG_TASK,               /**<  Desde una tarea */
    MSG_TIMER,              /**<  Desde un timer */ 
    MSG_UART,               /**< Desde el puerto serie */
    MSG_SPI,                /**< Desde el SPI */
    MSG_I2C,                /**< Desde I2C */
    MSG_EXT_INT             /**< Desde una interrupción externa */
};

/** 
 * \struct  msg_st    
 * \brief   Contenido del mensaje.
 * \note    En un mensaje que no se informe de un evento event = 0
  */
struct msg_st {
    uint8_t msg_src;                    /**< Tipo de mensaje, ver "origen del mensaje" */
    unsigned char event;                /**< Manipulador (handle) del evento */
    void * data;                        /**< Payload del mensaje */
};


/* ------ Macros ------- */
/**
 * \def     MSG_TASK_SEND
 * \brief   Macro para que una tarea envie un mensaje a otra tarea
 * \param   msg_send_to Tarea a la cual se le enviará el mensaje 
 * \param   msg_data    Cuerpo del mensaje
 * \note    Cada vez que se invoque a la macro el mensaje será reecrito
 * \note    Ya que enviar un mensaje a una tarea implica despertarla, si
 *          se envía un mensaje a una tarea que ya esté despierta y esperando
 *          en la cola, el mensaje se pierde.
 */
#define MSG_TASK_SEND(msg_send_to, msg_data)                \
            do {                                            \
                * msg_send_to.msg = {                       \
                    MSG_TASK,                               \
                    0,                                      \
                    msg_data                                \
                };                                          \
                task_set_ready(&msg_send_to);               \
            } while (0)

/**
 * \def     MSG_TMR_SEND
 * \brief   Macro para que un timer envie un mensaje a una otra tarea
 * \param   msg_send_to Tarea a la cual se le enviará el mensaje 
 * \param   msg_data    Cuerpo del mensaje
 * \note    Cada vez que se invoque a la macro el mensaje será reecrito
 * \note    Ya que enviar un mensaje a una tarea implica despertarla, si
 *          se envía un mensaje a una tarea que ya esté despierta y esperando
 *          en la cola, el mensaje se pierde.
 */
#define MSG_TIMER_SEND(msg_send_to, msg_data)               \
            do {                                            \
                * msg_send_to->msg = {                      \
                    MSG_TIMER,                              \
                    0,                                      \
                    msg_data                                \
                };                                          \
                task_set_ready(msg_send_to);                \
            } while (0)


/* ---------------- Funciones callback de los mensajes ---------------- */

/** 
 * \brief   Esta función se llama cuando una tarea le quiere mandar un msg
 * \param   task
 * */
void send_smg(task_st * task_dst, msg_st * msg);

#endif //_ICP_H_