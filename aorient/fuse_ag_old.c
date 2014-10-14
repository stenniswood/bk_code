/*********************************************************
 Sensor Fusion - Accelerometer, Gyro, and Magnetometer.
 Accel - Good at absolute angles when stationary.
 Gyro  - 
 Magnet - Best at absolute angles (correct even if accelerating)
 
 *********************************************************/
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "mcp2515.h"
#include "pican_defines.h"
#include "CAN_Interface.h"
#include "packer_lcd.h"
#include "can_txbuff.h"
#include "packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "fuse_ag.h"
#include "fuse_accel.h"
#include "fuse_gyro.h"
#include "fuse_magnet.h"


float  LSE_Gain=99;					// Gain factor (for moving weighted average)

struct fXYZ AR_weight_accel;		// 1=> Gyro; 2=> AccelDelta;

/***********************************************************/
struct fXYZ BestAnglesPrev;				// Holds the best combined estimate of the orientation.
/*********************  FINAL RESULTS  *********************/
struct fXYZ BestAngularPosition;				// Holds the best combined estimate of the orientation.
struct fXYZ BestAngularRate;		// Holds the best estimate of rate of turn.
struct fXYZ BestAngularAccel;		// Holds the best estimate of angular acceleration
struct fXYZ BestLinearPosition;		// Holds the best estimate of rate of turn.
struct fXYZ BestLinearVelocity;		// Holds the best estimate of rate of turn.
struct fXYZ BestLinearAcceleration;	// Holds the best estimate of rate of turn.
/***********************************************************/

void fuse_init()
{
	fuse_accel_init();
	fuse_gyro_init();
	fuse_magnet_init();	
}

/**********************************************
		PROCESSING HELPER FUNCTIONS
 *********************************************/

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

"if GYRO way different from ACCEL"			*/
BOOL significant_gyro_accel_discrepancy()
{
	// if GYRO way different from ACCEL (ie. +- 3 sigma)
	struct fXYZ g_sigma3 ;
	struct fXYZ ad_sigma3;
	struct fXYZ sigma3	 ;
	struct fXYZ Difference;

	// Compute Combined 3 Sigma:		
	scale( &GyroVariance, 	    3.0,  &g_sigma3  );
	scale( &AccelDeltaVariance, 3.0,  &ad_sigma3 );
	add  ( &g_sigma3, &ad_sigma3, &sigma3 );	

	// Compute Discrepancy:
	subtract( &GyroReading, (struct fXYZ*)&AccelAngularVelocity, &Difference );
	
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

	add( &BestAnglesPrev, &BestAngularRate, &mBestUpdatedSensorAngle );
}

// Angular Rate = w1 * AccelRate + w2*GyroRate;
//
// Question is how do we find the variance when the numbers are non constant?!
//    We can measure how they co-vary with other measurements.
void compute_accelGyro_fusion( BOOL mShowDebug)
{
	BestAnglesPrev = BestAngularPosition;
	
	process_accel( mShowDebug );
	process_gyro ( mShowDebug );	
	//printf("compute_accelGyro_fusion.  ");
	
	/* Note to compare apples to apples,
		Gyro gives (degrees per second)			
		AccelAngularVelocity gives (degrees change).  
			Need to divide this by time_period 
			between accelerometer readings.
		*** Or oranges to oranges ***
			Gyro gives (degrees per second)
				Scale by time_period ==> degrees change.
			AccelAngularVelocity gives (degrees change).  
	
		which unit to work in?
			To accumulate angles, need angle change (ie 2nd orange method)
			Preferrable.
	*/

	// IF SIGNIFICANT DISCREPANCY BETWEEN GRYO and ACCEL:
	if ( significant_gyro_accel_discrepancy() )
	{
		printf(" Linear Acceleration Detected\n");
		
		// GYRO IS FAR DIFFERENT FROM ACCEL (other acceleration likely):
		// take the gyro reading (later combined with magnet)
		extract_linear_acceleration();		
	}
	else 
	{
		printf(" Deltas Weighted Sum \n");
		// ACCEL & GYRO ARE SIMILAR:
		weighted_average_comp( &GyroReading, (struct fXYZ*)&AccelAngularVelocity, 
						&AR_weight_gyro, &AR_weight_accel,  &SumDeltaGyroVariances, 
						&BestAngularRate );						
		//accumulate_gyro_angles( &BestAngularRate );
		
		//if (mShowDebug)	
		//printf("SumDeltaGyroVar:");	print_vector( &SumDeltaGyroVariances );		printf("\n");	
		//printf("Gyro:"); 		print_vector( &GyroReading );		printf("\n");
		printf("Accel Delta:"); 	print_avector( &AccelAngularVelocity );	printf("\n");
		printf("BestAngularRate:"); 	print_vector( &BestAngularRate );	printf("\n");
	}
}


