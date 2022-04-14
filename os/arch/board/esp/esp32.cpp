/**
 * @file esp32.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifdef ARDUINO_ARCH_ESP32

#include "esp32.h"


/* ---------------------------- ADC ------------------------- */

/** 
 * \paragraph El ESP32 tiene 2 periféricos de hardware ADC de 12 
 * bits SAR (Successive Approximation Register) que pueden leer 
 * 18 canales de entrada analógica diferentes.
 * El driver del controlador del ADC admite el ADC1 (8 canales, 
 * conectados a los GPIOs 32 - 39), y el ADC2 (10 canales, 
 * conectados a los GPIOs 0, 2, 4, 12 - 15, y 25 - 27). Sin embargo, 
 * el uso de ADC2 tiene algunas restricciones para la aplicación:
 * \line El ADC2 es utilizado por el controlador Wi-Fi. Por lo tanto, 
 * la aplicación sólo puede utilizar ADC2 cuando el controlador Wi-Fi 
 * no se ha iniciado.
 * \line Algunos de los pines del ADC2 se utilizan como pines de 
 * conexión (GPIO 0, 2, 15), por lo que no se pueden utilizar libremente
 */

//void adc_init(void){ 

    /* Resolución del ADC. Por defecto es 12 bits (0 - 4096) */
//    analogReadResolution(ADC_RESOLUTION);

/*
  * Sets the sample bits and read resolution
  * Default is 12bit (0 - 4095)
  * Range is 9 - 12
  * */
  //analogSetWidth(12);

  /*
  * Set number of cycles per sample
  * Default is 8 and seems to do well
  * Range is 1 - 255
  * */
  //analogSetCycles(8);

  /*
  * Set number of samples in the range.
  * Default is 1
  * Range is 1 - 255
  * This setting splits the range into
  * "samples" pieces, which could look
  * like the sensitivity has been multiplied
  * that many times
  * */
  //analogSetSamples(1);

  /*
  * Set the divider for the ADC clock.
  * Default is 1
  * Range is 1 - 255
  * */
  //analogSetClockDiv(1);

  /*
  * Set the attenuation for all channels
  * Default is 11db
  * */
  //analogSetAttenuation(ADC_11db); //ADC_0db, ADC_2_5db, ADC_6db, ADC_11db

  /*
  * Set the attenuation for particular pin
  * Default is 11db
  * */
  //analogSetPinAttenuation(36, ADC_0db); //ADC_0db, ADC_2_5db, ADC_6db, ADC_11db

  /*
  * Get value for HALL sensor (without LNA)
  * connected to pins 36(SVP) and 39(SVN)
  * */
  //hallRead();

  /*
  * Non-Blocking API (almost)
  *
  * Note: ADC conversion can run only for single pin at a time.
  *       That means that if you want to run ADC on two pins on the same bus,
  *       you need to run them one after another. Probably the best use would be
  *       to start conversion on both buses in parallel.
  * */

  /*
  * Attach pin to ADC (will also clear any other analog mode that could be on)
  * */
  adcAttachPin(36);

  /*
  * Start ADC conversion on attached pin's bus
  * */
  adcStart(36);

  /*
  * Check if conversion on the pin's ADC bus is currently running
  * */
  //adcBusy(uint8_t pin);

  /*
  * Get the result of the conversion (will wait if it have not finished)
  * */
  //adcEnd(uint8_t pin);

  /**
  * When VDD_A is 3.3V:
  *
  * - 0dB attenuaton (ADC_ATTEN_DB_0) gives full-scale voltage 1.1V
  * - 2.5dB attenuation (ADC_ATTEN_DB_2_5) gives full-scale voltage 1.5V
  * - 6dB attenuation (ADC_ATTEN_DB_6) gives full-scale voltage 2.2V
  * - 11dB attenuation (ADC_ATTEN_DB_11) gives full-scale voltage 3.9V (see note below)
  *
  * @note The full-scale voltage is the voltage corresponding to a maximum reading (depending on ADC1 configured
  * bit width, this value is: 4095 for 12-bits, 2047 for 11-bits, 1023 for 10-bits, 511 for 9 bits.)
  *
  * @note At 11dB attenuation the maximum voltage is limited by VDD_A, not the full scale voltage.
  */  

//} 

#endif /* ARDUINO_ARCH_ESP8266 */