/** 
 * 
 * 
 */

#include "../../../idos.h"
#include "mac.h"


/** \note   Recibir un paquete se hace en dos operaciones, receive y read
 *          \b receive implica leer en la NIC si hay un paquete disponible, 
 *          caso positivo el driver obtendrá un puntero al mismo.
 *          \b read implica "traer" el paquete de la NIC a un buffer local
 */



/* ¡¡¡ ARCH !!! */

/* Inicializa la interface con la MAC por defecto */
/* bool mac_init(uint8_t * mac_address) {
    return mac_init_arch (mac_address);
} */

/* /¡¡ ARCH !!! */

/** 
 * 
 */
/* bool mac_send(void) {

    //Se verifica contenido en el frame
    if (uip_len > 0) {

        #if NET_DEBUG >= 3
        printf("Writing frame: %d bytes", uip_len);
        #endif

        write_frame_arch(uip_buf, uip_len);

        #if NET_DEBUG >= 3
        printf("%02X:%02X:%02X:%02X:%02X:%02X -> ",
                                hdr_eth->dest.addr[0],
                                hdr_eth->dest.addr[1],
                                hdr_eth->dest.addr[2],
                                hdr_eth->dest.addr[3],
                                hdr_eth->dest.addr[4],
                                hdr_eth->dest.addr[5]);
        #endif

        //Decirle a la NIC que envíe el paquete
        if (send_frame_arch()) {

            #if NET_DEBUG >= 3
            printf(" -> send success\r\n");
            #endif
            
            return true;
        }
    }

    #if NET_DEBUG >= 3
    printf("-> send failed\r\n");
    #endif

    return false;

} */

