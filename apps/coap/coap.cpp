/** 
 * 
 * 
 */

#include "../../os/idos.h"
#include "coap.h"


/** 
 *  \brief Puntero a lista de recursos 
 */
static struct coap_resource_st * coap_resources;

/** 
 *  \brief Recurso que fue solicitado por el extremo
 */
static struct coap_resource_st * resource_demanded;

/** \brief Encabezado del paquete CoAP recibido */
static struct coap_hdr_st * coap_hdr_in;


static struct coap_payload_st coap_payload;



/* --------------------------------------------------------------------------------- */

/** 
 * 
 */
void coap_resource_activate(coap_resource_st * resource){
    
    /* Si la lista está vacía... */
    if(coap_resources == NULL){
        
        /* ...asignamos el primer recurso */
        coap_resources = resource;
        resource->next = NULL;
        
        /* y nada más que hacer */
        return;

    }

    /* Indice... */
    struct coap_resource_st * resource_index = coap_resources;

    /* ...para buscar el final de lista */
    while (resource_index->next != NULL){
        resource_index = resource_index->next;
    }
    /* Una vez encontrado el último lo ponemos en la lista */
    resource_index->next = resource;

}

/* --------------------------------------------------------------------------------- */

/** 
 * 
 */
bool coap_basic_check(udp_listener_st * listener) {

    #if defined(COAP_DEBUG_NOTICE)
    printf("checking coap integrity -> ");
    #endif

    //struct coap_hdr_st * coap_hdr_check;
    //coap_hdr(coap_hdr_check, listener->msg_in);

    /* verificar que efectivamente es un paquete CoAP con versión válida */
    if(coap_version(coap_hdr_in) != COAP_VERSION){
        
        #if defined(COAP_DEBUG_NOTICE)
        printf("Version error\n\r");
        #endif

        return false;
    }

    /* Verificar si el tamaño del token es válido */
    if(coap_token_len(coap_hdr_in) > COAP_MAX_TOKEN_LEN){
        
        #if defined(COAP_DEBUG_NOTICE)
        printf("error token len\n\r");
        #endif

        return false;
    }

    uint8_t coap_hdr_len = coap_token_len(coap_hdr_in) + COAP_HEADER_LEN;

    /* El msg no puede ser menor que el header de CoAP + token lenth */
    if((coap_hdr_len) > listener->msg_len_in){

        #if defined(COAP_DEBUG_NOTICE)
        printf("error msg to short\n\r");
        #endif

        return false;
    }

    if(((coap_hdr_len) == listener->msg_len_in) && (coap_hdr_in->code != EMPTY_MSG_0_00)) {

        #if defined(COAP_DEBUG_NOTICE)
        printf("msg has not options or payload, but code is not an empty msg \n\r");
        #endif

        return false;        
    }

    if(((coap_hdr_len) != listener->msg_len_in) && (coap_hdr_in->code == EMPTY_MSG_0_00)) {

        #if defined(COAP_DEBUG_NOTICE)
        printf("has a empty code, but the msg is bigger than header\n\r");
        #endif

        return false;
    }

    #if defined(COAP_DEBUG_NOTICE)
    printf("CoAP ok...\n\r");
    #endif

    return true;
}


