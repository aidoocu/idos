/**
 * 
 * 
 * \file BME280.cpp
 * 
 */

#if defined(BME_280)

#define DEBUG_BME280

#include "bme280.h"

/* Descomentar y modificar en el caso que deban cambiar los settings */
/* BME280I2C::Settings settings(
	BME280::OSR_X1,
	BME280::OSR_X1,
	BME280::OSR_X1,
	BME280::Mode_Forced,
	BME280::StandbyTime_1000ms,
	BME280::Filter_16,
	BME280::SpiEnable_False,
	BME280I2C::I2CAddr_0x76
); */
 
BME280I2C bme;   // Default : forced mode, standby time = 1000 ms
				// Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off, 


/* Sensor init */
void bme280_init() {
    
    Wire.begin();
		
    if(!bme.begin()){
        
        #ifdef DEBUG_BME280
        printf("BME280 error\n");
        #endif
		return;
    }

	printf("BME280 init\n");
}

/* get temperature */
float bme280_get_temp() {
    return bme.temp();
}

/* get humidity */
uint8_t bme280_get_hum() {
    return bme.hum();
}

#endif /* BME_280 */