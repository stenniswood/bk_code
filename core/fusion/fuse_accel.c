/***************************************************************
 Sensor Fusion - Accelerometer, Gyro, and Magnetometer.
 Accel  - Good at absolute angles when stationary.
 Gyro   - Impervious to linear movements (delta of angles)
 Magnet - Best at absolute angles (correct even if accelerating)
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
#include "packer_lcd.h"
#include "packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "fuse_ag.h"
#include "fuse_accel.h"


#define One_g_scale2g  0x3FFF

BOOL   LinearAccelerationDetected=FALSE;

// See ../core/msg/parser_tilt.c   for RawxyzAccel and routine for extracting from CAN
struct fXYZ AccelScaled;

// First Cut from Accel sensor:
struct frXYZ AccelAngularPosition;		// Calc'd from aSin when stationary
struct frXYZ AccelAngularPositionPrev;	// Hold previous result for substract to get deltas.
struct frXYZ AccelAngularVelocity;		// Hold the change in angles (compare to gyro)
struct frXYZ AccelAngularVelocityPrev;	// Hold previous delta for linefit

// VARIANCES (FROM PREVIOUS MEASURED RESULTS .xls sheet)
struct fXYZ AccelRawVariance;			// Holds the variance as measured during trial stationary data collection.
struct fXYZ AccelAngleVariance;			// Holds the variance as measured during trial stationary data collection.
struct fXYZ AccelDeltaVariance;			// Holds the variance as measured during trial stationary data collection.
struct fXYZ AR_weight_accel; 			// Angular Rate Weights 

// Time:
//struct timeval  accel_start, accel_end;
float accel_time_delta_prev = 0;
float accel_time_delta 	    = 0.1;

struct timeval prev_accel_timestamp;
struct timeval latest_accel_timestamp;
//static float   time_delta;


void fuse_accel_init()
{
	AccelRawVariance.x = Accel_variance_raw_x;
	AccelRawVariance.y = Accel_variance_raw_y;
	AccelRawVariance.z = Accel_variance_raw_z;
	
	AccelAngleVariance.x = Accel_variance_angle_x;
	AccelAngleVariance.y = Accel_variance_angle_y;
	AccelAngleVariance.z = Accel_variance_angle_z;
	
	AccelDeltaVariance.x = Accel_variance_angle_delta_x;
	AccelDeltaVariance.y = Accel_variance_angle_delta_y;
	AccelDeltaVariance.z = Accel_variance_angle_delta_z;

	v_reciprocal( &AccelDeltaVariance,   &AR_weight_accel );
}

/* Note:  Accelerometer is more accurate at small angles.
	When close to 90 degrees, a change of 0.01 can cause 5 degrees 
	difference.  Compared to ~ -+1 degree at 12deg.
	This is direct result of the asin function.

	Triangulation of readings with gravity (vertical down)
INPUT :  Scaled 
OUTPUT:  AccelAngularPosition 
*/
void calc_stationary_angles( struct fXYZ* mRaw )
{
	/* We'll use .rx as Roll and .ry as pitch */

/* To find which quadrants - always use z sign +-.
	However to get the angle, instead of x & z.  Which works as long as y=0;
	we should work with the vector sum of y+z:   x & <y+z> 
	use atan2( x, y+z )     ???
*/
	// GET ELEVATION (PITCH):   yz projection
	AccelAngularPosition.rx = atan2( mRaw->x,  mRaw->z );	
	// GET ROLL:				xz projection
	AccelAngularPosition.ry = atan2( mRaw->y, mRaw->z );

	/* Accel cannot compute the Yaw!  This only thru the gyro & magnet
	   Z axis is only good for 
		a) Linear accel in Z (quad copter)
		b) if sensor is tipped so Z is near horizon.  
		   ( or even 45 degrees, it would supplement the Y or X axis )		     
	*/
	radians_to_degrees( &AccelAngularPosition, &AccelAngularPosition );
}

/*********************************************************************
 ******************   PRECONDITIONING  FUNCTIONS     *****************
 *********************************************************************/
