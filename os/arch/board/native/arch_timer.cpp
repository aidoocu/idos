/** 
 * 
 * 
 */

#include "arch_timer.h"

time_t  msec_now(){
    return 2;
}


time_t usec_now() {
    return 20;
}

/////// Esto no deberia estar definido
void timer_sys_init_arch(void){

    return;
}
