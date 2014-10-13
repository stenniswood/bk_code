/********************************************************
This module combines thrust with tilt balancing
and packages the data to be sent to the appropriate
BigMotorEn boards.

INPUT:
	float Thrust
	float x_tilt
	float y_tilt
	float x_alpha;	// x & y alphas may be different due to different intertias of the copter 
	float y_alpha;	// in the respective orthogonal directions.

OUTPUT:
	instance1 - speed1, speed2
	instance2 - speed1, speed2

*********************************************************/
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "pican_defines.h"
#include "CAN_Interface.h"
#include "packer_motor.h"
#include "can_txbuff.h"
#include "packer.h"
//#include "system_msgs_callback.h"
//#include "board_list.h"
#include "can_id_list.h"
//#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "catagorization.h"
#include "fuse_ag.h"
#include "ini_file.h"
#include "timer.h"

struct sCAN 	msg2;

byte instance1 = 0;
byte instance2 = 0;

float thrust = 1.0;			// 0.0 to 1.0   Percent/100
float x_alpha = 1.0;		// 0.0 to 1.0   Percent/100
float y_alpha = 1.0;		// 0.0 to 1.0   Percent/100

float x_tilt = 0;	// latest reading from tilt sensor
float y_tilt = 0;	// latest reading from tilt sensor


/************************************************
	Setup the motor boards for FREE_SPIN & 2 MOTOR 
 	with as low base Frequency as possible.
 ************************************************/
void configure_motor_boards( byte instance1, byte instance2 )
{
	byte Mask  = 0xFF;
	byte Value = 0x0A;		// 2 motors; free spin, slowest base frequency
	pack_configure( &msg1, instance1, 1, Mask, Value );
	AddToSendList ( &msg1 );
	pack_configure( &msg2, instance2, 1, Mask, Value );
	AddToSendList ( &msg2 );
	
	print_message( &msg1);
	print_message( &msg2);
}

/************************************************
 INPUT:	
 	mThrustFraction	- [0.0 .. 1.0]  
 OUTPUT:
 	The value is stored for computation
 	during the timeslice (mixing in the balance)
 ************************************************/
void AdjustThrust(float mThrustFraction)
{
	thrust = mThrustFraction;
}

/************************************************
 INPUT:	
 	mx_tilt	- [0.0 .. 1.0]  
 	my_tilt	- [0.0 .. 1.0]  
 	
 OUTPUT:
 	The value is stored for computation
 	during the timeslice (mixing in the balance)
 ************************************************/
void UpdateTiltReading( float mx_tilt, float my_tilt )
{
	x_tilt = mx_tilt;
	y_tilt = my_tilt;
}

/************************************************
 INPUT:	
 	mx_tilt	- [0.0 .. 1.0]  
 	my_tilt	- [0.0 .. 1.0]  
 	
 OUTPUT:
	Takes the stored info and computes each motors
	power based on Requested Thrust (percent of maximum)
	and tilt angle.
	
Update Rate:
	50ms	==> 20 Hertz
	20ms	==> 50 Hertz
 ************************************************/
void power_timeslice_50ms()
{
	float m1, m2, m3, m4;	
	m1 = thrust + x_alpha * x_tilt;
	m2 = thrust - x_alpha * x_tilt;
	m3 = thrust + y_alpha * y_tilt;
	m4 = thrust - y_alpha * y_tilt;

	pack_move_dual_speed( &msg1, instance1, m1, m2 );
	print_message( &msg1 );
	AddToSendList( &msg1);	

	pack_move_dual_speed( &msg2, instance2, m3, m4);
	print_message( &msg2 );
	AddToSendList( &msg2 );

}
