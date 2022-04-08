/** 
 * 
 */

#ifndef _COAP_H_
#define _COAP_H_

#include "../../os/idos.h"

/* CoAP debugger */
#define COAP_DEBUG_NOTICE
//#define COAP_DEBUG_WARNING
//#define COAP_DEBUG_ERROR

/** 
 * \brief Estructura para el parceo del header de CoAP
 */
struct coap_hdr_st{
    uint8_t ver_t_tkl;      /* Version, type and Token len */
    uint8_t code;           /* Respose code */
    uint16_t msg_id;        /* Menssage ID */
};

/** 
 *  \brief Dividir el mensaje en los campos de CoAP
 *  \param hdr Estructura del header de CoAP
 *  \param buf Buffer donde está el mensaje CoAP
 */
#define coap_hdr(hdr, buf) hdr = (struct coap_hdr_st *)&buf

struct coap_payload_st {
    uint8_t * rcvd;                     /* Message payload */
    uint16_t rcvd_len;                  /* Message payload length */    
    uint8_t send[MAX_UDP_MSG_SIZE];     /* Message payload */
    uint16_t send_len;                  /* Message payload length */
};

/* --------------------------------------------------------------------------------- */

/** 
 * \brief Optiene la version de coap a partir del header 
 */
#define coap_version(coap_hdr) ((coap_hdr->ver_t_tkl & COAP_HEADER_VERSION_MASK) \
                                    >> COAP_HEADER_VERSION_POSITION)

/** 
 * \brief Optiene el tipo de mensaje
 */
#define coap_type(coap_hdr) ((coap_hdr->ver_t_tkl & COAP_HEADER_TYPE_MASK) \
                                    >> COAP_HEADER_TYPE_POSITION)

/** 
 * \brief Optiene el valor de largo del Token de coap a partir del header
 */
#define coap_token_len(coap_hdr) (coap_hdr->ver_t_tkl & COAP_HEADER_TOKEN_LEN_MASK)

/**
 * \brief Optiene el código de respuesta
 */
#define coap_hdr_code(coap_hdr) (coap_hdr->code)

/** 
 * \brief Optiene el ID del mensaje
 */
#define coap_msg_id(coap_hdr) (uip_htons(coap_hdr->msg_id))

/** 
 * \brief Optener el option number
 */
#define coap_option_number() ((coap_listener.msg_in[cp] & 0xF0) >> 4) + option_delta 


/** 
 * \brief Responder inmeditamente.
 * \details Responder implica que ................
 * mismo token?
 */
#define coap_respond(type, code)      \
    coap_response(type, code, &coap_listener);

/* --------------------------------------------------------------------------------- */


/** 
 * \brief arranca el engine de coap
 */
void coap_start(void);


#include "coap_const.h"
#include "coap_resource.h"
#include "coap_client.h"

#endif /* _COAP_H_ */