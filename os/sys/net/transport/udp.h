#ifndef _UDP_H_
#define _UDP_H_

#include "../netstack.h"

//>>???
#include <Udp.h>

#define UIP_UDP_MAXDATALEN 1500
#define UIP_UDP_PHYH_LEN UIP_LLH_LEN+UIP_IPUDPH_LEN
#define UIP_UDP_MAXPACKETSIZE UIP_UDP_MAXDATALEN+UIP_UDP_PHYH_LEN

typedef struct {
    mem_address_t out_pos;
    mem_handle_t packet_next;
    mem_handle_t packet_in;
    mem_handle_t packet_out;
    bool send;
} uip_udp_userdata_t;

#endif /* _UDP_H_ */