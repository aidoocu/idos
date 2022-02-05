/** 
 * Este es una modificación del archivo pp de la 
 * biblioteca Arduino-UIP (UIPEthernet) 
 * 
 */


#include "enc28j60_arch.h"

/*  */
static uint16_t next_frame_ptr;
static uint8_t bank;

/** 
 *  \brief Guarda la dirección de memoria del paquete en el ENC y su tamaño
 */
static struct received_frame_t received_frame;

/* Registros para guardar temporalmente el estado de SPI */
//uint8_t spcr_temp;
//uint8_t spsr_temp;

/** \brief  Configurar SPI para el ENC28J60 */
void enc_spi_enable(void) {

    /* Guardo el estado de SPCR */
    //spcr_temp = SPCR;
    //spsr_temp = SPSR;

    /* initializar la interface SPI */
    SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
 
}

/** \brief Recuperar los valores de configuración de SPI */
void enc_spi_disable(void) {

    /* Recupero los registros */
    //SPCR = spcr_temp;
    //SPSR = spsr_temp;
    SPI.endTransaction();

}

/**  \note SPI Instruction Set
 *   \anchor commands
 *   \details From datasheet 4.2 SPI Instruction Set: 
 *           The operation of the ENC28J60 depends entirely on
 *           commands given by an external host controller over the
 *           SPI interface. These commands take the form of
 *           instructions, of one or more bytes, which are used to
 *           access the control memory and Ethernet buffer spaces.
 *           At the least, instructions consist of a 3-bit opcode,
 *           followed by a 5-bit argument that specifies either a
 *           register address or a data constant. Write and bit field
 *           instructions are also followed by one or more bytes of
 *           data.
 * Read Control Register (RCR)      | 0 0 0 | a a a a a |           N/A
 * Read Buffer Memory (RBM)         | 0 0 1 | 1 1 0 1 0 |           N/A
 * Write Control Register (WCR)     | 0 1 0 | a a a a a | d  d  d  d  d  d  d  d
 * Write Buffer Memory (WBM)        | 0 1 1 | 1 1 0 1 0 | d  d  d  d  d  d  d  d
 * Bit Field Set (BFS)              | 1 0 0 | a a a a a | d  d  d  d  d  d  d  d
 * Bit Field Clear (BFC)            | 1 0 1 | a a a a a | d  d  d  d  d  d  d  d
 * System Reset Command (Soft)(SRC) | 1 1 1 | 1 1 1 1 1 |           N/A
 * Legend: a = control register address, d = data payload.
*/

/** \brief  Send read command
 *  \param  op Opcode de 3 bits
 *  \param  address Registro que debe ser leido 
*/
uint8_t read_command(uint8_t op, uint8_t address) {

    enc_select();

    /* Enviar el comando de lectura */
    SPI.transfer(op | (address & ADDR_MASK));
    /* Leer el dato */
    if(address & 0x80) {
        // do dummy read if needed (for mac and mii, see datasheet page 29)
        SPI.transfer(0x00);
    }
    uint8_t c = SPI.transfer(0x00);

    enc_deselect();
    return c;
}

/** \brief Send write command
 *  \param op Opcode de 3 bits (ver \link commands \endlink)
 *  \param address Registro que debe ser modificado
 *  \param data Datos enviados
*/
void write_command(uint8_t op, uint8_t address, uint8_t data) {
    
    enc_select();

    /* write command */
    SPI.transfer(op | (address & ADDR_MASK));
    /* write data */
    SPI.transfer(data);

    enc_deselect();
 
 }

/** 
 * \brief Set bank 
*/
 void set_bank(uint8_t address) {

    /* Verifica si ya está en el banco */
    if((address & BANK_MASK) != bank) {
        // set the bank
        write_command(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
        write_command(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK) >> 5);
        bank = (address & BANK_MASK);
    }
 
 }

 uint8_t read_reg(uint8_t address) {

    set_bank(address);
    return read_command(ENC28J60_READ_CTRL_REG, address);

 }

 /** \brief Escribir el registro de 16 bits */
 void write_reg_8(uint8_t address, uint16_t data) {

    set_bank(address);
    write_command(ENC28J60_WRITE_CTRL_REG, address, data);

 }

 /** \brief Escribir el registro de 16 bits */
void write_reg_16(uint8_t address, uint16_t data) {

    set_bank(address);
    write_command(ENC28J60_WRITE_CTRL_REG, address, (data&0xFF));
    write_command(ENC28J60_WRITE_CTRL_REG, address + 1, (data) >> 8);

 }

 /** 
 * 
 */
