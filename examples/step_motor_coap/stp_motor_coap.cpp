
/* Incluir idOS ++ */
#include "os/idos.h"
#include <stdio.h>



/*  -------------- Step Motor -------------- */

/* Steps per 0.1 milimeters (tenths of a millimeter) */
#define STEPS_PER_TMM 20

/* Direction of movement */
#define UP HIGH
#define DOWN !UP
#define PIN_DIR 8

/* Step Pin */
#define PIN_STEP 9

/* Limit switch pin */
#define PIN_Z_HOME 7

/* Velocity of movement*/
#define WAIT_STEP 1000

/* Limit in Z */
#define Z_MAX 3000

/* Point on the z-axis at which it is currently */
static uint16_t z_point;


void z_move(int16_t tmm){

	/* Set direction of movement */
	if(tmm > 0)
		digitalWrite(PIN_DIR, UP);
	else
		digitalWrite(PIN_DIR, DOWN);

	/* from tmm to steps */
	tmm = tmm * STEPS_PER_TMM;

	for(int i=0; i < tmm; i++){
	
		digitalWrite(PIN_STEP,HIGH);
		delayMicroseconds(WAIT_STEP);
		digitalWrite(PIN_STEP,LOW);
		delayMicroseconds(WAIT_STEP);
	
	}
}


/* defiendo los callbacks de los recursos coap */

/* GET */
static uint8_t z_axis_get(coap_payload_st * payload) {
  
	char dstc_char[6];

    sprintf(dstc_char, "%d", z_point);

    payload->send_len = 6;
    memcpy(payload->send, dstc_char, payload->send_len);
 
  	return 1;
}

/**						PUT 
 * The payload containg just 2 bytes to conform an integer signed:
 * 		- 00 00 set to home
 * 		- Negative integer means going down
 * 		-Positive integer means going up
 * 	The resolution is STEPS_PER_TMM 0.1 milimeters (tenths of a 
 * 	millimeter).
 */
static uint8_t z_axis_put(coap_payload_st * payload) {

	/* Change the two uint_8 to a int_16 */
	int16_t ds;
	ds = payload->rcvd[0];
	ds = ds<<8;
	ds = ds & 0xFF00;
	ds = ds | payload->rcvd[1];

	/* If the mew position is out of the limits, the movement is't
	possible */
	if ((ds + z_point > Z_MAX) || (ds + z_point < 0))
		return 0;

	/* Home position */
	if (ds == 0){
		
		/* move down 0.1 mm and check if home has been reached. 
		If not, repeat until home has been reached.*/
		while (!digitalRead(PIN_Z_HOME)) {
			z_move(-1);
		}
		/* When it's in the home it rises to a safe distance */
		z_move(10);
		/* Current point is reset to 0 (home) */
		z_point = 0;
	} else {
		z_move(ds);
		z_point = z_point + ds;
	}

	/* Success */
	return 1;
}

/*  ------------- /Step Motor -------------- */



/*  --------------- idOS Task -------------- */

/*  */
TASK(task, "");

/*  */
TASKS_AUTO_START(&task)


TASK_PT(task){

  	TASK_BEGIN

    coap_resource(z_axis, "z", NULL);
    z_axis.get = * z_axis_get;
	z_axis.put = * z_axis_put;

    TASK_YIELD

	TASK_END

}

/*  -------------- /idOS Task -------------- */