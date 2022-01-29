/** 
 * 
 */

#ifndef _COAP_H_
#define _COAP_H_

#include "coap_const.h"


struct coap_hdr_st{
    uint8_t ver_t_tkl;      /* Version, type and Token len */
    uint8_t code;           /* Respose code */
    uint16_t msg_id;        /* Menssage ID */
    uint8_t * payload;      /* Message payload */
    uint16_t payload_len;   /* Message payload length */
};

/** 
 *  \brief Dividir el mensaje en los campos de CoAP
 *  \param hdr Estructura del header de CoAP
 *  \param buf Buffer donde está el mensaje CoAP
 */
#define coap_hdr(hdr, buf) coap_hdr_st * hdr = (struct coap_hdr_st *)&buf

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
 * \brief Optener el option number
 */
#define option_number() ((coap_listener.msg[cp] & 0xF0) >> 4) + option_delta 
/* --------------------------------------------------------------------------------- */


/** 
 * \brief arranca el engine de coap
 */
void coap_start(void);

#endif /* _COAP_H_ */