bool coap_response(uint8_t type, uint8_t code, udp_listener_st * listener){

    /* Array donde se arma el mensaje CoAP */
    uint8_t coap_send_buf[MAX_UDP_MSG_SIZE];
    /* Anotar la posición dentro coap_send_buf */
    uint8_t cp = COAP_HEADER_LEN;

    /* ------------------------------------------- header ------------------------------------------ */

    /* Puntero temporal para manejar el header de CoAP que será enviado */
    struct coap_hdr_st * coap_send_hdr;
    coap_hdr(coap_send_hdr, coap_send_buf);

    /* Version */
    coap_send_hdr->ver_t_tkl = COAP_VERSION << COAP_HEADER_VERSION_POSITION;
    /* Type */
    coap_send_hdr->ver_t_tkl = ((type << COAP_HEADER_TYPE_POSITION) 
                                    & COAP_HEADER_TYPE_MASK)
                                    | coap_send_hdr->ver_t_tkl;
    /* Token len */
    coap_send_hdr->ver_t_tkl =  (coap_hdr_in->ver_t_tkl 
                                    & COAP_HEADER_TOKEN_LEN_MASK)
                                    | coap_send_hdr->ver_t_tkl;
    /* Response code */
    coap_send_hdr->code = code;
    /* Sequence */
    coap_send_hdr->msg_id = coap_hdr_in->msg_id;

    /* ------------------------------------------- tokens ------------------------------------------ */

    /* Si el token no es 0 */
    if(coap_token_len(coap_hdr_in)) {
        /* Lo adicionamos al frame */
        memcpy(&coap_send_buf[COAP_HEADER_LEN], &listener->msg_in[COAP_HEADER_LEN], coap_token_len(coap_hdr_in));
        /* e incluimos su tamaño */
        cp += coap_token_len(coap_hdr_in);
    }

    /* -------------------------------------------- empty ------------------------------------------ */


    /* Si es un mensaje vacío o BAD_REQUEST_4_00 aquí hemos terminado */
    if((coap_send_hdr->code == EMPTY_MSG_0_00) || (coap_send_hdr->code == BAD_REQUEST_4_00))
        goto respose_coap_message;


    /* ------------------------------------------- options ----------------------------------------- */

    /* Comenzamos a tomar la posición para avanzar en las opciones y en el payload si hubiera */




    /* ------------------------------------------- payload ----------------------------------------- */

    /* Si hay un payload que enviar */
    if(coap_payload.send_len){

        /* Principio de payload */
        coap_send_buf[cp ++] = 0xFF;

        for(uint8_t i = 0; i < coap_payload.send_len; i++) {
            coap_send_buf[cp ++] = coap_payload.send[i];
        }
    
    }


respose_coap_message:

    if (!udp_response_to(listener, coap_send_buf, cp)) {

        // Si no se puede enviar algo tiene que pasar?
        
        #if defined(COAP_DEBUG_NOTICE)
        printf("CoAP error: Fail sending packet\n");
        #endif

        return false;

    }

    #if defined(COAP_DEBUG_NOTICE)
    printf("Response length: %d and packet: ", cp);
    for (int i = 0; i < cp; i++){
        printf("%02x ", coap_send_buf[i]);
    }
    printf("\n");
    #endif

    return true;

}


/* ------------------------------------------------------------------------------------ */
/* ----------------------------------- CoAP Engine ------------------------------------ */
/* ------------------------------------------------------------------------------------ */

/* Definimos la tarea de CoAP */
TASK(coap_task, "CoAP engine");

/* Iniciar coap engine */
void coap_start(void) { 
    task_set_ready(&coap_task); 
}

/* ------------------------------------ .well know ------------------------------------ */

/** 
 * GET
 * \brief Aquí se verifican todos los recursos disponibles y se le envía como respuesta
 *  según RFC5988, Constrained RESTful Environments (CoRE) Link Format.
 */
static uint8_t discover_get(coap_payload_st * payload) {

    #if defined(COAP_DEBUG_NOTICE)
    printf(".well_know\n\r");
    #endif

    /*  */

    return 0;
}

/* --------------------------------- CoAP Engine task ---------------------------------- */

