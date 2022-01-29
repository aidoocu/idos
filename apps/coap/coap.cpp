/** 
 * 
 * 
 */

#include "../../os/idos.h"
#include "coap.h"

/* Definimos la tarea de CoAP */
TASK(coap_task, "CoAP engine");

/* Iniciar coap engine */
void coap_start(void){ 
    task_set_ready(&coap_task); 
}

/* --------------------------------------------------------------------------------- */

/** 
 * 
 */
bool coap_hdr_check(udp_listener_st * listener, coap_hdr_st * coap_hdr) {

    #if NET_DEBUG >= 2
    printf("checking coap integrity -> ");
    #endif

    //coap_hdr_st * coap_hdr = (struct coap_hdr_st *)listener.msg;

    /* verificar que efectivamente es un paquete CoAP con versión válida */
    if(coap_version(coap_hdr) != COAP_VERSION){
        
        #if NET_DEBUG >= 2
        printf("Version error\n\r");
        #endif

        return false;
    }

    /* Verificar si el tamaño del token es válido */
    if(coap_token_len(coap_hdr) > COAP_MAX_TOKEN_LEN){
        
        #if NET_DEBUG >= 2
        printf("error token len\n\r");
        #endif

        return false;
    }

    uint8_t coap_hdr_len = coap_token_len(coap_hdr) + COAP_HEADER_LEN;

    /* El msg no puede ser menor que el header de CoAP + token lenth */
    if((coap_hdr_len) > listener->msg_len){

        #if NET_DEBUG >= 2
        printf("error msg to short\n\r");
        #endif

        return false;
    }

    if(((coap_hdr_len) == listener->msg_len) && (coap_hdr->code != EMPTY_MSG_0_00)) {

        #if NET_DEBUG >= 2
        printf("msg has not options or payload, but code is not an empty msg \n\r");
        #endif

        return false;        
    }

    if(((coap_hdr_len) != listener->msg_len) && (coap_hdr->code == EMPTY_MSG_0_00)) {

        #if NET_DEBUG >= 2
        printf("has a empty code, but the msg is bigger than header\n\r");
        #endif

        return false;
    }

    #if NET_DEBUG >= 2
    printf("CoAP ok...\n\r");
    #endif

    return true;
}

static uint8_t coap_send_payload[MAX_UDP_MSG_SIZE];
static uint16_t coap_send_len;

void coap_response(uint8_t type, uint8_t code, udp_listener_st * listener, coap_hdr_st * rcvd_hdr){

    /* Array donde se arma el mensaje CoAP */
    uint8_t coap_send_buf[MAX_UDP_MSG_SIZE];
    /* Anotar la posición dentro coap_send_buf */
    uint8_t cp = COAP_HEADER_LEN;

    /* ------------------------------------------- header ------------------------------------------ */

    coap_hdr(coap_send_hdr, coap_send_buf);

    /* Version */
    coap_send_hdr->ver_t_tkl = COAP_VERSION << COAP_HEADER_VERSION_POSITION;
    /* Type */
    coap_send_hdr->ver_t_tkl = ((type << COAP_HEADER_TYPE_POSITION) 
                                    & COAP_HEADER_TYPE_MASK)
                                    | coap_send_hdr->ver_t_tkl;
    /* Token len */
    coap_send_hdr->ver_t_tkl =  (rcvd_hdr->ver_t_tkl 
                                    & COAP_HEADER_TOKEN_LEN_MASK)
                                    | coap_send_hdr->ver_t_tkl;
    /* Response code */
    coap_send_hdr->code = code;
    /* Sequence */
    coap_send_hdr->msg_id = rcvd_hdr->msg_id;

    /* Si el token no es 0 */
    if(coap_token_len(rcvd_hdr)) {
        /* Lo adicionamos al frame */
        memcpy(&coap_send_buf[COAP_HEADER_LEN], &listener->msg[COAP_HEADER_LEN], coap_token_len(rcvd_hdr));
        /* e incluimos su tamaño */
        cp += coap_token_len(rcvd_hdr);
    }

    /* ------------------------------------------- tokens ------------------------------------------ */


    /* Si es un mensaje vacío o BAD_REQUEST_4_00 aquí hemos terminado */
    if((coap_send_hdr->code == EMPTY_MSG_0_00) || (coap_send_hdr->code == BAD_REQUEST_4_00))
        goto send_coap_message;


    /* ------------------------------------------- options ----------------------------------------- */

    /* Comenzamos a tomar la posición para avanzar en las opciones y en el payload si hubiera */
    cp = COAP_HEADER_LEN + coap_token_len(rcvd_hdr);

    /* ??? Final de options */
    //coap_send_buf[cp] = 0x11;
    //coap_send_buf[cp ++] = 0x00;

    /* ------------------------------------------- payload ----------------------------------------- */

    /* Principio de payload */
    coap_send_buf[cp ++] = 0xFF;

    for(uint8_t i = 0; i < coap_send_len; i++) {
        coap_send_buf[cp ++] = coap_send_payload[i];
    }


send_coap_message:

    udp_response(coap_send_buf, cp);

    #if NET_DEBUG >= 2
    printf("Response: ");
    for (int i = 0; i < cp; i++){
        printf("%02x ", coap_send_buf[i]);
    }
    printf("\n");
    #endif
}