uint8_t get_rev(void) {

    enc_spi_enable();

    uint8_t res = read_reg(EREVID);
    if (res == 0xFF) {
        res = 0;
    }

    enc_spi_disable();
    
    return res;
}

/** 
 * 
 */
void write_phy(uint8_t address, uint16_t data) {
    
    write_reg_8(MIREGADR, address);
    write_reg_16(MIWR, data);
    while (read_reg(MISTAT) & MISTAT_BUSY);

 }

/** 
 * 
 */
void free_frame_arch(void) {

    enc_spi_enable();

    /* Mover el puntero RX read al comienzo del próximo paquete */
    write_reg_16(ERXRDPT, next_frame_ptr == RXSTART_INIT ? RXSTOP_INIT : next_frame_ptr - 1);

    enc_spi_disable();

}

/** 
 *  Leer un único byte
 */
uint8_t read_byte(uint16_t addr){

    /* Habilitar el SPI para el ENC */
    enc_spi_enable();

    write_reg_16(ERDPT, addr);
    
    /* Selecionar ENC en SPI */
    enc_select();

    /* Invocar comando de lectura de SPI */
    SPI.transfer(ENC28J60_READ_BUF_MEM);

    /* Leer dato SPI */
    uint8_t c = SPI.transfer(0x00);
    
    /* Desabilitar el SPI para ENC */
    enc_deselect();
    enc_spi_disable();

    return c;

}

/** 
 *  Escribir un único byte
 */
void write_byte(uint16_t addr, uint8_t data){

    /* Habilitar el SPI para el ENC */
    enc_spi_enable();

    write_reg_16(EWRPT, addr);
    
    /* Selecionar ENC en SPI */
    enc_select();

    /* Invocar comando de lectura de SPI */
    SPI.transfer(ENC28J60_WRITE_BUF_MEM);

    /* Escribir dato SPI */
    SPI.transfer(data);
    
    /* Desabilitar el SPI para ENC */
    enc_deselect();
    enc_spi_disable();

}

/** 
 * 
 */
