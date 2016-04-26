/***************************************************************
 Sensor Fusion - Accelerometer, Gyro, and Magnetometer.
 This file puts together the various calculations made in each 
 sensor file.
 ***************************************************************/
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "bk_system_defs.h"
#include "mcp2515.h"
#include "pican_defines.h"
#include "CAN_base.h"
#include "CAN_Interface.h"
#include "packer_lcd.h"
#include "packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "can_id_list.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "fuse_accel.h"
#include "fuse_gyro.h"
#include "fuse_magnet.h"
#include "fuse_ag.h"
#include "fuse_log.h"
#include "CAN_Interface.h"
#include "can_txbuff.h"
#include "callbacks.hpp"

#define cast_f  (struct fXYZ  *)
#define cast_fr (struct frXYZ *)

// CONFIDENCES:
float 		 AccelerometerConfidence  = 10.0;
float 		 GyroConfidence 		  =  1.0;

struct frXYZ BestAnglesPrev;			// Holds the best combined estimate of the orientation.
struct frXYZ BestAngles;				// Holds the best combined estimate of the orientation.

struct frXYZ BestAngularRatePrev;		// Holds the best estimate of rate of turn.
struct frXYZ BestAngularRate;			// Holds the best estimate of rate of turn.

struct fXYZ  BestLinearPosition;		// Holds the best estimate of rate of turn.
struct fXYZ  BestLinearVelocity;		// Holds the best estimate of rate of turn.
struct fXYZ  BestLinearAcceleration;	// Holds the best estimate of rate of turn.

float  LSE_Gain=99;						// Gain factor (for moving weighted average)

int count_accel  = 0;
int count_gyro   = 0;
int count_magnet = 0;

BOOL ShowAccelerometerData 	= FALSE;
BOOL ShowGyroData 			= FALSE;
BOOL ShowMagnetData 		= FALSE;
BOOL ShowCANData 			= TRUE;
BOOL Classify 				= FALSE;

struct timeval fusion_start, fusion_end;
float  fusion_time_delta 	  = 0.1;



/* Output:  gyro_time_delta */
float calc_time_delta( struct timeval* mstart, struct timeval* mend )
{
	// SUBTRACT PREVIOUS TIME:
	long seconds  = mend->tv_sec  - mstart->tv_sec;
	long useconds = mend->tv_usec - mstart->tv_usec;
	float time_delta = seconds + ((float)useconds/1000000.0);
	return time_delta;
}

void fuse_init()
{
	fuse_accel_init();
	fuse_gyro_init();
	fuse_magnet_init();	
}


/* PROCESS FLOW :
		avisual_main.cpp		Sets up callbacks to this function.
		callbacks.cpp			This function.
		parser_tilt.c			Extracts raw xyz (parse_accel_msg, parse_gyro_msg, and parse_magnet_msg )
		fuse_accel.c			Processes the accelerometer independantly.
		fuse_gyro.c				Processes the gyro independantly.

*/
BOOL callback_tilt_reading( struct sCAN* mMsg )
{
	byte 	result;
	byte 	junk_count=0;

	BOOL retval = FALSE;
	switch (mMsg->id.group.id)
	{
	case ID_ACCEL_XYZ : 
		count_accel++;
		parse_accel_msg	(mMsg);		
		if (ShowAccelerometerData)				
			print_raw( mMsg, &RawxyzAccel, ShowCANData );
			
		process_accel	(ShowAccelerometerData);
		if (ShowAccelerometerData) 	printf("\n");

		tilt_sensor_update_gui();
		
		// For Histogram reporting:		
		/*sf1.set_ab( AccelAngularPosition.rx );
		ds_tiltx.shift_add( AccelAngularPosition.rx );
		ds_tilty.shift_add( AccelAngularPosition.ry );		*/
		retval= TRUE;
		break;
	case ID_GYRO_XYZ : 
		count_gyro++;
		parse_gyro_msg	(mMsg); 
		if (ShowGyroData)	
			print_raw(mMsg, &RawxyzGyro, ShowCANData);
		process_gyro	(ShowGyroData);		
		if (ShowGyroData) 							printf("\n");
		retval=TRUE;
		break;

	case ID_MAGNET_XYZ : 
		count_magnet++;
		parse_magnet_msg(mMsg);
		if (ShowMagnetData)	
			print_raw(mMsg, &RawxyzMagnet, ShowCANData);
		process_magnet( ShowMagnetData );
		if (ShowMagnetData) 						printf("\n");
		retval= TRUE;
		break; 	
	default:
		retval= FALSE;
		break;	
	}

	return retval;
}



