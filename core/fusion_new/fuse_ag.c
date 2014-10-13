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

#define cast_f (struct fXYZ *)
#define cast_fr (struct frXYZ *)

// CONFIDENCES:
float AccelerometerConfidence = 10.0;
float GyroConfidence 		  = 1.0;

struct frXYZ BestAnglesPrev;		// Holds the best combined estimate of the orientation.
struct frXYZ BestAngles;			// Holds the best combined estimate of the orientation.

struct frXYZ BestAngularRatePrev;	// Holds the best estimate of rate of turn.
struct frXYZ BestAngularRate;		// Holds the best estimate of rate of turn.

struct fXYZ  BestLinearPosition;		// Holds the best estimate of rate of turn.
struct fXYZ  BestLinearVelocity;		// Holds the best estimate of rate of turn.
struct fXYZ  BestLinearAcceleration;	// Holds the best estimate of rate of turn.

float  LSE_Gain=99;					// Gain factor (for moving weighted average)

int count_accel  = 0;
int count_gyro   = 0;
int count_magnet = 0;
int count_samples= 0;

BOOL ShowAccelerometerData 	= TRUE;
BOOL ShowGyroData 			= TRUE;
BOOL ShowMagnetData 		= TRUE;
BOOL ShowCANData 			= TRUE;
BOOL Classify 				= FALSE;

// Call back function for system msg processing 
BOOL (*tiltsensor_callback)() = NULL;	
void set_tiltsensor_callback( BOOL (*mtiltsensor_callback)() )
{
	tiltsensor_callback = mtiltsensor_callback;
}

#include "CAN_Interface.h"
#include "can_txbuff.h"

BOOL callback_tilt_reading( struct sCAN* mMsg )
{
	BOOL retval = FALSE;
	switch (mMsg->id.group.id)
	{
	case ID_ACCEL_XYZ : 
		count_accel++;
		parse_accel_msg	(mMsg);
		process_accel	(FALSE); //ShowAccelerometerData);		
		retval= TRUE;
		break;
	case ID_GYRO_XYZ : 
		count_gyro++;
		parse_gyro_msg	(mMsg); 		// puts into RawxyzGyro
		process_gyro	(FALSE); //ShowGyroData);		
		if (ShowGyroData)	
		{
			printf("\n");
			//print_vector( &RawxyzGyro );			printf("\n");
			//print_message(mMsg);					printf("\n");
		}
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
		printf("<<<=======FUSION===========>>>\n");
		compute_accelGyro_fusion(FALSE);
		(*tiltsensor_callback)( );
		
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

void fuse_init()
{
	fuse_accel_init();
	fuse_gyro_init();
	fuse_magnet_init();	
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
(Not the angles level)
*/
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

/* Time Stamp Variables & Function */
struct timeval start, end;
float  fusion_time_delta_prev = 0;
float  fusion_time_delta 	  = 0.1;
void calc_fusion_time_delta( BOOL mShowResult )				/* Output:  gyro_time_delta */
{
	// GET TIME:
	gettimeofday(&end, NULL);
	// SUBTRACT PREVIOUS TIME:
	long seconds  = end.tv_sec  - start.tv_sec;
	long useconds = end.tv_usec - start.tv_usec;    
	fusion_time_delta = seconds + ((float)useconds/1000000.0);
	// GET READY FOR NEXT CALL:
	start = end;
	if (mShowResult)
		printf("*** GYRO DELTA TIME=%10.4f\n", fusion_time_delta );
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
	printf("===============================================================================\n");
	print_best_angles_prev();
	print_best_angular_velocity();
	print_best_angles();
	printf("\n");
	print_accel_angles();
	print_gyro_result();
	print_accel_velocity();	
}

// Angular Rate = w1 * AccelRate + w2*GyroRate;
//
// Question is how do we find the variance when the numbers are non constant?!
//    We can measure how they co-vary with other measurements.
void compute_accelGyro_fusion( BOOL mShowDebug )
{ 	
	calc_fusion_time_delta(FALSE);
	
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
	
	//char LinearAccelerationDetected = compare_to_1g(&AccelScaled, 3.0*Gravity_variance);
	if (LinearAccelerationDetected)
	{
		//calc_stationary_angles( );  already done.
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
 
 
 
	//
	// IF SIGNIFICANT DISCREPANCY BETWEEN GRYO and ACCEL:
/*	if ( significant_gyro_accel_discrepancy() )
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
						&AR_weight_gyro, &AR_weight_accel,  &SumGyroDeviations, 
						&BestAngularRate );						
		//accumulate_gyro_angles( &BestAngularRate );		
		//if (mShowDebug)	
		//printf("SumDeltaGyroVar:");	print_vector( &SumDeltaGyroVariances );		printf("\n");	
		//printf("Gyro:"); 		print_vector( &GyroReading );		printf("\n");
		printf("Accel Delta:"); 	print_avector( &AccelAngularVelocity );	printf("\n");
		printf("BestAngularRate:"); 	print_vector( &BestAngularRate );	printf("\n");
	}
*/ 
 
 
 
 
 
 
/*********************************************************************
 ******************   NO LONGER USED FUNCTIONS       *****************
 *********************************************************************/
/* a vector constant used to find m & b for gyro 
struct fXYZ c1;
struct fXYZ c2;
struct fXYZ c3;

float compute_c1()
{
	struct fXYZ tmp;
	struct fXYZ tmp2;
	v_product( cast_f &AccelAnglesDelta,     cast_f &GyroReading,     &tmp  );
	v_product( cast_f &AccelAnglesDeltaPrev, cast_f &GyroReadingPrev, &tmp2 );
	scale( cast_f &tmp2, LSE_Gain, &tmp2 );
	add( &tmp, &tmp2, &c1 );
}
float compute_c2()
{
	struct fXYZ tmp;
	scale( cast_f &AccelAnglesDeltaPrev, LSE_Gain, &tmp );
	add( cast_f &AccelAnglesDelta, &tmp, &c2  );	
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
} 

void compute_accelGyro_fusion(BOOL mShowDebug)
{
		compute_c1_();
		compute_c2();
		compute_c3();
		compute_slope();
		compute_intercept();
		printf("c1=");
		print_vector( &c1 );
		printf("\nc2=");
		print_vector( &c2 );
		printf("\nc3=");
		print_vector( &c3 );

		printf("\nSlope=");
		print_vector( &GyroSlope );
		printf("\nIntercept=");
		print_vector( &GyroIntercept );
		printf("\n");

		GyroSlopePrev     = weighted_average( GyroSlopePrev,     GyroSlope, 	LSE_Gain, 0.10 );
		GyroInterceptPrev = weighted_average( GyroInterceptPrev, GyroIntercept, LSE_Gain, 0.10 );		
		printf("\nAvgd Slope=");
		print_vector( &GyroSlopePrev );
		printf("\nAvgd Intercept=");
		print_vector( &GyroInterceptPrev );
		printf("\n\n");
}  */
