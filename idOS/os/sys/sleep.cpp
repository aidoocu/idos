#include "sleep.h"

void deep_sleep(void){
    sleep_arch();
}

void sleep_mode_init(void){
    sleep_init_arch();
}