char compare_to_1g( struct fXYZ* mVector, float mMagnitudeThreshold )
{
	// Compare the magnitude of the vector to 1g : 
	float netforce = magnitude( mVector );
	/*	another way: float discrepancy = (mag - One_g_scale2g) / One_g_scale2g;	*/
	return compare_with_tolerance( netforce, 1.0, mMagnitudeThreshold );
}

char compare_to_1g_show(struct fXYZ* mVector, float mThreshold)
{	
	// Compare the magnitude of the vector to 1g 
	float netforce = magnitude( mVector );
	/*	another way: float discrepancy = (mag - One_g_scale2g) / One_g_scale2g;	*/
	char result = compare_with_tolerance( netforce, 1.0, mThreshold );
	printf( "Compared to 1g: %d Magnitude:%6.3f == 1+-%6.3f\t", result, netforce, mThreshold );
	return result;
}
BOOL less_than_1g(struct fXYZ* Vector, float Threshold)		{	return (compare_to_1g(Vector,Threshold ) == -1);	}
BOOL more_than_1g(struct fXYZ* Vector, float Threshold)		{	return (compare_to_1g(Vector,Threshold ) == 1);		}

void print_accel_angles_prev()
{
	printf("Accel static Angles (degrees):\t\t\t\trotate about xyz = <%6.2f, %6.2f, %6.2f>\n",
		AccelAngularPositionPrev.rx, AccelAngularPositionPrev.ry, AccelAngularPositionPrev.rz );
}
void print_accel_angles()
{
	printf("Accel static Angles (degrees):\t\t\t\trotate about xyz = <%6.2f, %6.2f, %6.2f>\n",
		AccelAngularPosition.rx, AccelAngularPosition.ry, AccelAngularPosition.rz );
}

void print_accel_velocity()
{
	printf("Accel AngularRate [t=%6.3f](degrees/sec):\t\t  Rate about xyz = <%6.2f, %6.2f, %6.2f>\n",
		accel_time_delta, AccelAngularVelocity.rx, AccelAngularVelocity.ry, AccelAngularVelocity.rz );
}

/*	Subtract and divide by time elapsed. (Degrees per second)
INPUT :  AccelAngularPosition & AccelAngularPositionPrev
OUTPUT:  AccelAngularVelocity
*/
void compute_accel_angular_velocity()
{
	struct fXYZ tmpDelta;
	// was AccelAngularPositionPrev!
	subtract( cast_f &AccelAngularPosition, cast_f &AccelAngularPositionPrev, cast_f &tmpDelta );

	// Divide the time to get degrees/second (dps)
	float freq = 1.0/accel_time_delta;
	//float freq = 1.0;		// will show the raw deltas
	scale( &tmpDelta, freq, cast_f &AccelAngularVelocity );
}


/* 
NAME:	process_accel()
INPUT:  RawxyzAccel - Data from CAN msg which has been put into the float of the fXYZ structure
OUTPUT: Result stored in file scope variable: AccelAngularPosition (Pitch,Roll) & AccelAngularVelocity
DESCRIPTION: Calculates the dot product of the incoming measurement 
		against the reference vector for each of the x=0, y=0, and
		z=0 planes.
*/
void process_accel( BOOL ShowData )
{
	// TIME STAMP : 
	prev_accel_timestamp = latest_accel_timestamp;
	gettimeofday( &latest_accel_timestamp, NULL	);
	accel_time_delta = calc_time_delta( &prev_accel_timestamp, &latest_accel_timestamp );	
	if (ShowData)
		printf("*** ACCEL TIME DELTA=%10.4f\n", accel_time_delta );

	// Update State Variables:
	AccelAngularPositionPrev    = AccelAngularPosition;			
	AccelAngularVelocityPrev 	= AccelAngularVelocity;

	// If near 1g, then stationary or rotational only!
	// (yes there is a loophole here:  freefall & 1g horizontal to name 1)
	// (gyro can verify this however.  If not validated, then...)	
	float mult = 1.0/One_g_scale2g;				// printf("mult=%6.6f\n", mult);
	scale( &RawxyzAccel, mult, &AccelScaled );
	float tolerance = 6.0*Gravity_variance;		// defined in fuse_ag.h from observed values.
	
	if (compare_to_1g(&AccelScaled, tolerance) !=0 )
	{
		if (ShowData)  printf("Linear Acceleration detected!\n");
	 	LinearAccelerationDetected = TRUE;	
	}
	else 
	{
		LinearAccelerationDetected = FALSE;	
	}
	if (ShowData)  printf("\n");

	// Compute new Angles/Deltas:
	calc_stationary_angles( cast_f &AccelScaled );		// updates "AccelAngularPosition"
	compute_accel_angular_velocity();					// updates "AccelAngularVelocity"

	if (ShowData)
	{
		print_accel_angles();
		print_accel_velocity();
	}
}

