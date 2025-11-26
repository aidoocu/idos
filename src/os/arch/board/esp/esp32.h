/**
 * @file esp32.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _ESP_32_H_
#define _ESP_32_H_


/* -------------------------- network ----------------------- */

#include <WiFi.h>


/* -------------------------- random ------------------------ */

#define idos_random() esp_random()


/* ---------------------------- ADC ------------------------- */

/* Resolución del ADC. Rango, 1 - 16 */
#define ADC_RESOLUTION 12


/* Inicializar el ADC */
//void adc_init(void)

#endif /* _ESP_32_H_ */