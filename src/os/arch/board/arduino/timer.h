

/* ------------------------- Timer -------------------------- */


/* Tipo de dato para guardar tiempo, sea en mseg o usec */
typedef unsigned long time_t; 

/** 
 * \brief   Devuelve los milisegundos del sistema 
 * */
time_t  msec_now();

/** 
 * \brief   Devuelve los microsegundos del sistema 
 * */
time_t usec_now();