

#include "netstack.h"


/* Variables */
static uint8_t mac_address[] = { MAC_ADDRESS };
static bool nic_initialized;

#ifdef UIP_PERIODIC_TIMER
static unsigned long periodic_timer;
#endif

/** 
 * 
 */
void net_stack_init(void) {
    
    #ifdef UIP_PERIODIC_TIMER
    periodic_timer = msec_now() + UIP_PERIODIC_TIMER;
    #endif

    /* Inicializar la interface */
    nic_initialized = mac_init(mac_address);

    if (!nic_initialized) {

        #if NET_DEBUG >= 1
        printf("No NIC found or ready :(\r\n");
        #endif
        
        return;
    
    }

    #if NET_DEBUG >= 1
    printf("NIC init success :)\r\n");
    
    #endif

    /* Pasarle a uip la mac inicializada */
    uip_seteth_addr(mac_address);

    #if NET_DEBUG >= 3
    printf("mac address: %X:%X:%X:%X:%X:%X \r\n", 
                        uip_ethaddr.addr[0], 
                        uip_ethaddr.addr[1], 
                        uip_ethaddr.addr[2], 
                        uip_ethaddr.addr[3], 
                        uip_ethaddr.addr[4], 
                        uip_ethaddr.addr[5]);
    #endif

    uip_init();
    uip_arp_init();

    /** 
     * Se configura la capa de red a partir de la implementación de uIP de Adam Dunkels. 
     *  Esta utiliza variables de 16 bits para representarlas direcciones IP así que hay 
     *  que llevarlas de 8 bits a 16 e inicializar. 
     */

    /* variables de 16 bits para direcciones IP de uIP */
    uip_ipaddr_t uipaddr;

    /* Si está configurado como static se inicializa según configuración */
    #ifdef NET_STATIC
    /* Variables de 8 bits */
    uint8_t host[] = {IP_ADDRESS};
    uint8_t gateway[] = {IP_GATEWAY};
    uint8_t subnet[] = {IP_SUBNET};

    /* Si no, se llama al DHCP */
    #else

    //DHCP aqui!!!!!!!!

    #endif /* NET_STATIC */

    uip_ip_addr(uipaddr, host);
    uip_sethostaddr(uipaddr);

    #if NET_DEBUG >= 3
    printf ("IP address: %d.%d.%d.%d \r\n", 
                        host[0],
                        host[1],
                        host[2],
                        host[3]);
    #endif

    uip_ip_addr(uipaddr, gateway);
    uip_setdraddr(uipaddr);

    #if NET_DEBUG >= 3    
    printf ("Gateway address: %d.%d.%d.%d \r\n", 
                        gateway[0],
                        gateway[1],
                        gateway[2],
                        gateway[3]);
    #endif

    uip_ip_addr(uipaddr, subnet);
    uip_setnetmask(uipaddr);
    
    #if NET_DEBUG >= 3
    printf ("Subnet address: %d.%d.%d.%d \r\n", 
                    subnet[0],
                    subnet[1],
                    subnet[2],
                    subnet[3]);
    #endif

}

/** 
 * 
 */
