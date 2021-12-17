#ifndef _UDP_H_
#define _UDP_H_

#include "../netstack.h"

//>>???
#include <Udp.h>

#define UIP_UDP_MAXDATALEN 1500
#define UIP_UDP_PHYH_LEN UIP_LLH_LEN+UIP_IPUDPH_LEN
#define UIP_UDP_MAXPACKETSIZE UIP_UDP_MAXDATALEN+UIP_UDP_PHYH_LEN

typedef struct {
    uint16_t out_pos;
    uint8_t packet_next;
    uint8_t packet_in;
    uint8_t packet_out;
    bool send;
} uip_udp_userdata_t;

#endif /* _UDP_H_ */