/*	compute_c2();
	compute_c3();
	compute_c1_();
	compute_slope();
	compute_intercept();
	if(mShowDebug)
	{
		printf("  c1=");		print_vector( &c1 );
		printf("\nc2=");		print_vector( &c2 );
		printf("\nc3=");		print_vector( &c3 );
		printf("\nSlope=");		print_vector( &GyroSlope );
		printf("\nIntercept=");	print_vector( &GyroIntercept );		printf("\n");
	}
	GyroSlopePrev     = weighted_average( GyroSlopePrev,     GyroSlope, 	LSE_Gain, 0.10 );
	GyroInterceptPrev = weighted_average( GyroInterceptPrev, GyroIntercept, LSE_Gain, 0.10 );
	if(mShowDebug)
	{
		printf("\nAvgd Slope=");		print_vector( &GyroSlopePrev );
		printf("\nAvgd Intercept=");	print_vector( &GyroInterceptPrev );		printf("\n\n");
	} */
/* a vector constant used to find m & b for gyro 
struct fXYZ c1;
struct fXYZ c2;
struct fXYZ c3;

float compute_c1()
{
	struct fXYZ tmp;
	struct fXYZ tmp2;
	v_product( cast_f &AccelAngularVelocity,     cast_f &GyroReading,     &tmp  );
	v_product( cast_f &AccelAngularVelocityPrev, cast_f &GyroReadingPrev, &tmp2 );
	scale( cast_f &tmp2, LSE_Gain, &tmp2 );
	add( &tmp, &tmp2, &c1 );
}
float compute_c2()
{
	struct fXYZ tmp;
	scale( cast_f &AccelAngularVelocityPrev, LSE_Gain, &tmp );
	add( cast_f &AccelAngularVelocity, &tmp, &c2  );	
}

float compute_c3()
{
	struct fXYZ tmp;
	scale( cast_f &GyroReadingPrev, LSE_Gain, &tmp );	
	add( cast_f &GyroReading, &tmp, &c3  );
}
float compute_c1_()
{
	struct fXYZ tmp;
	v_product( cast_f &c2, cast_f &c3, &c1 );
}

float compute_slope()
{
	struct fXYZ tmp;
	struct fXYZ tmp2;	
	struct fXYZ numerator;
	struct fXYZ denominator;

	v_product( &c2,  &c3, 	&tmp);
	subtract ( &c1,  &tmp, 	&numerator );
	scale	 ( &c3,  2, 	&tmp );	
	v_product( &c3,  &c3, 	&tmp2);
	subtract ( &tmp, &tmp2, &denominator );
	v_divide ( &numerator, &denominator, &GyroSlope);
	
	// For TEST:
	GyroSlope.x = 1.0;
	GyroSlope.y = 1.0;
	GyroSlope.z = 1.0; 
}

float compute_intercept()
{
	struct fXYZ tmp;
	struct fXYZ numerator;	
	v_product( &c3, &GyroSlope, &tmp      	   );
	subtract ( &c2, &tmp, 		&GyroIntercept );
	scale	 ( &GyroIntercept, 0.5, &tmp );
}*/
