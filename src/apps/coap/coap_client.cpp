


#include "coap.h"
#include <idos.h>


/* Id de secuencia del mensaje. Ver sesion 4 de RFC7252 */
static uint16_t message_id = 0;

/* Token (ver sección 5.3.1, RFC7252) */
#if COAP_TOKEN_LEN == 2
static uint16_t message_token_seq = 0;
#elif COAP_TOKEN_LEN == 1
static uint8_t message_token_seq = 0;
#endif

//static uint8_t message_token[COAP_TOKEN_LEN];

/* Array donde se arma el mensaje CoAP */
static uint8_t coap_send_buf[MAX_UDP_MSG_SIZE];
/* Posición que se está procesando dentro del mensaje */
static uint8_t cp = 0;

/* Tarea de engine CoAP */
static struct task_st * coap_task = NULL;

/* Lista de todos los clientes */
static struct coap_client_st * coap_clients = NULL;


/**
 * @brief 
 * 
 * @param task 
 */
void coap_clients_init(struct task_st * task) {

    coap_task = task;
    message_id = (uint16_t)idos_random();

    #if COAP_TOKEN_LEN == 2
    message_token_seq = (uint16_t)idos_random();
    #elif COAP_TOKEN_LEN == 1
    message_token_seq = (uint8_t)idos_random();
    #endif

}


/**
 * @brief 
 * 
 * @param client 
 */
void coap_start_client(struct coap_client_st * client) {

    /* El puerto debe estar entre 0xc000 (49152) y 0xffff (65535) */
    //uint16_t lport = 0xc000 | (0x0fff & (uint16_t)random());
    uint16_t lport = 53151;

    #if NET_DEBUG >= 2
    printf("Cliente escucha por el puerto: %u\n", lport);
    #endif

    client->listener->task = coap_task;
    udp_listener_begin(client->listener, lport);

    /* Si es el primero, coap_clients lo apuntará */
    if(coap_clients == NULL) {
        coap_clients = client;
    } else {
        /* Si no es el primero hay que buscar el último y que este lo apunte */
        coap_client_st * coap_client_index = coap_clients;
        while (coap_client_index->next != NULL) {
            coap_client_index = coap_client_index->next;
        }
        coap_client_index->next = client;
    }
}

/**
 * @brief 
 * 
 */
struct coap_client_st * coap_client_recv(void){
        
        struct coap_client_st * coap_client_index = coap_clients;

        while (coap_client_index != NULL) {
            if(udp_recv(coap_client_index->listener)){

                #if NET_DEBUG >= 2
                printf("Task del cliente CoAP: %s\n",
                            coap_client_index->task->task_strname);
                #endif

                return coap_client_index;
            }

            /* Nos movemos al próximo cliente */
            coap_client_index = coap_clients->next;
        }
    return NULL;

}



/**
 * @brief 
 * 
 * @param type 
 * @param token_len 
 * @param code 
 * @param link 
 */
struct coap_hdr_st * coap_header(uint8_t type, uint8_t token_len, uint8_t code, struct coap_link_st * link) {

    /* Inicializar el coap_send_buf */
    cp = COAP_HEADER_LEN;

    /* ----------------------------------- header ------------------------------------- */

    /* Puntero temporal para manejar el header de CoAP que será enviado */
    struct coap_hdr_st * coap_send_hdr;
    coap_hdr(coap_send_hdr, coap_send_buf);

    /* Version */
    coap_send_hdr->ver_t_tkl = COAP_VERSION << COAP_HEADER_VERSION_POSITION;
    /* Type */
    coap_send_hdr->ver_t_tkl = ((type << COAP_HEADER_TYPE_POSITION) 
                                    & COAP_HEADER_TYPE_MASK)
                                    | coap_send_hdr->ver_t_tkl;
    /* Token len = 0 (ver sección 5.3.1, RFC7252) */
    coap_send_hdr->ver_t_tkl =  (token_len & COAP_HEADER_TOKEN_LEN_MASK)
                                    | coap_send_hdr->ver_t_tkl;

    /* Response code */
    coap_send_hdr->code = code;

    /* Sequence Message ID (ver sección 4, RFC7252) */
    coap_send_hdr->msg_id = ip_htons(++ message_id);


    /* ------------------------------------ token ------------------------------------- */

    #if COAP_TOKEN_LEN == 2
    coap_send_buf[cp ++] = (uint8_t)(message_token_seq >> 8);
    coap_send_buf[cp ++] = (uint8_t)(message_token_seq);
    #elif COAP_TOKEN_LEN == 1
    coap_send_buf[cp ++] = (uint8_t)(message_token_seq);
    #endif /* COAP_TOKEN_LEN */



    /* ----------------------------------- opciones ----------------------------------- */

    uint8_t delta = 0;
    uint8_t option = 0;

    /* URI path option */
    if(link->uri_path) {
        option = COAP_OPTION_URI_PATH - delta;
        delta = COAP_OPTION_URI_PATH;
        option = ((option << 4) & 0xf0) | link->uri_path_len;

        coap_send_buf[cp ++] = option;

        memcpy(&coap_send_buf[cp ++], link->uri_path, link->uri_path_len);
        cp = cp + (link->uri_path_len - 1);
    }

    /* URI query option */
    if(link->uri_query) {
        option = COAP_OPTION_URI_QUERY - delta;
        delta = COAP_OPTION_URI_QUERY;
        option = ((option << 4) & 0xf0) | link->uri_query_len;

        coap_send_buf[cp ++] = option;

        memcpy(&coap_send_buf[cp ++], link->uri_query, link->uri_query_len);
        cp = cp + (link->uri_query_len - 1);
    }

    return coap_send_hdr;

}

/* -------------------------------- cargar payload -------------------------------- */

void coap_payload(const char * msg, uint8_t len) {

    /* Si hay un payload que enviar */
    if(len){

        /* Principio de payload */
        coap_send_buf[cp ++] = 0xFF;

        for(uint8_t i = 0; i < len; i++) {
            coap_send_buf[cp ++] = msg[i];
        }
    
    }

}

bool coap_put_non(struct coap_link_st * link, const char * msg, uint8_t len) {

    coap_header(COAP_TYPE_NON, 0, COAP_PUT, link);

    coap_payload(msg, len);

    udp_send(link->host, link->port, coap_send_buf, cp);


    #if NET_DEBUG >= 2
    printf("put non send: ");
    for (int i = 0; i < cp; i++){
        printf("%02x ", coap_send_buf[i]);
    }
    printf("\n");
    #endif

    return true;

}

bool coap_send(struct coap_link_st * link, struct coap_client_st * client) {

    /* Opteniendo el token */
    client->token = message_token_seq;

    /* Armando el header y guardando el ID obtenido */
    client->message_id = coap_msg_id(coap_header(COAP_TYPE_CON, COAP_TOKEN_LEN, COAP_GET, link));

    udp_send_from(client->listener, link->host, link->port, coap_send_buf, cp);
    
    message_token_seq ++;

    return true;
}

/**
 * @brief 
 * 
 */
bool coap_get(struct coap_link_st * link, struct coap_client_st * client) {
    
    /* Desde aquí se envía directamente */
    if(!coap_send(link, client))
        return false;

    /* y le enviamos el cliente al engine para que lo registre y espere su 
    respuesta */
    task_ipc(coap_task, client);
    
    return true;

}


/**
 * @brief 
 * 
 */
bool coap_received(struct coap_client_st * client) {


    if (client->task->msg->msg_src & MSG_TASK) {

        /* Limpio la bandera */
        client->task->msg->msg_src &= ~MSG_TASK;

        return true;
    
    }

    return false;
}