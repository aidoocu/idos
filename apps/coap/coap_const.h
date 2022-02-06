/** 
 * \details Una parte de las definiciones ha sido optenida de la implementación de CoAP para
 * Contki 
 */

#ifndef _COAP_CONST_H_
#define _COAP_CONST_H_



#define COAP_DEFAULT_PORT           5683
#define COAP_DEFAULT_SECURE_PORT    5684

#define COAP_HEADER_LEN     4  /* | version:0x03 type:0x0C tkl:0xF0 | code | mid:0x00FF | mid:0xFF00 | */
#define COAP_MAX_TOKEN_LEN  8 
#define COAP_ETAG_LEN       8

//#define COAP_PHYN_LEN UIP_UDP_PHYH_LEN + COAP_HEADER_LEN

/*  */
#define COAP_VERSION					1
#define COAP_HEADER_VERSION_MASK 		0xC0
#define COAP_HEADER_VERSION_POSITION	6
#define COAP_HEADER_TYPE_MASK 			0x30
#define COAP_HEADER_TYPE_POSITION		4
#define COAP_HEADER_TOKEN_LEN_MASK		0x0F


/* Versión de CoAP */
#define COAP_VERSION 1

/* Tipos de mensaje */
enum {
  COAP_TYPE_CON = 0,/* confirmables */
  COAP_TYPE_NON,	/* non-confirmables */
  COAP_TYPE_ACK,	/* acknowledgements */
  COAP_TYPE_RST		/* reset */
};

/* Métodos de peticiones (request) */
enum {
  COAP_GET = 1,
  COAP_POST,
  COAP_PUT,
  COAP_DELETE
};

/* Códigos de respuesta (response) */
enum {
	EMPTY_MSG_0_00 = 0,					/* EMPTY */
	CREATED_2_01 = 65,            		/* CREATED */
	DELETED_2_02 = 66,            		/* DELETED */
	VALID_2_03 = 67,              		/* NOT_MODIFIED */
	CHANGED_2_04 = 68,            		/* CHANGED */
	CONTENT_2_05 = 69,            		/* OK */
	CONTINUE_2_31 = 95,           		/* CONTINUE */

	BAD_REQUEST_4_00 = 128,       		/* BAD_REQUEST */
	UNAUTHORIZED_4_01 = 129,      		/* UNAUTHORIZED */
	BAD_OPTION_4_02 = 130,        		/* BAD_OPTION */
	FORBIDDEN_4_03 = 131,         		/* FORBIDDEN */
	NOT_FOUND_4_04 = 132,         		/* NOT_FOUND */
	METHOD_NOT_ALLOWED_4_05 = 133,		/* METHOD_NOT_ALLOWED */
	NOT_ACCEPTABLE_4_06 = 134,    		/* NOT_ACCEPTABLE */
	PRECONDITION_FAILED_4_12 = 140, 	/* BAD_REQUEST */
	REQUEST_ENTITY_TOO_LARGE_4_13 = 141,/* REQUEST_ENTITY_TOO_LARGE */
	UNSUPPORTED_MEDIA_TYPE_4_15 = 143, 	/* UNSUPPORTED_MEDIA_TYPE */

	INTERNAL_SERVER_ERROR_5_00 = 160,	/* INTERNAL_SERVER_ERROR */
	NOT_IMPLEMENTED_5_01 = 161,   		/* NOT_IMPLEMENTED */
	BAD_GATEWAY_5_02 = 162,       		/* BAD_GATEWAY */
	SERVICE_UNAVAILABLE_5_03 = 163,		/* SERVICE_UNAVAILABLE */
	GATEWAY_TIMEOUT_5_04 = 164,  		/* GATEWAY_TIMEOUT */
	PROXYING_NOT_SUPPORTED_5_05 = 165,  /* PROXYING_NOT_SUPPORTED */
};

/* CoAP header option numbers */
enum {
	COAP_OPTION_IF_MATCH = 1,     		/* 0-8 B */
	COAP_OPTION_URI_HOST = 3,     		/* 1-255 B */
	COAP_OPTION_ETAG = 4,         		/* 1-8 B */
	COAP_OPTION_IF_NONE_MATCH = 5,		/* 0 B */
	COAP_OPTION_OBSERVE = 6,      		/* 0-3 B */
	COAP_OPTION_URI_PORT = 7,     		/* 0-2 B */
	COAP_OPTION_LOCATION_PATH = 8,		/* 0-255 B */
	COAP_OPTION_URI_PATH = 11,    		/* 0-255 B */
	COAP_OPTION_CONTENT_FORMAT = 12,	/* 0-2 B */
	COAP_OPTION_MAX_AGE = 14,     		/* 0-4 B */
	COAP_OPTION_URI_QUERY = 15,   		/* 0-255 B */
	COAP_OPTION_ACCEPT = 17,      		/* 0-2 B */
	COAP_OPTION_LOCATION_QUERY = 20,	/* 0-255 B */
	COAP_OPTION_BLOCK2 = 23,      		/* 1-3 B RFC7959 */
	COAP_OPTION_BLOCK1 = 27,      		/* 1-3 B RFC7959 */
	COAP_OPTION_SIZE2 = 28,       		/* 0-4 B */
	COAP_OPTION_PROXY_URI = 35,   		/* 1-1034 B */
	COAP_OPTION_PROXY_SCHEME = 39,  	/* 1-255 B */
	COAP_OPTION_SIZE1 = 60,       		/* 0-4 B */
};
	


#define COAP_PAYLOAD_MARK 0xFF

#endif /* _COAP_CONST_H_ */