/** 
 * 
 */

#include "arch_net.h"

/* subnet, ¡¡¡¡¡ esto debe ser configurable !!!! */
static char network[] = "172.18.0.0";

/** Manipulador (file descriptor) del tap */
static int fd_tap;

/*----------------------------- remove_route() ------------------------------*/

static void remove_route(void) {

  sys_cmd("route delete -net %s", network);

}

/*------------------------------- mac_init() --------------------------------*/

bool mac_init(uint8_t * mac){

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

uint16_t mac_poll(uint8_t * frame, uint16_t max_len) {
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
    // hay que analizar aqui la limitacion que puede tener esto
	ret = read(fd_tap, frame, max_len);

	if(ret == -1) {
		perror("tapdev_poll: read");
        return 0;
	}

	//printf("tapdev_poll: read %d bytes\n", ret);

	return ret;
}

/*------------------------------ mac_send() ---------------------------------*/

bool mac_send(uint8_t * frame, uint16_t len){
    int ret;

	if(fd_tap <= 0) {
		return false;
	}

	#if DROP
	drop++;
	if(drop % 8 == 7) {
		fprintf(stderr, "Dropped an output packet!\n");
		return;
	}
	#endif /* DROP */

	//printf("tapdev_send: sending %d bytes\n", len);
	ret = write(fd_tap, frame, len);

    /* Si no se pudo escribir en tap se retorna el fail */
	if(ret == -1) {
		perror("tap_dev: tapdev_send: writev");
		return false;
	}
    return true;
}

 /// Quitar 
void free_frame_arch(void){
    return;
}


/* << estado de la interface >> */
uint8_t nic_is_active(void){
    return 1;
}

bool nic_link_status(void){
    return true;
}

void nic_power_off(void){
    return;
}

void nic_power_on(void){
    return;
}