/* --------------------------------------------------------------------------------- */


TASK_PT(coap_task){

    TASK_BEGIN

    /* Definimos el listener de CoAP */
    udp_listener(coap_listener);
    udp_listener_begin(&coap_listener, COAP_DEFAULT_PORT); 

    #if NET_DEBUG >= 2
    printf(":: CoAP listening!!\n\r");
    #endif

    while(1){
        
        TASK_YIELD
        
        /* Si se ha recibido algo */
        if(udp_recv(&coap_listener)){

            /* Anotar la posición dentro coap_send_buf */
            uint8_t cp = COAP_HEADER_LEN;

            /* Llevar registro de la option delta */
            uint8_t option_delta = 0;

            /* obtenemos el ip y el puerto del remoto que envió el mensaje... */
            ip_address_t ip_remote;
            udp_remote_addr(coap_listener, ip_remote);
            uint16_t remote_port = udp_remote_port(coap_listener);

            /* -------------------------- debug ------------------------ */
            // ... y lo imprimimos
            printf(":: CoAP msg from: %d.%d.%d.%d:%d\r\n", 
                ip_remote[0], ip_remote[1], ip_remote[2], ip_remote[3], 
                remote_port);

            //imprimimos el mensaje
            printf("msg len: %d\n", coap_listener.msg_len);
            for (int i = 0; i < coap_listener.msg_len; i++){
            printf("%02x ", coap_listener.msg[i]);
            }
            printf("\n");
            for (int i = 0; i < coap_listener.msg_len; i++){
            printf(" %c ", coap_listener.msg[i]);
            }
            printf("\n");

            /* -------------------------- header ----------------------- */

            /* Dividir el mensaje en los campos de CoAP */
            coap_hdr(coap_rcvd_hdr, coap_listener.msg);

            /* Chequeo básico de forma de mensaje */
            if(!coap_hdr_check(&coap_listener, coap_rcvd_hdr)){

                /* Mensaje malformado, responder con 4_00 */
                coap_response(COAP_TYPE_RST, BAD_REQUEST_4_00, &coap_listener, coap_rcvd_hdr);

                #if NET_DEBUG >= 2
                printf("CoAP error: malformed header\n\r");
                #endif

                /* Terminar */
                goto end_coap_process;

            } 
            
            /* CoAP ping: Si el mesaje está vacío y */
            if(coap_rcvd_hdr->code == EMPTY_MSG_0_00){
                /* ...es confirmable se trata de un CoAP ping */
                if(coap_type(coap_rcvd_hdr) == COAP_TYPE_CON){
                    /* Responder con un mensaje rst vacío (pong) */
                    coap_send_len = 0;
                    coap_response(COAP_TYPE_RST, EMPTY_MSG_0_00, &coap_listener, coap_rcvd_hdr);
                }

                /* Terminar */
                goto end_coap_process;
            }

            /* -------------------------- options ----------------------- */

            /* Primera posición después del header */
            cp += coap_token_len(coap_rcvd_hdr);



            /* Verificamos si hay opciones. Si este byte es 0xFF... */
            if(coap_listener.msg[cp] == COAP_PAYLOAD_MARK){
                /* ... es el marcador de payload, no hay opciones */
                goto coap_payload_proccess;
            }

            /* Chequeamos todas las opciones */
            do {
                /* Verificamos que sea correcto el primer byte de la opción */
                if(((coap_listener.msg[cp] & 0x0F) == 0x0F) 
                    || ((coap_listener.msg[cp] & 0xF0) == 0xF0)){

                    /* Una F en Option Delta u Option Length es un error de formato */
                    coap_response(COAP_TYPE_RST, BAD_REQUEST_4_00, &coap_listener, coap_rcvd_hdr);

                    #if NET_DEBUG >= 2
                    printf("CoAP error: Malformed Options\n\r");
                    #endif

                    /* Nada más que hacer aquí... */
                    goto end_coap_process;
                }

                /* Aislamos la opción. Debemos tener en cuenta que el nuevo option_delta = option_number */
                option_delta = option_number();

                /* ------- Identificamos la opción ------- */

                /* Uri-Path - 11 */
                if(option_delta == COAP_OPTION_URI_PATH){
                    printf("URI: ");
                    for(int i=1; i <= (coap_listener.msg[cp] & 0x0F); i++){
                        printf("%c", coap_listener.msg[cp + i]);
                    }
                    printf("\n");
                }
                /* Accept - 12 */
                else if(option_delta == COAP_OPTION_ACCEPT){
                    printf("Accept: ");
                    for(int i=1; i == (coap_listener.msg[cp] & 0x0F); i++){
                        printf("%d", coap_listener.msg[cp + i]);
                    }
                    printf("\n");
                }
                /* Block2 - 23 */
                else if(option_delta == COAP_OPTION_BLOCK2){
                    printf("Block2: ");
                    for(int i=1; i == (coap_listener.msg[cp] & 0x0F); i++){
                        printf("%d", coap_listener.msg[cp + i]);
                    }
                    printf("\n");
                }
                /* Si no ha macheado ninguna de las opciones, pues o no es correcta o 
                no la tenemos implementada, así que... */
                else {
                    /* respondemos con un bad option */
                    coap_response(COAP_TYPE_RST, BAD_OPTION_4_02, &coap_listener, coap_rcvd_hdr);
                    /* Nada más que hacer aquí... */
                    goto end_coap_process;
                }


                /* ------ \Identificamos la opción ------- */

                /* Avazamos el índice hasta el final de la opción */
                cp = cp + (coap_listener.msg[cp] & 0x0F);

                /* Si se acabó el mensaje, hemos terminado, no hay payload */
                if (cp == coap_listener.msg_len - 1)
                    goto end_hdr_process;

                /* Abanzamos al próximo bloque a procesar */
                cp ++;

                //printf("%02x\n", coap_listener.msg[cp]);

            /* Repetimos esta operación siempre que exita una opción. El final de las opciones será o
            el marcador de payload. */
            } while (coap_listener.msg[cp] != COAP_PAYLOAD_MARK);


            /* -------------------------- payload ----------------------- */

            coap_payload_proccess:

            /* Avanzo a la primera posición del payload y lo guardo */
            coap_rcvd_hdr->payload = &coap_listener.msg[cp ++];
            /* así como su largo */
            coap_rcvd_hdr->payload_len = 0;
            while(cp < coap_listener.msg_len){
                cp++;
                coap_rcvd_hdr->payload_len ++;
            }

            printf("payload\n");

            /* ----------------------- process code ---------------------- */

            /* Hasta aquí hemos procesado todo el mensaje */
            end_hdr_process:
            

            /* GET */
            if(coap_rcvd_hdr->code == COAP_GET){
                
                if(coap_type(coap_rcvd_hdr) == COAP_TYPE_CON){
                    coap_send_payload[0] = 'u';
                    coap_send_payload[1] = 'r';
                    coap_send_payload[2] = 'r';
                    coap_send_payload[3] = 'a';
                    coap_send_len = 4;

                    coap_response(COAP_TYPE_ACK, CONTENT_2_05, &coap_listener, coap_rcvd_hdr);
                }
            }

            end_coap_process:
            printf("CoAP: process\n\r");
        }
    }

    TASK_END
}