bool receive_frame_arch(void) {

    uint8_t rxstat;
    uint16_t len;

    enc_spi_enable();

    /* Verificar si ha sido recibido algún paquete */
    if (read_reg(EPKTCNT) != 0) {

        /* From datasheet (7.2.2): The frames are preceded by a six-byte header which
        contains a Next frame Pointer, in addition to a receive status vector which 
        contains receive statistics, including the packe's size. */
        uint16_t read_ptr = next_frame_ptr + 6 > RXSTOP_INIT ? next_frame_ptr + 6 - RXSTOP_INIT + RXSTART_INIT : next_frame_ptr + 6;
        
        /* Situar el puntero de lectura al principio del paquete recibido para... */
        write_reg_16(ERDPT, next_frame_ptr);

        /* ...leer el puntero al próximo paquete */ 
        next_frame_ptr = read_command(ENC28J60_READ_BUF_MEM, 0);
        next_frame_ptr |= read_command(ENC28J60_READ_BUF_MEM, 0) << 8;

        /* Leer la longitud del paquete */
        len = read_command(ENC28J60_READ_BUF_MEM, 0);
        len |= read_command(ENC28J60_READ_BUF_MEM, 0) << 8;
        /* Quitar el CRC */
        len -= 4;

        /* Leer el vector receive status (ver tabla 7.3) */
        rxstat = read_command(ENC28J60_READ_BUF_MEM, 0);
        //rxstat |= read_command(ENC28J60_READ_BUF_MEM, 0) << 8;

        #if NET_DEBUG >= 3
        printf("Receive frame [%X-%X], next: %X, stat: %X, count: %d, -> ",
                                read_ptr,
                                (read_ptr + len) % (RXSTOP_INIT + 1),
                                next_frame_ptr,
                                rxstat,
                                read_reg(EPKTCNT));
        if ((rxstat & 0x80)!=0)
            printf("OK\r\n");
        else
            printf("Failed\r\n");
        #endif

        /* Decrementar el contador de paquetes indicando que ya se ha leido */
        write_command(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

        /* Chequear el CRC antes de devolver el buffer */
        if ((rxstat & 0x80) != 0) {

            received_frame.begin = read_ptr;
            received_frame.size = len;

            enc_spi_disable();

            return true;
        }

        /* Mover el puntero RX read al comienzo del próximo paquete */
        write_reg_16(ERXRDPT, next_frame_ptr == RXSTART_INIT ? RXSTOP_INIT : next_frame_ptr - 1);

    }

    /* No ha llegado paquete */
    enc_spi_disable();
    return false;
}


/** 
 * 
 */
uint16_t read_frame_arch (uint8_t * buffer, uint16_t max_len) {

    uint16_t len = received_frame.size;

    /** Se ajusta len al largo del buffer que se va a leer en caso que sea más grande. En caso que el 
     * buffer llegado a ENC sea más grande la transferencia se limitará al tamaño máximo de uip_len.
     */
    if (UIP_BUFSIZE < len)
        len = UIP_BUFSIZE;

    #if NET_DEBUG >= 3
    printf("Readed %d bytes from NIC\r\n", len);
    #endif

    /* Habilitar el SPI para el ENC */
    enc_spi_enable();

    /* Se posiciona el buffer que se quiere leer */
    if (received_frame.begin > RXSTOP_INIT)
        write_reg_16(ERDPT, received_frame.begin - RXSTOP_INIT + RXSTART_INIT);
    else
        write_reg_16(ERDPT, received_frame.begin);
    
    /* <<- Traer el buffer de ENC al buffer de uip <<- */
    
    /* Seleccionar el ENC en SPI */
    enc_select();

    /* Invocar comando de lectura de SPI */
    SPI.transfer(ENC28J60_READ_BUF_MEM);

    while(len) {
        len --;
        /* Leer dato SPI */
        * buffer = SPI.transfer(0x00);
        buffer ++;
    }
    
    /* Desabilitar el SPI para ENC */
    enc_deselect();
    enc_spi_disable();

    return len;
}

/* (revisar esto para optimizar) 
    esta variable recoge el tamanno del buffer que ha sido enviado al enc 
    para que sea enviado a la red por send 
*/
static uint16_t buffer_length = 0;

/** 
 * 
 */
void  write_frame_arch(uint8_t * buffer, uint16_t len) {

    buffer_length = len;

    /* Habilitar el SPI para el ENC */
    enc_spi_enable();

    /* Situamos la posición donde vamos a escribir el frame */
    write_reg_16(EWRPT, TXSTART_INIT);

    /* Se limita len a ?? */
    //if (len > frame->size - position) {
    //    len = frame->size - position;
    //}

    /* ->> Escribir el buffer en el ENC ->> */

    enc_select();

    /* Invocar comando de escritura de SPI */
    SPI.transfer(ENC28J60_WRITE_BUF_MEM);

    while(len) {
        len --;
        /* Escribir el dato */
        SPI.transfer(* buffer);
        buffer ++;
    }

    /* Desabilitar el SPI para ENC */
    enc_deselect();
    enc_spi_disable(); 

}

/** 
 * 
 */
bool send_frame_arch(void) {

    /* Apunto al bloque donde está el paquete y marco el pricipio y el fin */
    uint16_t start = TXSTART_INIT - 1;
    uint16_t end = start + buffer_length;

    /* Habilitar el SPI para el ENC */
    enc_spi_enable();

    /* ?? write control-byte (if not 0 anyway) */
    write_byte(start, 0);

    /* Comenzamos la transmisión. Se sitúan los punteros de inicio y fin */
    write_reg_16(ETXST, start);
    write_reg_16(ETXND, end);

    bool success = false;

    /* Intentar al menos TX_COLLISION_RETRY_COUNT veces en caso de colision 
        ver Rev. B7 Silicon Errata issues 12 and 13 */
    for (uint8_t retry = 0; retry < TX_COLLISION_RETRY_COUNT; retry++){

        // Reset the transmit logic problem. Errata 12
        write_command(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
        write_command(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
        write_command(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXERIF | EIR_TXIF);

        // send the contents of the transmit buffer onto the network
        write_command(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

        // wait for transmission to complete or fail
        uint8_t eir;
        while (((eir = read_reg(EIR)) & (EIR_TXIF | EIR_TXERIF)) == 0);
        write_command(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
        success = ((eir & EIR_TXERIF) == 0);
        if (success)
            break; // usual exit of the for(retry) loop

        // Errata 13 detection
        uint8_t tsv4 = read_byte(end + 4);
        if (!(tsv4 & 0b00100000)) // is it "late collision" indicated in bit 29 of TSV?
            break; // other fail, not the Errata 13 situation

    }

    enc_spi_disable();

    return success;

}


/* ------------------------------- mac_init() -------------------------------- */

/** \brief  Inicializar el ENC28J60 
 *  \note   Lo primero debería ser configurar el SPI pero este 
 *          debió haber sido inicializado por el sistema. 
*/
bool mac_init(uint8_t * macaddr) {

    printf("ini mac_init\n");

    enc_spi_enable();

    /* Reseteamos el sistema. Le damos un tiempo */
    write_command(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    delay(50);

    /* Inicializar la recepción y sus buffers ERXST and ERXND (ver en datasheet 6.1) */
    next_frame_ptr = RXSTART_INIT;

    /* Rx start, pointer address and stop */
    write_reg_16(ERXST, RXSTART_INIT);
    write_reg_16(ERXRDPT, RXSTART_INIT);
    write_reg_16(ERXND, RXSTOP_INIT);

    /* Tx start and stop pointers */
    write_reg_16(ETXST, TXSTART_INIT);
    write_reg_16(ETXND, TXSTOP_INIT);

    /* For broadcast frames we allow only ARP packtets */
    write_reg_8(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
    write_reg_16(EPMM0, 0x303f);
    write_reg_16(EPMCS, 0xf7f9);

    /* enable MAC receive */
    write_reg_8(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    /* enable automatic padding to 60bytes and CRC operations */
    write_command(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
    /* set inter-frame gap (non-back-to-back) */
    write_reg_16(MAIPG, 0x0C12);
    /* set inter-frame gap (back-to-back) */
    write_reg_8(MABBIPG, 0x12);
    /* set the maximum frame size */
    write_reg_16(MAMXFL, MAX_FRAMELEN);

    /* Config MAC address */
    write_reg_8(MAADR5, macaddr[0]);
    write_reg_8(MAADR4, macaddr[1]);
    write_reg_8(MAADR3, macaddr[2]);
    write_reg_8(MAADR2, macaddr[3]);
    write_reg_8(MAADR1, macaddr[4]);
    write_reg_8(MAADR0, macaddr[5]);

    /* no loopback of transmitted frames */
    write_phy(PHCON2, PHCON2_HDLDIS);

    set_bank(ECON1);
    
    /* Configure leds, LED_A=Link, LED_B=activity */
    write_phy(PHLCON, 0x476);

    /* enable interrutps */
    write_command(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
    /* enable frame reception */
    write_command(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

    enc_spi_disable();


    uint8_t eth_rev = get_rev();


    if (eth_rev) {
        
        #if NET_DEBUG >= 3
        printf("NIC ENC28J60 -> EtheRev: %d - ", eth_rev);
        #endif
        
        return true;

    }

    return false;

}

/*----------------------------- tap_poll() ----------------------------------*/

uint16_t mac_poll(uint8_t * frame, uint16_t max_len) {

    /* Si hay un frame viable */
    if(receive_frame_arch()){
        read_frame_arch(frame, max_len);
    }

    return received_frame.size;

}

/* ------------------------------ mac_send() --------------------------------- */

bool mac_send(uint8_t * frame, uint16_t len){

    write_frame_arch(frame, len);

    if(send_frame_arch()){
        return true;
    } 
    return false;
}


/* --------------------------- power and status ------------------------------ */

void nic_power_off(void) {

    /* Habilitar el SPI para el ENC */
    enc_spi_enable();
    enc_select();

    write_command(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_RXEN);
    delay(50);

    write_command(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_VRPS);
    delay(50);

    write_command(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PWRSV);

    /* Desabilitar el SPI para ENC */
    enc_deselect();
    enc_spi_disable(); 
}

void nic_power_on() {
    
    /* Habilitar el SPI para el ENC */
    enc_spi_enable();
    enc_select();
    
    write_command(ENC28J60_BIT_FIELD_CLR, ECON2, ECON2_PWRSV);
    delay(50);

    write_command(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
    delay(50);
  
    /* Desabilitar el SPI para ENC */
    enc_deselect();
    enc_spi_disable(); 

}


/** 
 * 
 */
uint8_t nic_is_active(void) {
    return get_rev();
}

/** 
 * 
 */
bool nic_link_status() {

    /* Habilitar el SPI para el ENC */
    enc_spi_enable();
    enc_select();

    uint16_t phy_phstat2_reg;

    write_reg_8(MIREGADR, PHSTAT2);
    write_reg_8(MICMD, MICMD_MIIRD);
    /* esperar a que la lectura de PHY se complete */
    while(read_reg(MISTAT) & MISTAT_BUSY) {
        delayMicroseconds(15);
    }  //and MIRDH
    write_reg_8(MICMD, 0);

    phy_phstat2_reg = (read_reg(MIRDL) | read_reg(MIRDH) << 8);

    bool res = (phy_phstat2_reg & 0x0400) > 0;

    /* Desabilitar el SPI para ENC */
    enc_deselect();
    enc_spi_disable(); 
  
    return res;

}
