
/* Incluir idOS ++ */
#include <idos.h>
#include <stdio.h>



/*  -------------- Step Motor -------------- */

/* Steps per 0.1 milimeters (tenths of a millimeter) */
#define STEPS_PER_TMM 2

/* Direction of movement */
#define UP LOW
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

	printf("tmm = %d\n", tmm);

	/* Set direction of movement */
	if(tmm > 0) {
		//digitalWrite(PIN_DIR, UP);
		printf("up\n");
	}
	else {
		//digitalWrite(PIN_DIR, DOWN);
		tmm = -tmm;
		printf("down\n");
	}

	/* from tmm to steps */
	tmm = tmm * STEPS_PER_TMM;

	for(int i=0; i < tmm; i++){
	
		//digitalWrite(PIN_STEP,HIGH);
		//delayMicroseconds(WAIT_STEP);
		//digitalWrite(PIN_STEP,LOW);
		//delayMicroseconds(WAIT_STEP);
		printf("-");
	
	}
	printf("\n");
}

void set_home() {

	/* Siempre empezar en el home */
	/* move down 0.1 mm and check if home has been reached. 
	If not, repeat until home has been reached.*/
	//while (digitalRead(PIN_Z_HOME)) {
	//	z_move(-1);
	//}
	/* When it's in the home it rises to a safe distance */
	//z_move(10);
	/* Current point is reset to 0 (home) */
	z_point = 0;

	printf("Home position\n");

}


/* defiendo los callbacks de los recursos coap */

/* GET */
static uint8_t z_axis_get(coap_payload_st * payload) {
  
	char dstc_char[2];

    dstc_char[0] = z_point & 0x00FF;
	dstc_char[1] = (z_point >> 8) & 0x00FF;

	printf("payload: %x %x\n", dstc_char[0], dstc_char[1]);

/* 	char str_debug[3];
	str_debug[2] = '\0';
	str_debug[1] = dstc_char[0];
	str_debug[0] = dstc_char[1];

	printf("payload: %ld\n", strtol(str_debug, NULL, 10)); */

    payload->send_len = 2;
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
	int16_t z_new;
	z_new = payload->rcvd[1];
	z_new = z_new<<8;
	z_new = z_new & 0xFF00;
	z_new = z_new | payload->rcvd[0];

	printf("recv: %x %x\n", payload->rcvd[1], payload->rcvd[0]);
	printf("z new: %d\n", z_new);

	/* If the mew position is out of the limits, the movement is't
	possible */
	if ((z_new > Z_MAX) || (z_new < 0))
		return 0;

	/* Home position */
	if (z_new == 0){
		set_home();
	} else {
		/* ds = z_new - z_point */
		z_move(z_new - z_point);
	}

	/* Update the z_point */
	z_point = z_new;

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

	//pinMode(PIN_DIR, OUTPUT);
	//pinMode(PIN_STEP, OUTPUT);
	//pinMode(PIN_Z_HOME, INPUT_PULLUP);	

	set_home();

    TASK_YIELD

	TASK_END

}

/*  -------------- /idOS Task -------------- */