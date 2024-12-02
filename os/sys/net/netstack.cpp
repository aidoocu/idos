
#include "../../idos.h"
#include "netstack.h"


/* Variables */
static bool nic_initialized;

#ifdef UIP_STACK
#ifdef UIP_PERIODIC_TIMER
static unsigned long periodic_timer;
#endif /* UIP_PERIODIC_TIMER */
#endif /* UIP_STACK */

/** 
 * 
 */
void net_stack_init(void) {

    /* ------------------------------ ALL STACK ------------------------------ */

    /* -------------------------- IP Configuration --------------------------- */

    /* Si está configurado como static se inicializa según configuración */
    #ifdef NET_STATIC
    
    /* Variables de 8 bits */
    uint8_t host[] = {IP_ADDRESS};
    uint8_t gateway[] = {IP_GATEWAY};
    uint8_t subnet_mask[] = {IP_MASK};

    ip_config(host, gateway, subnet_mask);

    /* Si no, se llama al DHCP */
    #else

    //DHCP aqui!!!!!!!!
    uint8_t host[4];
    uint8_t gateway[4];
    uint8_t subnet_mask[4];

    #endif /* NET_STATIC */


    /* ------------------------------ MAC Init ------------------------------- */

    #ifdef MAC_ADDRESS
    uint8_t mac_address[] = {MAC_ADDRESS};
    #else 
    static uint8_t mac_address[6];
    #endif /* MAC_ADDRESS */

    /** \todo yo creo que no deberia ser asi, creo la MAC no deberia pasarse por parametro
     * arreglar esto
     */
    /* Inicializar la interface de red con la dirección mac si hubiera */
    nic_initialized = mac_init(mac_address);

    if (!nic_initialized) {

        #if NET_DEBUG >= 1
        printf("No NIC found or ready :(\r\n");
        #endif
        
        return;

    #if NET_DEBUG >= 1
    printf("NIC init success :)\r\n");
    #endif
    
    }


    /* ------------------------------ STACK UIP ------------------------------ */

    #ifdef UIP_STACK
    
    #ifdef UIP_PERIODIC_TIMER
    periodic_timer = msec_now() + UIP_PERIODIC_TIMER;
    #endif


    /* Pasarle a uip la mac inicializada */
    uip_seteth_addr(mac_address);

    uip_init();
    uip_arp_init();

    #endif /* UIP_STACK */

    #if NET_DEBUG >= 2

    /* Sustituimos la dirección mac de configuración por la que efectivamente
    se ha configurado */
    mac_get_address(mac_address);

    printf("mac address: %x:%x:%x:%x:%x:%x \r\n", 
                        mac_address[0], 
                        mac_address[1], 
                        mac_address[2], 
                        mac_address[3], 
                        mac_address[4], 
                        mac_address[5]);

    /* Se sustituyen las direcciones de configuración por las
    que efectivamente se han configurado */
    ip_get_address(host, ADDR_HOST);
    ip_get_address(gateway, ADDR_GATEWAY);
    ip_get_address(subnet_mask, MASK_SUBNET);

    /* Se imprime la dirección IP configurada */
    printf("\n ...\n");
    printf ("IP address: %d.%d.%d.%d \r\n", 
                        host[0],
                        host[1],
                        host[2],
                        host[3]);
    printf ("Gateway address: %d.%d.%d.%d \r\n", 
                        gateway[0],
                        gateway[1],
                        gateway[2],
                        gateway[3]);
    printf ("Subnet_mask address: %d.%d.%d.%d \r\n", 
                    subnet_mask[0],
                    subnet_mask[1],
                    subnet_mask[2],
                    subnet_mask[3]);

    #endif /* NET_DEBUG */

}



/** 
 * 
 */
