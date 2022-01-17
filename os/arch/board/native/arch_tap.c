/** 
 * 
 * 
 */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <unistd.h>
#include <sys/select.h>
#include <time.h>

#define DEVTAP "/dev/tap0"

#include "arch_tap.h"


//// fake
#define UIP_BUFSIZE 98

/* subnet, ¡¡¡¡¡ esto debe ser configurable !!!! */
static char network[] = "172.18.0.0";

/** Manipulador (file descriptor) del tap */
static int fd_tap;

uint16_t temp_uip_len;
static char temp_uip_buf[UIP_BUFSIZE];

/*-------------------------------- sys_cmd() --------------------------------*/

//esta funcion tal vez deba estar en otro archivo mas global
static int sys_cmd(const char * cmd_buf, ...) {

    /* Memoria para el comando formateado y... */
    char cmd[250];
    /* ... lista de variables */
    va_list args;

    /* Cargando la lista de variables y formateando el comando */
    va_start(args, cmd_buf);
    vsnprintf(cmd, sizeof(cmd), cmd_buf, args);
    va_end(args);

    return system(cmd);

}

/*----------------------------- remove_route() ------------------------------*/

static void remove_route(void) {

  sys_cmd("route delete -net %s", network);

}

/*------------------------------- tap_init() --------------------------------*/

bool tap_init(void){

    /* Abrir tap */
    fd_tap = open(DEVTAP, O_RDWR);
    /* Si no fue posible habrir tap notificamos y salimos */
    if(fd_tap == -1) {
   	    perror("tap: open");
   	    return false;
    }

    /* Inicializar la interface y la ruta hacia la red */
	sys_cmd("ifconfig tap0 inet 172.18.0.1/16");
	sys_cmd("route add -net %s/16 -iface tap0", network);

    /* Eliminar la ruta una vez que termine */
	atexit(remove_route);

	return true;

}


/*----------------------------- tap_poll() ----------------------------------*/

uint16_t tap_poll(void) {
	
	int ret;

	/* Limite de tiempo antes de que termine la espera de 
	comprobación debe ser 0 */
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	/** Para comprobar si hay nuevos datos asociamos tap 
	 *  a un conjunto de sockets y chequeamos su estado.
	 */

	/* Declarar el conjunto de sockets a chequear */
	fd_set fdset;
	/* Inicializar el conjunto */
	FD_ZERO(&fdset);
	/* Si efectivamente está tap abierta... */
	if(fd_tap > 0) {
		/* ...se añade tap al conjunto y... */
		FD_SET(fd_tap, &fdset);
	}
	/* ...se verifica si hay algo para leer en tap */
	ret = select(fd_tap + 1, &fdset, NULL, NULL, &tv);

	/** select retorna la cantidad de sockets del conjunto 
	 *  que tienen datos, en este caso hay solo uno, por lo
	 *  que no es necesario ver cual socket es. Si retorna
	 *  0, es que no hay datos en el socket */
	if(ret == 0) {
		return 0;
	}
	
	/* Si hay algo que leer, se lee */
	ret = read(fd_tap, temp_uip_buf, UIP_BUFSIZE);



	if(ret == -1) {
		perror("tapdev_poll: read");
	}

	printf("tapdev_poll: read %d bytes\n", ret);

	temp_uip_len = ret;

/* 	for(int i = 0; i < ret; i++) {
		printf("%c", temp_uip_buf[i]);
	}
	printf("\n"); */

	return ret;
}

/*------------------------------ tap_send() ---------------------------------*/

void tap_send(void){
    int ret;

	if(fd_tap <= 0) {
		return;
	}

	#if DROP
	drop++;
	if(drop % 8 == 7) {
		fprintf(stderr, "Dropped an output packet!\n");
		return;
	}
	#endif /* DROP */

	printf("tapdev_send: sending %d bytes\n", temp_uip_len);
	ret = write(fd_tap, temp_uip_buf, temp_uip_len);

	if(ret == -1) {
		perror("tap_dev: tapdev_send: writev");
		exit(1);
	}
}




//-------- fake ------------------------------

uint16_t tap_read(uint8_t* buffer) {

	memcpy(buffer, temp_uip_buf, temp_uip_len);
	return temp_uip_len;

}