/**
 * 
 * 
 * 
 */
#ifndef _IDOS_BME280_H_
#define _IDOS_BME280_H_

#if defined(BME_280)

/* ToDo esto tiene que pasarse a la biblioteca del BME280 que se carge desde el entorno
no incluirla como parte del proyecto
seria:*/
/* #include <BME280I2C.h> */

#include "BME280/src/BME280I2C.h"
#include <Wire.h>

/* Sensor init */
void bme280_init(void);

/* get temperature */
float bme280_get_temp(void);

/* get humidity */
uint8_t bme280_get_hum(void);

#endif /* BME_280 */

#endif /* _IDOS_BME280_H_ */