void net_tick(void) {

    /* ------------------------------ STACK UIP ------------------------------ */

    #ifdef UIP_STACK

    /** ---------------------------- Enviar datos a la red ---------------------------- */

    /** @todo UIP_PERIODIC_TIMER esta definido en uipethernet-conf.h y evita que se llame 
     * continuamente a las conexiones en busca de tareas periodicas, uip_periodic y uip_udp_periodic
     * pero lo cierto es que las tareas que quieren mandar (como esta ahora) algo primero lo 
     * escriben en la conexion y tienen que esperar por UIP_PERIODIC_TIMER de estar activo, creando
     * problemas de concurrencia por esta espera. Por ahora tan solo esta debilidato al igual que
     * TCP (ver el ToDo de mas abajo). Esto asi no es eficiente ni conveniente, asi que hay 
     * que hacer que se ejecute de forma periodica sino hay tareas intentando enviar (en caso de
     * un retransmision o lo que sea que quede pendiente) y de forma inmediata cuando una tarea
     * lo necesite*/
    #ifdef UIP_PERIODIC_TIMER
    unsigned long now = msec_now();

    /* Si periodic_timer expiró */
    if (((long)(now - periodic_timer) >= 0)) {
        
        /* Reiniciar periodic_timer */
        periodic_timer = now + UIP_PERIODIC_TIMER;
    #endif /* UIP_PERIODIC_TIMER */

        /** @todo Aqui tan solo estoy evitando que se "utilice" TCP en el net_tick
         * pero lo cierto es que se sigue compilando y ocupando espacio en memoria
         * todo lo que tiene que ver con TCP. Hay que hacer algo...
         * UIP_TCP y UIP_UDP deberían ser definidos en el archivo de configuración
         * por ahora UIP_TCP esta definido en tcp.h (sistema de idOS) y UIP_UDP si
         * esta definido en el archivo de configuración de uIP
         */
        #ifdef UIP_TCP
        for (int conn = 0; conn < UIP_CONNS; conn++) {
            
            /** Verificar que alguna tarea quiere enviar algo (poll) o reitentar 
             * un envío fallido (rexmit) */
            uip_periodic(conn);

            /** Si hubieran datos a enviar, uip_process escribe el frame en uip_buf
             * con uip_len de largo. uip_len == 0 implica que no hay nada que enviar */
            if (uip_len > 0) {

                #if NET_DEBUG >= 3
                printf("App tcp resp to: %d.%d.%d.%d:%u len: %u\r\n",
                        hdr_ip_tcp->destipaddr[0] & 0xff,
                        (hdr_ip_tcp->destipaddr[0] >> 8) & 0xff,
                        hdr_ip_tcp->destipaddr[1] & 0xff,
                        (hdr_ip_tcp->destipaddr[1] >> 8) & 0xff,
                        ip_htons(hdr_ip_tcp->destport), 
                        uip_len);
                #endif

                /* uip_busca en la tabla ARP la dirección MAC a partir de IP 
                y pone el encabezado LLH (Eth) completo */
                uip_arp_out();
                /* Enviar frame */
                mac_send((uint8_t * )uip_buf, uip_len);
            
            }
        }
        #endif  /* UIP_TCP */

        /* UDP conection */
        #if UIP_UDP
        for (int udp_conn = 0; udp_conn < UIP_UDP_CONNS; udp_conn++) {
            uip_udp_periodic(udp_conn);
            // If the above function invocation resulted in data that
            // uip_len is set to a value > 0.
            if (uip_len > 0) {

                /* Limpiar el remoto para que pueda aceptar una nueva pseudoconexión */
                udp_remove_remote(uip_udp_conn);

                #if NET_DEBUG >= 2
                printf("-> udp send msg to: %d.%d.%d.%d:%u len: %u\r\n",
                        hdr_ip_tcp->destipaddr[0] & 0xff,
                        (hdr_ip_tcp->destipaddr[0] >> 8) & 0xff,
                        hdr_ip_tcp->destipaddr[1] & 0xff,
                        (hdr_ip_tcp->destipaddr[1] >> 8) & 0xff,
                        ip_htons(hdr_ip_tcp->destport), 
                        uip_len);
                #endif

                /* uip_busca en la tabla ARP la dirección MAC a partir de IP 
                y pone el encabezado LLH (Eth) completo */
                uip_arp_out();
                /* Enviar frame */
                mac_send((uint8_t * )uip_buf, uip_len);
            }
        }
        #endif  /* UIP_UDP */
    #ifdef UIP_PERIODIC_TIMER
    }
    #endif /* UIP_PERIODIC_TIMER */


    /** ------------------------- Recibir datos desde la red -------------------------- */

    /* Hacemos un poll a la interface... */
    uip_len = mac_poll((uint8_t * )uip_buf);

    /* ...y si ha sido leido un frame y tiene efectivamente datos */
    if (uip_len > 0) {

        /* !!!!! Esta funcion esta contando con que el frame es Eth !!!!!! */
        if (hdr_eth->type == ip_htons(UIP_ETHTYPE_IP)) {

            #if NET_DEBUG >= 3
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
                                    ip_htons(hdr_ip_tcp->destport),
                                    ip_htons(hdr_ip_tcp->srcport));
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

            /** y si hay que responder inmediatamente (por ejemplo a un ping) respondemos */
            if (uip_len > 0) {
                /* uip_busca en la tabla ARP la dirección MAC a partir de IP 
                y pone el encabezado LLH (Eth) completo */
                uip_arp_out();

                /* y se envía */
                mac_send((uint8_t * )uip_buf, uip_len);
            }                

        } else if (hdr_eth->type == ip_htons(UIP_ETHTYPE_ARP)) {

            #if NET_DEBUG >= 3
            printf("ARP frame from: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
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

                /* Enviamos el frame */
                mac_send((uint8_t * )uip_buf, uip_len);
            }
        }
    }

    #endif /* UIP_STACK */




    /* ---------------------------- ESP_NET_STACK ---------------------------- */

    #ifdef ESP_NET_STACK

    if(WiFi.status() != WL_CONNECTED){
        /* Aquí hay que hacer algo, reconectarse..... */

        return;
    }

    /** El stack del ESP que es atendido por el framework de Arduino ya se ocupa
     * de alguna manera de atender y gestionar hasta ip y protocolos intermedios
     * como arp e icmp. Hasta aquí sólo habrá que verificar si hay alguna escucha
     * activa de TCP o UDP y en ese caso verificar si hay algo recibido, o listo 
     * para enviar.
     */

    /* ------------------------------ UDP RX/TX ------------------------------- */
    if(udp_listeners){
        udp_poll();
    }

    if(tcp_listeners){
        /* !!! */
    }



    /* ------------------------------- Receive ------------------------------- */



    #endif /* ESP_NET_STACK */



    /* ------------------------------ STACK LWIP ----------------------------- */
    
    #ifdef LWIP_STACK


    #endif /* LWIP_STACK */
    
}




/* ------------------------------ STACK UIP ------------------------------ */

#ifdef UIP_STACK

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
    return (sum == 0) ? 0xffff : ip_htons(sum);
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
    sum = chksum(sum, (u8_t *)&hdr_ip_tcp->srcipaddr[0], 2 * sizeof(ipaddr_t));

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
            ip_htons(sum));
    #endif
    
    /* 0xffff es el resultado de una comprobación exitosa de checksum */
    return (sum == 0) ? 0xffff : ip_htons(sum);
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


#endif /* UIP_STACK */