/**********************************************
		PROCESSING HELPER FUNCTIONS
 *********************************************/
/* Note:  Accelerometer is more accurate at small angles.
	When close to 90 degrees, a change of 0.01 can cause 5 degrees 
	difference.  Compared to ~ -+1 degree at 12deg.
	This is direct result of the asin function.
*/

/*********************************************************************
 ******************   PRECONDITIONING  FUNCTIONS     *****************
 *********************************************************************/

/*********************************************************************
 ****************** TOP LEVEL PROCESSING  FUNCTIONS  *****************
 *********************************************************************/
/* Sigma (stdev) probabilities:
1σ 	68.2689492% 	31.7310508% 			1 / 3.1514872
2σ 	95.4499736% 	4.5500264% 				1 / 21.977895
3σ 	99.7300204% 	0.2699796% 				1 / 370.398
4σ 	99.993666% 		0.006334% 				1 / 15787
5σ 	99.9999426697% 	0.0000573303% 			1 / 1744278
6σ 	99.9999998027% 	0.0000001973% 			1 / 506797346
7σ 	99.9999999997440% 	0.000000000256% 	1 / 390682215445

"if GYRO way different from ACCEL"			

Compares the Gyro reading to the computed Accelerometer Deltas
In other words:  at the angular rate level
(Not the angles level)		*/
BOOL significant_gyro_accel_discrepancy()
{
	// if GYRO way different from ACCEL (ie. +- 3 sigma)
	struct fXYZ g_sigma3 ;
	struct fXYZ ad_sigma3;
	struct fXYZ sigma3	 ;
	struct fXYZ Difference;

	// Compute Discrepancy:
	subtract( cast_f &GyroAdjusted, cast_f &AccelAngularVelocity, cast_f &Difference );

	// Compute Combined 3 Sigma:		
	scale( cast_f &GyroVariance, 	    3.0, cast_f &g_sigma3  );
	scale( cast_f &AccelDeltaVariance,  3.0, cast_f &ad_sigma3 );
	add  ( cast_f &g_sigma3, 	cast_f &ad_sigma3,  cast_f &sigma3 );	
	
	// if any 1 axis is > 3sigma, then significant
	if ( (Difference.x > sigma3.x) || 
		 (Difference.y > sigma3.y) || 
		 (Difference.z > sigma3.z) ) 
		 return TRUE;	
	else 
		 return FALSE;	
}

void extract_linear_acceleration()
{
	// We assume new Accelerometer Angles are updated
	//		Angular Angles Prev + BestAngularRate
	// We know how much gravity is
	// Anything else is linear acceleration
	
	// We use the more accurate updated angular position.
	//scale (BestAngularRate, time_period, &m
	struct fXYZ mBestUpdatedSensorAngle;
	add( cast_f &BestAnglesPrev,  cast_f &BestAngularRate,  cast_f &mBestUpdatedSensorAngle );
}

void print_best_angles_prev()
{
	printf("Best Angles Prev (degrees):\t\t\t\trotate about xyz = <%6.2f, %6.2f, %6.2f>\n",
		BestAnglesPrev.rx, BestAnglesPrev.ry, BestAnglesPrev.rz );	
}
void print_best_angles()
{
	printf("Best Angles\t(degrees): \t\t\t\trotate about xyz = <%6.2f, %6.2f, %6.2f>\n",
		BestAngles.rx, BestAngles.ry, BestAngles.rz );	
}

