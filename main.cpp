/** 
 * @file main.cpp
 * @brief Main file of the mote
 * @version 1.0
 * @date 2021-09-15
 * @author Bernardo Yaser León Ávila
 * 
 * Hardware: 
 *  - Microprocessor: ATmega328P
 *  - Sensor: BME280
 *  - Communication: nRF24L01
 * 
 * The main program reads the sensor data each PERIOD_TEMP and PERIOD_MOIST and sends it 
 * through the nRF24L01 module each PERIOD_SEND.
 * 
 */
#include "os/idos.h"

/* -------------------- este tramo hay que pasarlo al driver (arch/dev) ------------------------ */

#include "os/arch/dev/sensor/bosh/bme280/BME280I2C.h"
#include <Wire.h>

BME280I2C::Settings settings(
	BME280::OSR_X1,
	BME280::OSR_X1,
	BME280::OSR_X1,
	BME280::Mode_Forced,
	BME280::StandbyTime_1000ms,
	BME280::Filter_16,
	BME280::SpiEnable_False,
	BME280I2C::I2CAddr_0x76
);
/* 
BME280I2C bme;   // Default : forced mode, standby time = 1000 ms
                // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off, 
*/
static BME280I2C bme(settings);

/* ------------------- /este tramo hay que pasarlo al driver (arch/dev) ------------------------ */

/* Interval for reading temperature, moisture and for sending the sada, all in milliseconds */
#define PERIOD_TEMP 1000
#define PERIOD_HUM 1000
#define PERIOD_SEND 2000

/* Variables for the sensor data */
static double last_temp = 0.0f;
static double last_hum = 0.0f;


/* protohilos */
TASK(task_temp, "T");
TASK(task_hum, "H");
TASK(task_send, "S");

/* Arranco el primer protohilo al inicio */
TASKS_AUTO_START(&task_temp, &task_hum, &task_send);


/* Mote main */

TASK_PT(task_temp){


	TASK_BEGIN

		/* -------------------- este tramo hay que pasarlo al driver (arch/dev) ------------------------ */

		/* Sensor init */
			Wire.begin();
 			if(!bme.begin()){
				#ifdef DEBUG_MAIN
					printf("BME280 error\n");
				#endif
				//hay que sennalarle al programa que el sensor esta error y no recogera datos
				//esto se puede traducir en que la tarea intentara inicializar el sensor hasta que lo logre
				//y/o mandar un mensaje al controlador en la nube para que este decida que hacer
			}

		/* ------------------- /este tramo hay que pasarlo al driver (arch/dev) ------------------------ */	

		/* Seteamos creamos un timer y lo seteamos a 0,5 seg */
		timer_set(timer_temp, PERIOD_TEMP);

		while (1) {

			TASK_YIELD

			printf("temp wakeup\n");

			if(timer_expired(&timer_temp)){

				/* Read temperature */
				last_temp = bme.temp();
				#ifdef DEBUG_MAIN
					//fprintf(stdout, "temp %.2f\n", last_temp);
					printf("temp ");
					Serial.println(last_temp);
				#endif
				timer_reset(&timer_temp);
			}
		}
		

	TASK_END
}


TASK_PT(task_hum){
	
	TASK_BEGIN

		//printf("task humidity\n");

		timer_set(timer_hum, PERIOD_HUM);

		while (1) {

			TASK_YIELD

			printf("hum wakeup\n");

			if(timer_expired(&timer_hum)){
				/* Read hum */
				last_hum = bme.hum();
				#ifdef DEBUG_MAIN
					//printf("hum %.2f\n", last_hum);
					printf("hum ");
					Serial.println(last_hum);
				#endif
				timer_reset(&timer_hum);
			}
		}

		TASK_END
}
#define SEND_LENGTH 18

char dstc_char[SEND_LENGTH];
uint8_t dstc[SEND_LENGTH];
static uint8_t dst_addr[5] = {0xE8, 0xE8, 0xF0, 0xF0, 0xE1};

TASK_PT(task_send){
	
	TASK_BEGIN

		//printf("task send\n");

		timer_set(timer_send, PERIOD_SEND);

		/* Set destination address {0xE8, 0xE8, 0xF0, 0xF0, 0xE1} */
		set_nrf_dst_addr(dst_addr);

		while (1) {

		TASK_YIELD

		printf("send wakeup\n");
		
			if(timer_expired(&timer_send)){

				sprintf(dstc_char, "Temp: %u, Hum: %u", (uint8_t)last_temp, (uint8_t)last_hum);

				for(int i = 0; i < SEND_LENGTH; i++){
					dstc[i] = (uint8_t)dstc_char[i];
				}

				/* Send data */
				mac_send(dstc, SEND_LENGTH);

				timer_reset(&timer_send);
			}
		}

	TASK_END
}