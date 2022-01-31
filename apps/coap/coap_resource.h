/** 
 * 
 */

#ifndef _COAP_RESORCE_H_
#define _COAP_RESORCE_H_

#include "coap.h"

/* Tamaño máximo que puede tener un recurso */
#define COAP_RESORCE_MAX_LEN 14

struct coap_resource_st {
    const char * uri_path;          /* Opción Uri-Path */
    coap_resource_st * parent;      /* Recurso padre (si tiene) */
    coap_resource_st * next;        /* Próximo recurso en la lista de 
                                    recursos activos */
    /* call backs por cada método */
    void (* get)(coap_payload_st *);     /* Payload */
    void (* post)(void *);
    void (* put)(void *);
    void (* delt)(void *);
};

/** 
 * \brief Cear un recurso CoAP 
 * \param name Nombre del recurso
 * \param uri Uri-Path del recurso
 * \param parent Padre del recurso si tiene
 */
#define coap_resource_create(name, uri, parent) \
    static struct coap_resource_st name = {     \
        uri,                                    \
        parent,                                 \
        NULL,                                   \
        NULL,                                   \
        NULL,                                   \
        NULL,                                   \
        NULL                                    \
    }                                                  


/** 
 * \brief Poner el recurso en la lista de recursos activos
*/
void coap_resource_activate(coap_resource_st * resource);

//#define coap_send(msg) 


#endif /* _COAP_RESORCE_H_ */