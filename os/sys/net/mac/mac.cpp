/** 
 * 
 * 
 */

#include "mac.h"


/** \note   Recibir un paquete se hace en dos operaciones, receive y read
 *          \b receive implica leer en la NIC si hay un paquete disponible, 
 *          caso positivo el driver obtendrá un puntero al mismo.
 *          \b read implica "traer" el paquete de la NIC a un buffer local
 */



/* ¡¡¡ ARCH !!! */

/* Inicializa la interface con la MAC por defecto */
bool mac_init(uint8_t * mac_address) {
    return mac_init_arch (mac_address);
}

/* /¡¡ ARCH !!! */


/** 
 * 
 */
uint16_t block_size(mem_handle_t packet) {

    if (packet != NOBLOCK) {
    if (packet == UIP_RECEIVEBUFFERHANDLE)
        return frame_size_arch ();
    else
        return blocks[packet].size;  
    }

    return 0;

}

/** 
 * 
 */
bool mac_send(void) {

    /* Se envía solo el encabezado eth */
    if (packet_state & UIPETHERNET_SENDPACKET) {
        
        #if NET_DEBUG >= 3
        printf("Writing just header %d bytes", uip_hdrlen);
        #endif
    
        //write_packet_arch(uip_packet, 0, uip_buf, uip_hdrlen);

        packet_state &= ~ UIPETHERNET_SENDPACKET;

        goto send_and_free;
    }

    /* No entiendo porque se hace pero se crea un bloque de memoria del tamanno de uip_len con 
    manipulador uip_packet */
    //uip_packet = mem_block_alloc(uip_len + UIP_SENDBUFFER_OFFSET + UIP_SENDBUFFER_PADDING);

    /* Se envía paquete completo */
    if (uip_len > 0) {

        #if NET_DEBUG >= 3
        printf("Writing full packet: %d bytes", uip_len);
        #endif

        write_packet_arch(uip_buf, uip_len);

        goto send_and_free;
    
    }

    return false;

    send_and_free: {
        

        #if NET_DEBUG >= 3
        printf("%02X:%02X:%02X:%02X:%02X:%02X -> ",
                                hdr_eth->dest.addr[0],
                                hdr_eth->dest.addr[1],
                                hdr_eth->dest.addr[2],
                                hdr_eth->dest.addr[3],
                                hdr_eth->dest.addr[4],
                                hdr_eth->dest.addr[5]);
        #endif

        /* Decirle a la NIC que envíe el paquete */
        if (send_packet_arch()) {

            #if NET_DEBUG >= 3
            printf(" -> send success\r\n");
            #endif

            //mem_block_free(uip_packet);

            /* Limpiar uip_packet */
            uip_packet = NOBLOCK;

            return true;
        }

        #if NET_DEBUG >= 3
        printf("-> send failed\r\n");
        #endif

        /* !!! Si el paquete no se puede enviar debe pasar algo !!! */
        uip_packet = NOBLOCK;

        return false;
    }
}

