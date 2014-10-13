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

/* Time Stamp Variables & Function */
struct timeval prev_gyro_timestamp;
struct timeval latest_gyro_timestamp;

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
#define NO_SIGNIFICANT_CHANGE_ANGLE 2.0		// Degrees per 0.01 second.
#define TOO_MUCH_CHANGE_ANGLE 		6.0		// Degrees per 0.01 second.

#define MIN_ALPHA 0.0
#define MAX_ALPHA 4.0

float adjust_alpha( byte bin, float alpha )
{

	// Hopefully TiltAngle is closer to 0 than is TiltAnglePrev.
	// After all Balance is the whole goal!

	// OBSERVATION OF CHANGE:
	byte mid_index = round(MEDIAN_ANGLE_FILTER_SIZE/2.0);
	float LastAngleChange = fabs(median_angle_filter[MEDIAN_ANGLE_FILTER_SIZE-1]- 
								 median_angle_filter[mid_index]);

	// ADJUST:
	if ( LastAngleChange < NO_SIGNIFICANT_CHANGE_ANGLE )
	{
		// Don't adjust this if we are in the bin closest to zero
		if ((bin != 4) && (bin != 5))	// b/c we want small changes here!
		{
			alpha *= 1.10;	printf("Inc*");
			if (alpha>MAX_ALPHA) alpha = MAX_ALPHA;
			//skip_count = (MEDIAN_ANGLE_FILTER_SIZE - mid_index);
		}
	}
	else if (LastAngleChange > TOO_MUCH_CHANGE_ANGLE )
	{
		alpha *= 0.90;		printf("dec/");
		if (alpha<MIN_ALPHA) alpha = MIN_ALPHA;
		//skip_count = (MEDIAN_ANGLE_FILTER_SIZE - mid_index);
	}
	return alpha;
}

// want to update the alpha's for the prev bin:
void update_alphas( )
{
	static byte skip_count = 0;
	if ((skip_count--) > 0)
		return ;	

	// We adjust the alpha of the previous timeslice's bin:
	byte prev_bin = sc.get_bin_number( TiltAnglePrev );
	float alpha1  = sc.get_LeftAlpha ( prev_bin );
	float alpha2  = sc.get_RightAlpha( prev_bin );
	
	float aalpha1 = adjust_alpha( prev_bin, alpha1 );		// Based on prev and curr angles
	float aalpha2 = adjust_alpha( prev_bin, alpha2 );		// 
	if (aalpha1 != alpha1)
		skip_count = 3;			// delay 3x (slow changes)

	// WANT TO effect the alpha of the bin from TiltAnglePrev,
	// because that alpha is what was used to make the change we now see!
	printf("b=%d; adj alpha = %2.4f %2.4f \n", prev_bin, aalpha1, aalpha2 );

	sc.set_LeftAlpha ( prev_bin, aalpha1 );
	sc.set_RightAlpha( prev_bin, aalpha2 );
}

/***********************************************************
 ***********************************************************/
/* calculates the duty cycle needed */
float compute_speed( float AngularRate, float Alpha )
{
	float percent = AngularRate * Alpha * 100.0;	
	return percent;
}

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
/*	case ID_GYRO_XYZ : 
		count_gyro++;		
		parse_gyro_msg(mMsg); 	// puts into RawxyzGyro

		// TIME STAMP:
		prev_gyro_timestamp = latest_gyro_timestamp;
		gettimeofday(&latest_gyro_timestamp, NULL);
		time_delta = calc_time_delta( &prev_gyro_timestamp, &latest_gyro_timestamp );
		if (ShowGyroData)
			printf("*** GYRO TIME DELTA=%10.4f\n", time_delta );

		process_gyro	(FALSE); 	// ShowGyroData);		
		retval=TRUE;
		break;
	case ID_MAGNET_XYZ : 
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