/*
The angular rate is way off when held still.  The small noise in the sensor is multplied
10x, when dividing by the time period.  

This is probably not a problem when the sensor is moving since the delta will be out 
of the noise.  
*/

/*********************************************************************
 ******************   	END OF  FUNCTIONS     ************************
 *********************************************************************/
/*float calc_angles_1_plane_old_way(struct fXYZ* mRaw )
{
	float hypotenus = 1.0; 
	float tmp_x = asin( mRaw->x / hypotenus );
	float tmp_z = acos( mRaw->z / hypotenus );
	float retval;
	if (1)
	{
		// We are in Quadrant I or II				
		if (mRaw->x > 0)	// Quadrant I  (0 to 90deg)
		{
			* Now to improve accuracy, we use the axis which is closest to 0
			   Small angles give better measurements. *
			if (tmp_x > (M_PI/4.0))		// greater than 45 degas
				retval = tmp_x;	* no change * 		  
			else
				retval = tmp_x;	* no change * 		  
		}
		else 				// Quadrant II (0 to -90deg)
		{
			if (tmp_x > (-M_PI/4.0))		// greater than 45 degas
				retval = 2*M_PI + tmp_x;	* no change *
			else
				retval = 2*M_PI + tmp_x;	// was	 2*M_PI - tmp_z;
		}
		// We want to give it as [0..360]
	}
	else
	{
		// We are in Quadrant III or IV		
		if (mRaw->x > 0)	// Quadrant IV
		{					// asin gives (90 to 0 deg)
			if (tmp_x > (M_PI/4.0))		// greater than 45 deg
			retval =  tmp_x;
			else 		
			retval = M_PI - tmp_x;
		}
		else 				// Quadrant III
		{					// asin gives (0 to -90 deg)
			if (tmp_x > (-M_PI/4.0))		// greater than 45 deg
			retval = M_PI - tmp_x;
			else 			
			retval = 2*M_PI + tmp_x;
		}
	}
	return retval;
}*/
/* Quadrants:
	 IV	|   I
   -----|-------
	III	|   II
*/
/*float calc_angles_alpha_beta_gamma_way(struct fXYZ* mRaw )
{
	float alpha1= acos ( -mRaw->y / sqrt(1 - mRaw->z * mRaw->z) );
	float alpha2= atan2( mRaw->x, -mRaw->y );
	float beta  = acos ( mRaw->z );
	float gamma = atan2( mRaw->x, mRaw->y );
	alpha1 = rad_to_deg(alpha1);
	alpha2 = rad_to_deg(alpha2);
	beta   = rad_to_deg(beta);
	gamma  = rad_to_deg(gamma);	
	printf("\t\t\t\t\t\talpha1,2 = %6.3f,%6.3f; beta=%6.3f;  gamma=%6.3f \n",
		   alpha1, alpha2, beta, gamma);
}*/

/* Triangulation of readings with gravity (vertical down) */
/*void accel_stationary_angles(struct fXYZ* mRaw )
{
	AccelAngles.rx = asin( mRaw->x );
	AccelAngles.ry = asin( mRaw->y );
	AccelAngles.rz = asin( mRaw->z );
	radians_to_degrees( &AccelAngles, &AccelAngles);	
}*/
