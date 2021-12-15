

#include "udp.h"

#if UIP_UDP
#define UIP_ARPHDRSIZE 42
#define UDPBUF ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])

/* void udp_send(uip_udp_userdata_t *data) {
    
    uip_arp_out();

    //ARP busca el header eth para determinar el IP, si no el paquete es reeplazado por arp-request
    if (uip_len == UIP_ARPHDRSIZE) {
        uip_packet = NOBLOCK;
        packet_state &= ~UIPETHERNET_SENDPACKET;

        #ifdef DEBUGGER
        printf("UDP, uip_poll results in ARP-packet");
        #endif
    } else {
        data->send = false;
        data->packet_out = NOBLOCK;
        packet_state |= UIPETHERNET_SENDPACKET;
        
        #ifdef UIPETHERNET_DEBUG_UDP
        printf("UDP, uip_packet to send: %d", uip_packet);
        #endif
    }
    mac_send();
} */


void uipudp_appcall(void) {
    if (uip_udp_userdata_t * data = (uip_udp_userdata_t *)(uip_udp_conn->appstate)) {
        if (uip_newdata()) {
            if (data->packet_next == NOBLOCK) {
                uip_udp_conn->rport = UDPBUF->srcport;
                uip_ipaddr_copy(uip_udp_conn->ripaddr, UDPBUF->srcipaddr);
                data->packet_next = mem_block_alloc(ntohs(UDPBUF->udplen)-UIP_UDPH_LEN);
                /* Si no se puede reservar memoria el paquete es descartado pues UDP no garantiza entrega */
                if (data->packet_next != NOBLOCK) {
                    //discard Linklevel and IP and udp-header and any trailing bytes:
                        //copy_packet_udp(data->packet_next, 0, in_packet, UIP_UDP_PHYH_LEN, mem_block_size(data->packet_next));

                        #ifdef UIPETHERNET_DEBUG_UDP
                        printf("UPD, packet received \r\n");
                        #endif
                }
            }
        }
        if (uip_poll() && data->send) {

            /* set uip_slen (uip private) by calling uip_udp_send */
            #ifdef UIPETHERNET_DEBUG_UDP
            printf("UDP, packet to send: %d \r\n", data->packet_out);
            #endif

            uip_packet = data->packet_out;
            uip_hdrlen = UIP_UDP_PHYH_LEN;
            uip_udp_send(data->out_pos - (UIP_UDP_PHYH_LEN));
        }
    }
}

#endif /* UIP_UDP */