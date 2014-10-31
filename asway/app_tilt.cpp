// Application Specific commands for Tilt
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "pican_defines.h"
#include "CAN_Interface.hpp"
#include "packer_lcd.h"
#include "can_txbuff.h"
#include "can_rxbuff.hpp"
#include "packer.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "catagorization.h"
#include "fuse_ag.h"
#include "fuse_gyro.h"
#include "preferences.hpp"
#include "help.h"
#include <pthread.h>
#include "packer_motor.h"
#include "fuse_accel.h"
#include "sway_config.hpp"
#include "mcp2515.h"
#include "mcp2515_defs.h"
#include "sway_config.hpp"

bool	EmergencyStop = false;
#define MEDIAN_ANGLE_FILTER_SIZE 7

// This will hold the last 5 angle measurements.
// we'll sort them into another array.
// and then pick the middle number.
float median_angle_filter[MEDIAN_ANGLE_FILTER_SIZE];
float sorted_angles		 [MEDIAN_ANGLE_FILTER_SIZE];

int count_accel  = 0;
int count_gyro   = 0;
int count_magnet = 0;
int count_samples= 0;

float TiltAngle 	= 0.;
float TiltAnglePrev	= 0.;


// Call back function for system msg processing 
BOOL (*tiltsensor_callback)() = NULL;	
void set_tiltsensor_callback( BOOL (*mtiltsensor_callback)() )
{
	tiltsensor_callback = mtiltsensor_callback;
}

/***************************************************************
  Median Filter Routines - for removing spikes in accelerometer
 ***************************************************************/
