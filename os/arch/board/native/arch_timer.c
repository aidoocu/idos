/** 
 * 
 * 
 */

#include "arch_timer.h"


/* Obtener el tiempo del sistema ... */
time_t get_time() {

    /** Buscamos si está disponible un reloj monótono. Esto debería ser cierto para 
     *  MacOS X, Linux y otros sistemas POSIX. Esto nos permitiría el uso de clock_gettime() 
     */
    #ifdef CLOCK_MONOTONIC

    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    /* Retornar el tiempo a usec a partir de (sec * 1000) + (nsec / 1000) */
    return (time_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;


    //#endif
    /** Si no está disponible la mejor opción es gettimeofday */
    #else

    struct timeval tv;

    gettimeofday(&tv, NULL);

    /* Retornar el tiempo a usec a partir de (sec * 1000) + msec */
    return (time_t)(tv.tv_sec * 1000000 + tv.tv_usec);

    #endif /* CLOCK_MONOTONIC */

}

time_t  msec_now(){
    //clock();
    return(get_time() / 1000);
}


time_t usec_now() {
    return(get_time());
}

