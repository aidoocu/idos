/** 
 * 
 */

#include "arch_net.h"
#include "arch_tap.h"

bool mac_init_arch(uint8_t * mac){

    return tap_init();

}

uint16_t frame_size_arch(void){
    return 100;
}

bool receive_frame_arch(void){
    uint16_t frame_size;
    frame_size = tap_poll();

    if(frame_size > 0){
        return true;
    }
    return false;
}

uint16_t read_frame_arch(uint8_t* buffer, uint16_t len){
    return tap_read(buffer);
}

void write_frame_arch(uint8_t* tx_buffer, uint16_t len){
    return;
}

bool send_frame_arch(void){
    return true;
}



/* <<>> */

void free_frame_arch(void){
    return;
}

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