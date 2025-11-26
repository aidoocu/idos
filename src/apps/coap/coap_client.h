/**
 * @file coap_client.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _COAP_CLIENT_H_
#define _COAP_CLIENT_H_

#include <idos.h>

struct coap_client_st {
    const char * name;                  /* Nombre del cliente */
    struct udp_listener_st * listener;  /* Listener del cliente. Tener en cuenta que este
                                        listener tendrá como task dueña en engine CoAP */
    struct task_st * task;              /* Tarea que crea al cliente */

    #if COAP_TOKEN_LEN == 2             /* Token de la respuesta que espera el cliente */
    uint16_t token;                     /* 0 cuando no hay respuesta que esperar */
    #elif COAP_TOKEN_LEN == 1
    uint8_t token;
    #endif

    uint16_t message_id;                /* Message id */
    uint8_t back_off;                   /* Back off de la respuesta */
    struct coap_client_st * next;       /* Proximo cliente */
};

/** 
 * \brief Crea y activa un cliente de CoAP que no es más que un
 * listener udp.
 * \param name Nomnre del cliente
 */
#define coap_client(name)                           \
    static struct udp_listener_st coap_##name;      \
    static struct coap_client_st name;              \
    name.listener = &coap_##name;                   \
    name.task = task;                               \
    name.token = name.back_off = 0;                 \
    coap_start_client(&name)


void coap_start_client(struct coap_client_st * client);


/** 
 * 
 */
void coap_clients_init(struct task_st * task);

/**
 * @brief Identificar que cliente ha recibido el mensaje
 * 
 * @return Un puntero al cliente que recivio el mensaje o null
 */
struct coap_client_st * coap_client_recv(void);

/**
 * \brief   Estructura para referirse al enlace del recurso remoto
 * \details Sigue el esquema URI de CoAP:
 *  coap://<host>:<port>/<uri_path>/?<uri_query>
 * 
 */
struct coap_link_st {
    uint8_t * host;         /* IP del extremo */
    uint16_t port;          /* puerto del extremo */
    const char * uri_path;  /* recurso dentro del host */
    uint8_t uri_path_len;   /* tamaño del uri path */
    const char * uri_query; /* Petición al recurso */
    uint8_t uri_query_len;  /* tamaño del uri query */    
};

#define coap_link(name, host, port, path, query)    \
    static struct coap_link_st name = {             \
        host,                                       \
        port,                                       \
        path,                                       \
        (sizeof(path) - 1),                         \
        query,                                      \
        (sizeof(query) - 1)                         \
    };



/**
 * @brief 
 * 
 * @param link 
 * @param client 
 * @return true 
 * @return false 
 */
bool coap_get(struct coap_link_st * link, struct coap_client_st * client);


/*  */
bool coap_received(struct coap_client_st * client);






/** 
 *  \brief Envía un paquete CoAP tipo PUT a un remoto sin conexión
 *  \details Es el elemento más simple porque envía un paquete CoAP
 *  sin esperar respuesta de ningún tipo.    
 */
bool coap_put_non(struct coap_link_st * link, const char * msg, uint8_t len);




#endif /* _COAP_CLIENT_H_ */