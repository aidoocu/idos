/** 
 * 
 */

#ifndef _COAP_RESORCE_H_
#define _COAP_RESORCE_H_

/* Tamaño máximo que puede tener un recurso */
#define COAP_RESORCE_MAX_LEN 14

struct coap_resource_st {
    const char * uri;
    coap_resource_st * parent;
    coap_resource_st * next;
    /* call backs por cada método */
};

/** 
 * \brief Cear un recurso CoAP 
 * \param name Nombre del recurso
 * \param uri Uri-Path del recurso
 * \param parent Padre del recurso si tiene
 */
#define coap_resource_create(name, uri, parent)    \
    static struct coap_resource_st name = {        \
        uri,                                       \
        parent,                                    \
        NULL                                       \
    }                                                  


/** 
 * \brief Poner el recurso en la lista de recursos activos
*/
void coap_resource_activate(coap_resource_st * resource);

#endif /* _COAP_RESORCE_H_ */