int compare(const void * elem1, const void * elem2) 
{
    float f = *((float*)elem1);
    float s = *((float*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}
void copy_to_sort(  )
{
	for (int i=0; i<MEDIAN_ANGLE_FILTER_SIZE; i++)
		sorted_angles[i] = median_angle_filter[i];
}
void sort_median_filter( )
{
	qsort (sorted_angles, sizeof(sorted_angles)/sizeof(*sorted_angles), 
    		sizeof(*sorted_angles), compare );
	// does it in place!
}
void add_to_median_filter( float mNewest )
{
	for (int i=0; i<MEDIAN_ANGLE_FILTER_SIZE-1; i++)
		median_angle_filter[i] = median_angle_filter[i+1];
	median_angle_filter[MEDIAN_ANGLE_FILTER_SIZE-1] = mNewest;
}

float median_filter( float raw_angle )
{
	// MEDIAN FILTERING:
	add_to_median_filter( raw_angle );
	copy_to_sort		(	);
	sort_median_filter	( 	);
	byte middle = floor((MEDIAN_ANGLE_FILTER_SIZE/2.0)+0.5);
	float angle = sorted_angles[middle];
	return angle;
}

float get_raw_tilt_angle()
{
	float raw_angle=0.;
	switch (sc.get_Axis())
	{
		case 'x' : raw_angle = AccelAngularPosition.rx; 	break;
		case 'y' : raw_angle = AccelAngularPosition.ry; 	break;
		case 'z' : raw_angle = AccelAngularPosition.rz; 	break;
		default	 : break;
	}
	return raw_angle;
}

/*******************************************************************
   Load Balancing Algorithm  (Alhpa adjustments)
A) Bins for alpha based on tilt angle
B) 2 consequetive timeslices with no significant change,
		increase alpha by 10%
C) 2 consequetive timeslices with more than 5 degrees/100th second, 
		decrease alpha by 10%
 *******************************************************************/

struct state_variables
{
	float angle;
	float angular_rate;
	float position;
	float speed;
	
	float composite;
};
struct constants
{
	float Kangle;
	float Kangular_rate;
	float Kposition;
	float Kspeed;
};
struct s_pid_state
{
	float integral;
	float derivative;
} PID_state;

/*****************************************
			PID FUNCTIONS
*****************************************/
struct s_pid_params
{
	// Gain parameters
	float  Kgain;        // Loop gain parameter
	float  Ti;           // Integrator time constant
	float  Td;           // Differentiator time constant
	float  deltaT;         // Update time interval
} PID_params;

struct constants	   KError;
struct state_variables Nominal;
struct state_variables Error;

void pid_init()
{
	KError.Kangle 		 = ;
	KError.Kangular_rate = ;
	KError.Kposition 	 = ;
	KError.Kspeed	 	 = ;

	PID_state.integral 		= 0.;
	PID_state.derivative 	= 0.;

	PID_params.Kgain 		=;
	PID_params.Ti			=;
	PID_params.Td			=;	
	PID_params.deltaT		=;
}

void pid_set_nominal( float mangle, float mrate, float mposition, float mspeed )
{
	Nominal.angle 		 = mangle;
	Nominal.angular_rate = mrate;	
	Nominal.position     = mposition;
	Nominal.speed		 = mspeed;
}

float calc_error( float mangle, float mangular_rate, float mposition, float mspeed )
{
	// COMPUTE ERROR FROM NOMINAL:
	Error.angle 		= (mangle 		 - Nominal.angle		);
	Error.angular_rate 	= (mangular_rate - Nominal.angular_rate	);
	Error.position 		= (mposition	 - Nominal.position		);
	Error.speed    		= (mspeed		 - Nominal.speed		);

	// COMPUTE COMPOSITE:
	Error.composite  = KError.angle 		* Error.angle;
	Error.composite += KError.angular_rate 	* Error.angular_rate;
	Error.composite += KError.position 		* Error.position;
	Error.composite += KError.speed 		* Error.speed;
	return Error.composite;
}

float pid_computations( float mangle, float mangular_rate, float mposition, float mspeed )
{
	// PROPORTIONAL:
	float error = calc_error(  mangle, mangular_rate, mposition, mspeed );
	pid_out 	= error;

	// INTEGRAL:
	pidout 			   += PID_state.integral * PID_params.deltaT / PID_params.Ti;
	PID_state.integral += error;
	
	// DERIVATIVE:
	float change 	 	 = error - PID_state.derivative;
	pidout 				+= change * PID_params.Td / PID_params.deltaT;
	PID_state.derivative = error;
	
	// OVERALL GAIN:
	pidout 				*= -PID_state.Kgain;
	return pidout;
}


/***********************************************************

 ***********************************************************/
void send_speeds(float mLpercent, float mRpercent)
{
	if (EmergencyStop==false)
	{
		pack_move_speed( &msg1, sc.get_LeftMotorInstance(),  mLpercent );
		pack_move_speed( &msg2, sc.get_RightMotorInstance(), mRpercent );
		AddToSendList( &msg1 );
		AddToSendList( &msg2 );
	}
}

void update_segway_base( )
{
	static byte call_count = 0;
	
	// GET FILTERED ANGLE:
	float raw_angle = get_raw_tilt_angle();
	float angle = median_filter( raw_angle );

	// UPDATE STATE VARIABLES:
	TiltAnglePrev = TiltAngle;
	TiltAngle = angle;

	// COMPUTE DUTY:
	byte bin     = sc.get_bin_number(angle);			// current bin
	float Lalpha = sc.get_LeftAlpha (bin);
	float Ralpha = sc.get_RightAlpha(bin); 					// alphas for current bin!
	float Lpercent = compute_speed( angle, Lalpha );
	float Rpercent = compute_speed( angle, Ralpha );

	// DISPLAY RESULTS:
	printf("==%1.3f; <%6.1f, %6.1f, %6.1f>\tAngle=%6.1f/%6.1f;", 
		   accel_time_delta, 
		   RawxyzAccel.x, RawxyzAccel.y, RawxyzAccel.z,
		   AccelAngularPosition.rx, angle 
		    );
	printf("\tDuty=<%6.3f,%6.3f> ", Lpercent, Rpercent );
	//printf("\n");

	// SEND SPEEDS TO MOTOR BOARDS:
	send_speeds( Lpercent, Rpercent );

	// ADJUST ALPHAS:
	if (call_count++ > MEDIAN_ANGLE_FILTER_SIZE)	// have to wait after start up until the median filter has good data.
	{												// otherwise, since angle was not valid, no power was given to the motors.
													// so we'll see no change in the tilt angle.
		call_count=255;					
		update_alphas( );
	} 
	printf("\n");
}

BOOL fusion_complete()
{
	update_segway_base( );
}

BOOL callback_tilt_reading( struct sCAN* mMsg )
{
	float percent ;
	float time_delta;
	BOOL retval = FALSE;
	switch (mMsg->id.group.id)
	{
	case ID_ACCEL_XYZ:
		count_accel++;
		parse_accel_msg	(mMsg);		
		process_accel	(FALSE);   //ShowAccelerometerData);
		if (EmergencyStop==false)
			update_segway_base();
		retval = TRUE;
		break;

	case ID_GYRO_XYZ : 
		count_gyro++;		
		parse_gyro_msg(mMsg); 	// puts into RawxyzGyro
		process_gyro	(FALSE); 	// ShowGyroData);		
		retval=TRUE;
		break;

/*	case ID_MAGNET_XYZ : 
		count_magnet++;
		if (ShowMagnetData && ShowCANData)		print_message(mMsg);		
		parse_magnet_msg(mMsg);
		process_magnet(ShowMagnetData);
		if (ShowMagnetData) printf("\n");		
		retval= TRUE;
		break; */
	default:
		retval= FALSE;
		break;
	}
	//if ((count_accel==1) && (count_gyro==1) && (count_magnet==1))
	if ((count_accel==1) && (count_gyro==1))
	{
		//printf("<<<=======FUSION===========>>>\n");
		//compute_accelGyro_fusion(FALSE);
		//if (tiltsensor_callback)
		//	(*tiltsensor_callback)( );

		/*if (count_samples++ < 300)
			save_timeslice_data();
		else 
			close_log_file();
		*/
		count_accel=0;
		count_gyro=0;
		count_magnet=0;
	}
	return retval;
}




