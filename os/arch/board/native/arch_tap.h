/** 
 * 
 * \details Una interfaz TAP es un dispositivo virtual de la capa de enlace. La
 *  capa física se maneja con un descriptor de archivo (fd) que la representa.
 *  Luego se escriben datos de frame Ethernet sin procesar en el fd que serán 
 *  tratados como cualquier frame Eth. */

#ifndef _ARCH_TAP_H_
#define _ARCH_TAP_H_

#include <stdbool.h>
#include <stdint.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
//#include <unistd.h>
//#include <string.h>
//#include <sys/ioctl.h>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <sys/time.h>
//#include <sys/uio.h>
//#include <sys/socket.h>

#ifdef Darwin
#define DEVTAP "/dev/tap0"
#elif Linux
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#define DEVTAP "/dev/net/tun"
#else 
#define DEVTAP "/dev/null"
#endif


bool tap_init(void);

uint16_t tap_poll(void);

void tap_send(void);


//---------- fake --------------------------------

uint16_t tap_read(uint8_t* buffer);

#endif /* _ARCH_TAP_H_ */