TASK_PT(coap_task){

    TASK_BEGIN

    /* Definimos el listener de CoAP */
    udp_listener(coap_listener, COAP_SERVER_PORT);

    coap_clients_init(task);

    /* Creamos el recurso .well-known de descubrimiento */
    coap_resource_create(discover, ".well-known", NULL);
    coap_resource_activate(&discover);

    discover.get = * discover_get;

    #if defined(COAP_DEBUG_NOTICE)
    printf(":: CoAP listening!!\n\r");    
    #endif

    while(1){
        
        TASK_YIELD
        
        /* En cada ciclo inicializamos todas las variables */
        coap_payload.send_len = 0;
        coap_payload.rcvd_len = 0;
        resource_demanded = NULL;

        /* ---------------------------------------------------------- */
        /* -------------------- timer_back_koff --------------------- */

        if(0/* timer_expired(&timer_back_off) */) {

            /* Buscar un cliente con un back_off activo */

            /* Si hay uno volver a solicitar ?? y reiniciar el timer */

            /* Si el cliente ha consumido su back_off notificar timer_out
            a su tarea */

            /* Si no hay ninguno ?? */
        }

        /* ------------------- /timer_back_koff --------------------- */
        /* ---------------------------------------------------------- */


        /* ---------------------------------------------------------- */
        /* --------------------- client_request --------------------- */

        if(task->msg->msg_src & MSG_TASK) {

            /* Limpiar la bandera */
            task->msg->msg_src &= ~MSG_TASK;

            /* Separando el cliente para procesarlo */
            struct coap_client_st * client = (coap_client_st *)(task->msg->data);
            
            #if defined(COAP_DEBUG_NOTICE)
            printf("El cliente: %s envio un msg CoAP\n", client->task->task_strname);
            printf("ID: %u - Token: %x\n", client->message_id, client->token);
            #endif

            /* Arrancar el timer de back_off */
            client->back_off = COAP_CLIENT_BACK_OFF;
            //timer_set(timer_back_off, COAP_CLIENT_RESEND_MS);
        
        }

        /* -------------------- /client_request --------------------- */
        /* ---------------------------------------------------------- */


        /* ---------------------------------------------------------- */
        /* ----------------------- udp_request ---------------------- */

        if(task->msg->msg_src & MSG_NETWORK) {

            /* Limpiar la bandera */
            task->msg->msg_src &= ~MSG_NETWORK;

            /* obtenemos el ip y el puerto del remoto que envió el mensaje... */
            #if defined(COAP_DEBUG_NOTICE)
            ip_address_t remote_ip;
            uint16_t remote_port;
            #endif

            /* Anotar la posición dentro coap_send_buf */
            uint8_t cp = COAP_HEADER_LEN;

            /* Llevar registro de la option delta */
            uint8_t option_delta = 0;


            #if defined(COAP_DEBUG_NOTICE)
            printf("Network: ");
            #endif
            

            /* ------------------------- Client ------------------------- */

            /* Hay que determinar se fue un cliente y ya de paso cual */
            struct coap_client_st * client = coap_client_recv();
            if(client) {

                
                coap_hdr(coap_hdr_in, client->listener->msg_in);

                /* #if defined(COAP_DEBUG_NOTICE)
                printf("client request \n");
                udp_remote_addr(client->listener, remote_ip);
                remote_port = udp_remote_port(client->listener);
                #endif */

                /* Chequeo básico de forma de mensaje */
                if(!coap_basic_check(client->listener)){

                    #if defined(COAP_DEBUG_NOTICE) || defined(COAP_DEBUG_ERROR)
                    printf("CoAP error: malformed header\n\r");
                    #endif

                    /* Terminar */
                    goto end_coap_process;

                } 
                
                /** \note Un cliente solo espera por una respuesta, así que nada 
                 * que no sea una respuesta es una petición válida. Esto se verifica 
                 * por el token. Por restricciones en los recursos esta implementación
                 * solo contempla que cada cliente puede hacer una única solicitud
                 * y hasta que esta no sea satisfecha o rechazada no podrá hacer otra.
                 */



                /* Primero se chequea que efectivamente viene un token */
                if(!coap_token_len(coap_hdr_in) || coap_token_len(coap_hdr_in) != COAP_TOKEN_LEN){

                    #if defined(COAP_DEBUG_NOTICE)
                    printf("CoAP notice: no token or not with rigth size\n\r");
                    #endif

                    /* Terminar */
                    goto end_coap_process;

                }

                /* Luego se chequea si el token recibido coincide con el guardardo 
                por el cliente. Note que si el cliente no espera una respuesta (token 
                = 0) igual será diferente que el recibido */
                #if COAP_TOKEN_LEN == 2

                if ((client->listener->msg_in[cp++] != (uint8_t)(client->token >> 8)) || 
                        client->listener->msg_in[cp++] != (uint8_t)(client->token)) {

                    #if defined(COAP_DEBUG_NOTICE)
                    printf("CoAP notice: is not a response\n\r");

                    #endif

                    /* Terminar */
                    goto end_coap_process;                    
                }

                #elif COAP_TOKEN_LEN == 1

                if (client->listener->msg_in[cp++] != client->token) {

                    #if defined(COAP_DEBUG_NOTICE)
                    printf("CoAP notice: is not a response\n\r");
                    #endif

                    /* Terminar */
                    goto end_coap_process;                    
                }

                #endif/* COAP_TOKEN_LEN */

                /* Si todo es correcto se trata de una posible respuesta a un mensaje 
                puede pasar cuatro cosas, 
                    - o que sea un piggybacked (rfc7252 - Section 5.2.1) donde la respuesta 
                    viene en el ACK.
                    - Un ACK vacío.
                    - Un RST.
                    - Un CON / NON con un separate response (rfc7252 - Section 5.2.2) 
                En caso que sea un ACK/RST hay que verificar que el ID sea el mismo, en caso 
                contrario se puede verificar que el ID sea simplemente mayor, aunque en este 
                puede haber un error cuando el ID se desvorde, situación que habría que 
                controlar */

                #if defined(COAP_DEBUG_NOTICE)
                printf("<-- Coap mensaje received: ");
                #endif

                /* Las tres primeras posibilidades se virifican en un único if */
                if((coap_type(coap_hdr_in) == COAP_TYPE_ACK || 
                    coap_type(coap_hdr_in) == COAP_TYPE_RST) &&
                    client->message_id == coap_msg_id(coap_hdr_in)){

                    /* En este caso se trata de un respuesta que puede ser success (ACK)
                    o fail (RST) pero en cualquier caso ese ID no se espera más */
                    client->message_id = 0;
                    client->back_off = 0;

                    /* Si es un un RST... */
                    if(coap_type(coap_hdr_in) == COAP_TYPE_RST) {

                        #if defined(COAP_DEBUG_NOTICE)
                        printf("Response - RST\r\n");
                        #endif

                        /* Se limpia el token... */
                        client->token = 0;

                        /* Se buscan posibles opciones? */

                        /* ...se despierta a la tarea con un NULL payload */
                        task_ipc(client->task, NULL);

                        /* y nada más que hacer */
                        goto end_coap_process;

                    }

                    #if defined(COAP_DEBUG_NOTICE)
                    printf("Response - ACK\r\n");
                    #endif

                    if (coap_hdr_code(coap_hdr_in) == CONTENT_2_05) {
                    
                        /* Se limpia el token... */
                        client->token = 0;

                        /* Se buscan posibles opciones? */

                        /* Se separa el payload */
                        if(client->listener->msg_in[cp++] == 0xFF) {
                            task_ipc(client->task, &client->listener->msg_in[cp]);
                        }


                        /* ...y se le notifica a la tarea dueña del cliente que tiene una respuesta */



                    }
                    
                    /* Si es un ACK vacío no se puede limpiar el token pues la respuesta viene en un 
                    mensaje por separado. En este caso no se le notifica a la tarea (no hay respuesta)
                    aún y se termina */
                    goto end_coap_process;

                }
                /* Ocurre el cuarto caso */ 
                else {
                   
                    /* Si es CON el extremo espera una respuesta */
                    if (coap_type(coap_hdr_in) == COAP_TYPE_CON)
                        coap_response(COAP_TYPE_ACK, EMPTY_MSG_0_00, client->listener);                        
                    
                    /** Ya no se espera más por esta respuesta 
                     * \attention Si el extremo no recibe este ACK seguirá enviando la respuesta
                     * hasta su timeout. Esto podría crear un problema
                    */
                    client->token = 0;

                    /* Se le notifica a la tarea */
                    task_ipc(client->task, NULL);

                    /* y terminamos */
                    goto end_coap_process;
                }
                
            }

            /* ------------------------- Server ------------------------- */

            if(udp_recv(&coap_listener)){


                /* -------------------------- debug ------------------------ */

                #if defined(COAP_DEBUG_NOTICE)
                // ... y lo imprimimos
                udp_remote_addr(coap_listener, remote_ip);
                remote_port = udp_remote_port(coap_listener);
                
                printf(":: CoAP server has received a message from: ");
                printf("%d.%d.%d.%d:%d\r\n", 
                    remote_ip[0], remote_ip[1], remote_ip[2], remote_ip[3], 
                    remote_port);

                //imprimimos el mensaje
                printf("msg len: %d\n", coap_listener.msg_len_in);

                for (uint8_t i = 0; i < coap_listener.msg_len_in; i++){
                    printf("%02x ", coap_listener.msg_in[i]);
                }
                printf("\n");

                #endif 

                /* -------------------------- header ----------------------- */

                /* Dividir el mensaje en los campos de CoAP a partir del puntero 
                coap_hdr_in */
                coap_hdr(coap_hdr_in, coap_listener.msg_in);

                /* Chequeo básico de forma de mensaje */
                if(!coap_basic_check(&coap_listener)){

                    /* Mensaje malformado, responder con 4_00 */
                    coap_respond(COAP_TYPE_RST, BAD_REQUEST_4_00);

                    #if defined(COAP_DEBUG_NOTICE)
                    printf("CoAP error: malformed header\n\r");
                    #endif

                    /* Terminar */
                    goto end_coap_process;

                } 
                
                /* CoAP ping: Si el mesaje está vacío y */
                if(coap_hdr_in->code == EMPTY_MSG_0_00){
                    /* ...es confirmable se trata de un CoAP ping */
                    if(coap_type(coap_hdr_in) == COAP_TYPE_CON){
                        /* Responder con un mensaje rst vacío (pong) */
                        coap_payload.send_len = 0;
                        coap_respond(COAP_TYPE_RST, EMPTY_MSG_0_00);
                    
                        /* Terminar */
                        goto end_coap_process;
                    }
                }

                /* Primera posición después del header */
                cp += coap_token_len(coap_hdr_in);

                /* -------------------------- options ----------------------- */

                /* Verificamos si hay opciones. Si este byte es 0xFF... */
                if(coap_listener.msg_in[cp] == COAP_PAYLOAD_MARK){
                    /* ... es el marcador de payload, no hay opciones */
                    goto coap_payload_proccess;
                }

                /* Chequeamos todas las opciones */
                do {
                    /* Verificamos que sea correcto el primer byte de la opción */
                    if(((coap_listener.msg_in[cp] & 0x0F) == 0x0F) 
                        || ((coap_listener.msg_in[cp] & 0xF0) == 0xF0)){

                        /* Una F en Option Delta u Option Length es un error de formato */
                        coap_respond(COAP_TYPE_RST, BAD_REQUEST_4_00);

                        #if defined(COAP_DEBUG_NOTICE)
                        printf("CoAP error: Malformed Options\n\r");
                        #endif

                        /* Nada más que hacer aquí... */
                        goto end_coap_process;
                    }

                    /* Aislamos la opción. Debemos tener en cuenta que el nuevo option_delta = coap_option_number */
                    option_delta = coap_option_number();

                    /* ------- Identificamos la opción ------- */

                    /* Uri-Path - 11 */
                    if(option_delta == COAP_OPTION_URI_PATH){

                        #if defined(COAP_DEBUG_NOTICE)
                        printf("URI: ");
                        for(int i=1; i <= (coap_listener.msg_in[cp] & 0x0F); i++){
                            printf("%c", coap_listener.msg_in[cp + i]);
                        }
                        printf("\n");
                        #endif

                        /* Buscamos si el recurso existe. Debería existir al menos un recurso activo
                        que es .well-known */

                        coap_resource_st * resource_parent = NULL;
                        /* Si hay algo aquí es un padre, y hay que determinar si este recurso es su hijo */
                        if(resource_demanded){
                            resource_parent = resource_demanded;
                        }

                        resource_demanded = coap_resources;

                        /* Si  */
                        while(resource_demanded != NULL) {
                            if((!memcmp(resource_demanded->uri_path, 
                                    &coap_listener.msg_in[cp + 1], 
                                    (coap_listener.msg_in[cp]) & 0x0F))
                                    && (resource_demanded->parent == resource_parent)){

                                break;
                            }
                            
                            resource_demanded = resource_demanded->next;
                        }
                        
                        /* Si el recurso solicitado no existe */
                        if(resource_demanded == NULL){
                            
                            #if defined(COAP_DEBUG_NOTICE)
                            printf("CoAP error: Resource \"");
                            for(int i = 1; i <= (coap_listener.msg_in[cp] & 0x0F); i++ )
                                printf("%c", coap_listener.msg_in[cp + i]);
                            printf("\" not found\n\r");
                            #endif

                            /* Si el método es GET respondemos con not found */
                            if(coap_hdr_in->code == COAP_GET){
                                /* notificamos y... */
                                coap_respond(COAP_TYPE_RST, NOT_FOUND_4_04);
                            } else if((coap_hdr_in->code == COAP_POST) || 
                                        (coap_hdr_in->code == COAP_DELETE)){
                                /* Si no, es otra operación no implementada (post o 
                                delete)... */
                                coap_respond(COAP_TYPE_RST, METHOD_NOT_ALLOWED_4_05); 
                            } else {
                                /* ... o es un put donde el recurso no existe, pero
                                crear un recurso no está permitido por política */
                                coap_respond(COAP_TYPE_RST, FORBIDDEN_4_03);                            
                            }
                            /* terminamos */
                            goto end_coap_process;
                        }

                    }

                    /* Content-Format - 12 */
                    else if (option_delta == COAP_OPTION_CONTENT_FORMAT){
                        
                        #if defined(COAP_DEBUG_NOTICE)
                        printf("Content-Format\n");
                        #endif

                        /* Como el objetivo de idOS son dispositivo muy restrigido, por ahora solo aceptaremos
                        texto plano en tipos MIME. Para ello hay que verificar que esta opción sea len = 0 */
                        if((coap_listener.msg_in[cp] & 0x0F) != 0 && 
                            (coap_listener.msg_in[++ cp] != 0)){
                            
                            #if defined(COAP_DEBUG_NOTICE)
                            printf("CoAP error: Unsupported Content-Format\n\r");
                            #endif

                            coap_respond(COAP_TYPE_RST, UNSUPPORTED_MEDIA_TYPE_4_15);

                            /* Nada más que hacer aquí... */
                            goto end_coap_process;                        
                        }

                    }

                    /* Accept - 17 */
                    else if(option_delta == COAP_OPTION_ACCEPT){
                        
                        #if defined(COAP_DEBUG_NOTICE)
                        printf("Accept:\n");
                        #endif

                        /* Como el objetivo de idOS son dispositivo muy restrigido, por ahora solo enviaremos
                        texto plano en tipos MIME. Para ello hay que verificar que esta opción sea len = 0 */
                        if((coap_listener.msg_in[cp] & 0x0F) != 0 && 
                            (coap_listener.msg_in[++ cp] != 0)){
                            
                            #if defined(COAP_DEBUG_NOTICE)
                            printf("CoAP error: Unsupported media type\n\r");
                            #endif

                            coap_respond(COAP_TYPE_RST, UNSUPPORTED_MEDIA_TYPE_4_15);

                            /* Nada más que hacer aquí... */
                            goto end_coap_process;                        
                        }

                    }
                    /* Block2 - 23 */
                    else if(option_delta == COAP_OPTION_BLOCK2){
                        #if defined(COAP_DEBUG_NOTICE)
                        printf("Block2: ");
                        for(int i=1; i == (coap_listener.msg_in[cp] & 0x0F); i++){
                            printf("%d", coap_listener.msg_in[cp + i]);
                        }
                        printf("\n");
                        #endif
                    }
                    /* Si no ha macheado ninguna de las opciones, pues o no es correcta o 
                    no la tenemos implementada, así que... */
                    else {

                        /* respondemos con un bad option */
                        coap_respond(COAP_TYPE_RST, BAD_OPTION_4_02);

                        #if defined(COAP_DEBUG_NOTICE)
                        printf("CoAP error: Bad option\n\r");
                        #endif

                        /* Nada más que hacer aquí... */
                        goto end_coap_process;
                    }


                    /* ------ \Identificamos la opción ------- */

                    /* Avazamos el índice hasta el final de la opción */
                    cp = cp + (coap_listener.msg_in[cp] & 0x0F);

                    /* Si se acabó el mensaje, hemos terminado, no hay payload */
                    if (cp == coap_listener.msg_len_in - 1)
                        goto end_hdr_process;

                    /* Abanzamos al próximo bloque a procesar */
                    cp ++;

                /* Repetimos esta operación siempre que exita una opción. El final de las opciones será o
                el marcador de payload. */
                } while (coap_listener.msg_in[cp] != COAP_PAYLOAD_MARK);


                /* -------------------------- payload ----------------------- */

                coap_payload_proccess:

                cp ++;
                /* Avanzo a la primera posición del payload y lo apunto */
                coap_payload.rcvd = &coap_listener.msg_in[cp];

                /* así como su largo */
                coap_payload.rcvd_len = 0;
                while(cp < coap_listener.msg_len_in){
                    cp ++;
                    coap_payload.rcvd_len ++;
                }

                #if defined(COAP_DEBUG_NOTICE)
                printf("payload\n");
                #endif

                /* ----------------------- process code ---------------------- */

                /* Hasta aquí hemos procesado todo el mensaje */
                end_hdr_process:
                

                /* Si hay un recurso */
                if(resource_demanded) {
                    
                    #if defined(COAP_DEBUG_NOTICE)
                    printf("resource demanded: %s\n", resource_demanded->uri_path);
                    #endif

                    /** \attention Esta forma de implementar el get/put hace recaer en el callback
                     * la seguridad de la operación. Note que cualquier solicitud de PUT será
                     * enviada al callback, así que es su responsabilidad de asegurar la 
                     * legitimidad de la solicitud.
                     */
                    /** \todo Verificar si es necesario pasar más información a los callbacks
                     * acerca del origen del mensaje y la licitud de la solicitud.
                     */

                    /* GET */
                    if(coap_hdr_in->code == COAP_GET && resource_demanded->get != NULL){

                        #if defined(COAP_DEBUG_NOTICE)    
                        printf("Call GET\n");
                        #endif

                        /** \todo Aquí falta el mecanismo para atender a los recursos que son
                        separados, o sea, este get no tiene la respuesta, así que la enviará
                        en otro mensaje (con el mismo token?) */

                        /* Se llama al callbak GET del recurso */
                        uint8_t get = resource_demanded->get(&coap_payload);

                        /* Si es con conexión, se envía en una respuesta */
                        if(get && coap_type(coap_hdr_in) == COAP_TYPE_CON){
                            coap_respond(COAP_TYPE_ACK, CONTENT_2_05);
                        } else {
                            /* Si no puede ser NON o un RST, get devolver 0 por un acceso 
                            denegado, porque el recurso... y..., 
                            entonces ... ToDo... */  
                            //...
                        }

                        /* Terminanos */
                        goto end_coap_process;
                    }

                    /* PUT */
                    if (coap_hdr_in->code == COAP_PUT && resource_demanded->put != NULL){
                        
                        #if defined(COAP_DEBUG_NOTICE)
                        printf("Call PUT\n");
                        #endif

                        /* Se llama al callback PUT de recurso */
                        uint8_t put = resource_demanded->put(&coap_payload);

                        if(put && coap_type(coap_hdr_in) == COAP_TYPE_CON){
                            
                            /* ToDo !!! que pasa cuado el recurso no se pudo cambiar y hay conexion */
                            
                            coap_respond(COAP_TYPE_ACK, CHANGED_2_04);
                        } else if(!put){
                            
                            /* Si no puede ser NON o un RST, put devolver 0 por un acceso 
                            denegado, porque el recurso...y..., 
                            entonces ... ToDo... */  
                            //...
                            coap_respond(COAP_TYPE_ACK, BAD_REQUEST_4_00);
                        }

                        /* Terminanos */
                        goto end_coap_process;

                    }
                    

                    /* Si llega aquí es que el método no está implementado para este recurso. 
                    No están implementado para idOS como política (por ahora) ni post ni delete */
                    coap_respond(COAP_TYPE_RST, METHOD_NOT_ALLOWED_4_05);

                    #if defined(COAP_DEBUG_NOTICE)
                    printf("CoAP error: Method not allowed\n\r");
                    #endif

                }

                end_coap_process:
                
                #if defined(COAP_DEBUG_NOTICE)
                printf("   < ------------------ >\n\r");
                #endif
                asm("NOP");
            }
        }

        /* ---------------------- /udp_request ---------------------- */
        /* ---------------------------------------------------------- */

    }

    TASK_END
}