void net_tick(void) {

    /** -------------------------------- Recibir datos desde la red --------------------------------  */

    /* Hacemos un poll a la interface... */
    uip_len = mac_poll((uint8_t * )uip_buf, UIP_BUFSIZE);

    /* ...y si ha sido leido un frame y tiene efectivamente datos*/
    if (uip_len > 0) {
    

        bool send_success = false;

        /* !!!!! Esta funcion esta contando con que el frame es Eth !!!!!! */
        if (hdr_eth->type == UIP_HTONS(UIP_ETHTYPE_IP)) {

            /* ¿El frame de entrada pasa a ser oficialmente uIP frame? */
            //uip_frame = in_frame;

            #if NET_DEBUG >= 2
            printf("IP frame from NIC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                                    hdr_eth->src.addr[0],
                                    hdr_eth->src.addr[1],
                                    hdr_eth->src.addr[2],
                                    hdr_eth->src.addr[3],
                                    hdr_eth->src.addr[4],
                                    hdr_eth->src.addr[5]);
            printf("IP frame from host: %d.%d.%d.%d to port: %u from port: %u\r\n",
                                    hdr_ip_tcp->srcipaddr[0] & 0xff,
                                    (hdr_ip_tcp->srcipaddr[0] >> 8) & 0xff,
                                    hdr_ip_tcp->srcipaddr[1] & 0xff,
                                    (hdr_ip_tcp->srcipaddr[1] >> 8) & 0xff,
                                    uip_htons(hdr_ip_tcp->destport),
                                    uip_htons(hdr_ip_tcp->srcport));
            #endif
            
            /* Refrescar o inicializar la tabla ARP */
            uip_arp_ipin();

            /* uip procesa el frame */
            uip_input();

            #if NET_DEBUG >= 3
            /* Las estadisticas deben ser desactivadas en produccion */
            #if UIP_CONF_STATISTICS == 1
            printf("Stats-> recv: %d, send: %d drop: %d\r\n",
            uip_stat.tcp.recv,
            uip_stat.tcp.sent,
            uip_stat.tcp.drop);
            #endif
            #endif

            /* y si hay que responder, respondemos */
            if (uip_len > 0) {
                /* uip_busca en la tabla ARP la dirección MAC a partir de IP 
                y pone el encabezado LLH (Eth) completo */
                uip_arp_out();

                /* y se envía */
                send_success = mac_send((uint8_t * )uip_buf, uip_len);
            }                

        } else if (hdr_eth->type == UIP_HTONS(UIP_ETHTYPE_ARP)) {


            #if NET_DEBUG >= 2
            printf("ARP frame from: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                                    hdr_eth->src.addr[0],
                                    hdr_eth->src.addr[1],
                                    hdr_eth->src.addr[2],
                                    hdr_eth->src.addr[3],
                                    hdr_eth->src.addr[4],
                                    hdr_eth->src.addr[5]);
            #endif

            /* Situar nueva info ARP en la tabla */
            uip_arp_arpin();
            if (uip_len > 0) {

                #if NET_DEBUG >= 3
                printf("ARP response\r\n");
                #endif   

                /* Enviamos el frame y si fue exitoso... */
                send_success = mac_send((uint8_t * )uip_buf, uip_len);
            }
        }

        if (send_success) {
            
            #if NET_DEBUG >= 3
            printf("Freeing frame\r\n");
            #endif      

            /* Liberamos el buffer en la NIC */
            free_frame_arch();

        } else {
            /* !!!!!! Si el frame no se pudo enviar tiene que pasar algo!!!!! */                        
            ;
        }
    }

    /** -------------------------------- Enviar datos a la red --------------------------------  */

    #ifdef UIP_PERIODIC_TIMER
    unsigned long now = msec_now();

    /* Si periodic_timer expiró */
    if ((long)( now - periodic_timer ) >= 0) {
        
        /* Reiniciar periodic_timer */
        periodic_timer = now + UIP_PERIODIC_TIMER;
    #endif /* UIP_PERIODIC_TIMER */

        for (int conn = 0; conn < UIP_CONNS; conn++) {
            
            /** Verificar que alguna tarea quiere enviar algo (poll) o reitentar 
             * un envío fallido (rexmit) */
            uip_periodic(conn);

            /** Si hubieran datos a enviar, uip_process escribe el frame en uip_buf
             * con uip_len de largo. uip_len == 0 implica que no hay nada que enviar */
            if (uip_len > 0) {

                #if NET_DEBUG >= 3
                printf("App resp to: %d.%d.%d.%d:%u len: %u\r\n",
                        hdr_ip_tcp->destipaddr[0] & 0xff,
                        (hdr_ip_tcp->destipaddr[0] >> 8) & 0xff,
                        hdr_ip_tcp->destipaddr[1] & 0xff,
                        (hdr_ip_tcp->destipaddr[1] >> 8) & 0xff,
                        uip_htons(hdr_ip_tcp->destport), 
                        uip_len);
                #endif

                /* uip_busca en la tabla ARP la dirección MAC a partir de IP 
                y pone el encabezado LLH (Eth) completo */
                uip_arp_out();
                /* Enviar frame */
                mac_send((uint8_t * )uip_buf, uip_len);
                /* Aquí no será necesario limpiar el buffer de TX */

                if (uip_conn->appstate != NULL) {
                    tcp_listener_st * listener = (tcp_listener_st *) uip_conn->appstate;
                    if ((listener->state & LISTENER_CONECTING) && (uip_conn->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED) {
                        listener->state = LISTENER_LISTENING | LISTENER_CONNECTED;
                        printf(" >> connected\r\n");
                    }
                }
            }
        }
/*     #if UIP_UDP
        for (int i = 0; i < UIP_UDP_CONNS; i++) {
            uip_udp_periodic(i);
            // If the above function invocation resulted in data that
            // should be sent out on the Enc28J60Network, the global variable
            // uip_len is set to a value > 0.
            if (uip_len > 0) {
                EthernetUDP::_send(&uip_udp_conns[i]);
            }
        }
        #endif  *//* UIP_UDP */
    #ifdef UIP_PERIODIC_TIMER
    }
    #endif
}

/* ------------------------------ Funciones de checksum ------------------------------ */

/** 
 *  \brief Checksum simple
 *  \details Hace la suma de 16 bits del buffer data hasta la posición len.
 *  \param sum suma anterior que se venía haciendo o punto de partida. Puede ser 0.
 *  \param data puntero al buffer donde están los datos
 *  \param len hasta dónde se hace la suma chequeo
 */
uint16_t chksum(uint16_t sum, const uint8_t * data, uint16_t len) {

    uint16_t t;
    const uint8_t * dataptr;
    const uint8_t * last_byte;

    /* Ponemos un puntero al princio y otro al final del buffer */
    dataptr = data;
    last_byte = data + len - 1;
    /* Si al menos quedan dos bytes */
    while(dataptr < last_byte) {  
        t = (dataptr[0] << 8) + dataptr[1];
        sum += t;
        if(sum < t) {
        sum++;            /* carry */
        }
        dataptr += 2;
    }
    /* Si queda solo un byte se rellena con 00 al final (a la derecha) */
    if(dataptr == last_byte) {
        t = (dataptr[0] << 8) + 0;
        sum += t;
        if(sum < t) {
        sum ++;            /* carry */
        }
    }

    /* Return sum in host byte order. */
    return sum;
}

/** 
 *  \brief Suma chequeo de IP (ipchksum)
 *  \details suma arimética de 16 bits con complemento a 1 del encabezado IP
 *  \note El complemento a 1 lo hace uIP cuando esta función retorna a uip_process
 *  \return chksum
 */
uint16_t uip_ipchksum(void) {
    uint16_t sum;

    sum = chksum(0, &uip_buf[UIP_LLH_LEN], UIP_IPH_LEN);
    return (sum == 0) ? 0xffff : uip_htons(sum);
}

/** 
 *  \brief Suma chequeo de la capa de transporte (tcpchksum / udpchksum)
 *  \details El checksum de TCP u UDP se hace a partir de la suma arimética de 16 bits con complemento 
 *  a 1 del pseudoheader + la suma aritmética complemento 1 del header TCP más la totalidad del payload. 
 *  Si uno de estos campos no existe o no llega los 16 bits se rellena con ceros (a la derecha).
 *  El pseudoheader se compone de la dirección IP de origen + la dirección IP de destino + protocolo + 
 *  la logitud del segmento TCP (header TCP + datos).
 *  \param protocol Protocolo de transporte al que se le ve a hacer la suma chequeo
 *  \note El complemento a 1 lo hace uIP cuando esta función retorna a uip_process
 *  \return chksum
 */
uint16_t upper_layer_chksum(uint8_t protocol) {

    /* Logitud del segmento TCP (header TCP + datos) */
    uint16_t upper_layer_segment_len;

    /* Donde vamos a ir recogiendo la checksum */
    uint16_t sum;

    /* ------ Checksum del pseudoheader ------ */

    /* Se comienza obteniedo la longitud del segmento TCP (header TCP + datos == uip_len - UIP_LLH_LEN - UIP_IPH_LEN) pero 
    * a partir de lo que esté declarado en el header IP (campo len) - UIP_IPH_LEN */
    #if UIP_CONF_IPV6
    upper_layer_segment_len = (((u16_t)(hdr_ip_tcp->len[0]) << 8) + hdr_ip_tcp->len[1]);
    #else
    upper_layer_segment_len = (((u16_t)(hdr_ip_tcp->len[0]) << 8) + hdr_ip_tcp->len[1]) - UIP_IPH_LEN;
    #endif

    /* Sumamos el protocolo: 0006 para TCP, 17 para UDP, 1 para ICMP */
    sum = upper_layer_segment_len + protocol;

    /* Llamamos a checksum para calcular el pseudoheader a partir de upper_layer_segment_len + protocol */
    sum = chksum(sum, (u8_t *)&hdr_ip_tcp->srcipaddr[0], 2 * sizeof(uip_ipaddr_t));

    /* ------ Checksum del TCP header ------ */

    uint8_t upper_layer_header_len;

    /* Obtenemos el tamaño del header de TCP  */
    if (protocol == UIP_PROTO_UDP)
        upper_layer_header_len = UIP_UDPH_LEN;
    else
        upper_layer_header_len = (hdr_ip_tcp->tcpoffset >> 4) << 2;

    /* Suma chequeo del encabezado TCP + pseudoheader */
    sum = chksum(sum, &uip_buf[UIP_IPH_LEN + UIP_LLH_LEN], upper_layer_header_len); 

    /* ------ Checksum del payload si hay ------ */

    /* Si hay datos en el payload se hace la suma chequeo de los datos */
    if (upper_layer_header_len < upper_layer_segment_len) {

        sum = chksum(sum, 
            &uip_buf[UIP_LLH_LEN + UIP_IPH_LEN + upper_layer_header_len], 
            (upper_layer_segment_len - upper_layer_header_len));

    }

    #if NET_DEBUG >= 3
    printf("chksum uip_frame [%d - %d]: %X\r\n",
            UIP_IPH_LEN + UIP_LLH_LEN + upper_layer_header_len,
            UIP_IPH_LEN + UIP_LLH_LEN + upper_layer_segment_len, 
            uip_htons(sum));
    #endif
    
    /* 0xffff es el resultado de una comprobación exitosa de checksum */
    return (sum == 0) ? 0xffff : uip_htons(sum);
}

/** 
 * \brief Checksum de TCP 
 */
uint16_t uip_tcpchksum(void) {
    return upper_layer_chksum(UIP_PROTO_TCP);
}    

/** 
 * \brief Checksum de UDP 
 */
#if UIP_UDP
uint16_t uip_udpchksum(void) {
    return upper_layer_chksum(UIP_PROTO_UDP);
}
#endif /* UIP_UDP */