void print_best_angular_velocity()
{
	printf("Best AngularRate (degrees/sec):\t\t\t\t  Rate about xyz = <%6.2f, %6.2f, %6.2f>\n",
		BestAngularRate.rx, BestAngularRate.ry, BestAngularRate.rz );
}

void print_params()
{
	printf("========================================================================\n");
	print_best_angles_prev();
	print_best_angles();
	print_accel_angles_prev();
	print_accel_angles();	
	printf("\n");
	print_best_angular_velocity();	
	print_accel_velocity();	
	print_gyro_result();
}

// Angular Rate = w1 * AccelRate + w2*GyroRate;
//
// Question is how do we find the variance when the numbers are non constant?!
//    We can measure how they co-vary with other measurements.
void compute_accelGyro_fusion( BOOL mShowDebug )
{
	// GET TIME:
	fusion_start = fusion_end;
	gettimeofday( &fusion_end, NULL );
	fusion_time_delta = calc_time_delta( &fusion_start, &fusion_end );
	if (mShowDebug)
		printf("*** FUSION TIME DELTA=%10.4f\n", fusion_time_delta );

	BestAnglesPrev      = BestAngles;
	BestAngularRatePrev = BestAngularRate;

	//printf("compute_accelGyro_fusion.  ");	
	/* Note to compare apples to apples,  
	   Gyro and Accel need to both give degrees per second!
	
		Gyro gives (degrees per second)
		AccelAngularVelocity gives (degrees per second)

		*** Not oranges to oranges! ***
			Gyro gives (degrees change) & AccelAngularVelocity gives (degrees change)
			But time period may be different for each.	
	*/
	
	// IF ACCEL IS_WITHIN_1G() AND GYRO NEAR 0
	//		Safe to extract angles based on accelerometer.
	// 			Don't just average this angle in with previous+delta.  Instead it sets the standard, 
	//			the best result can be within a range +- from this.
	//	IF ACCEL IS_WITHIN_1G() AND Gyro shows movement
	//		Safe to extract angles based on accelerometer.
	//			Average the delta with Gyro
	//		BestAngle will be PrevAngles + averaged deltas.
	//	IF ACCEL IS NOT WITHIN_1G() 
	//		BestAngle will be PrevAngles + gyro deltas.
	//		Extract linear acceleration	
			
	if (LinearAccelerationDetected)		// determined in fuse_accel.h
	{
		BestAngularRate = GyroAdjusted;
		extract_linear_acceleration(); 
		
	} else if ( significant_gyro_accel_discrepancy() )
	{
		float wG = 2.0;
		float wA = 1.0;
		printf("Significant Discrepancy between Accel & Gyro\n");
		// more weight given to gyro for angular changes.
		struct fXYZ average_rate = weighted_average( 
								  cast_f &GyroAdjusted, 
								  cast_f &AccelAngularVelocity, 
								  wG,  wA );
		BestAngularRate = *(cast_fr &average_rate);		
	} else {
		float wG = 1.0;
		float wA = 10.0;
		// DELTA denotes a change from 1 time period to the next.
		// Rate  denotes a change over 1 second.
		struct fXYZ average_rate = weighted_average( 
								  cast_f &GyroAdjusted, 
								  cast_f &AccelAngularVelocity, 
								   wG,  wA );
		BestAngularRate = *(cast_fr &average_rate);
	}
	
	// SCALE ANGULAR RATE BY Time to get DELTA ANGLE (for the timeslice):
	struct frXYZ BestAngularDelta;
	scale( cast_f &BestAngularRate, fusion_time_delta, cast_f &BestAngularDelta );
	add  ( cast_f &BestAnglesPrev, cast_f &BestAngularDelta, cast_f &BestAngles );

	// 
	print_params();	
} 
 
 
 
/*********************************************************************
 ******************   NO LONGER USED FUNCTIONS       *****************
 *********************************************************************/
