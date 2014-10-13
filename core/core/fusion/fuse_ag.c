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
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"


#define cast_f (struct fXYZ *)

// First Cut from sensors:
struct frXYZ AccelAnglesDelta;		// Hold the change in angles (compare to gyro)
struct frXYZ AccelAnglesDeltaPrev;	// Hold previous delta for linefit
struct frXYZ AccelAngles;		// Calc'd from aSin when stationary
struct frXYZ AccelAnglesPrev;	// Hold previous result for substract to get deltas.

//struct fXYZ GyroReading;		// Integration
struct fXYZ GyroReadingPrev;	// prev sensor reading (for linefit)

struct frXYZ GyroAngles;		// Integration
struct frXYZ GyroAnglesPrev;	// prev sensor reading (for linefit)

struct frXYZ MagnetAngles;		// Sin/Cos when stationary
struct frXYZ MagnetAnglesPrev;	// Sin/Cos when stationary

struct fXYZ GyroSlopePrev;		// Linear transform done to the gyro readings. This holds the least squares fit.
struct fXYZ GyroInterceptPrev;	// Linear transform done to the gyro readings. This holds the slope fit.

struct fXYZ GyroSlope;		// Linear transform done to the gyro readings. This holds the least squares fit.
struct fXYZ GyroIntercept;	// Linear transform done to the gyro readings. This holds the slope fit.
float  LSE_Gain=99;		// Gain factor (for moving weighted average)

/**********************************************
		PROCESSING HELPER FUNCTIONS
 *********************************************/
/* Note:  Accelerometer is more accurate at small angles.
	When close to 90 degrees, a change of 0.01 can cause 5 degrees 
	difference.  Compared to ~ -+1 degree at 12deg.
	This is direct result of the asin function.
	
*/
/* Triangulation of readings with gravity (vertical down) */
void accel_stationary_angles(struct fXYZ* mRaw )
{
	AccelAngles.rx = asin( mRaw->x );
	AccelAngles.ry = asin( mRaw->y );
	AccelAngles.rz = asin( mRaw->z );
	radians_to_degrees( &AccelAngles, &AccelAngles);	
}

/* Steady accumulation (integration) of the angle deltas */
void gyro_angles(struct fXYZ* mRaw )
{
	GyroAngles.rx += mRaw->x;
	GyroAngles.ry += mRaw->y;
	GyroAngles.rz += mRaw->z;	
}

/* Need to establish Reference Vector when level &
	pointing due North.
   Reading taken when orientation is 0 for all rotations
*/
struct fXYZ magnetReference;
void magnet_angles( struct fXYZ* mRaw )
{
	// Similar to accel_
	// Normalize the Raw Magnet vector
	//  Compared to 1 earth magnet field strength
	struct fXYZ tmp;
	tmp.x = 1; tmp.y = 0; tmp.z = 0;
	MagnetAngles.rx  = angle_between( mRaw, &tmp );
	tmp.x = 0; tmp.y = 1; tmp.z = 0;
	MagnetAngles.ry  = angle_between( mRaw, &tmp );
	tmp.x = 0; tmp.y = 0; tmp.z = 1;
	MagnetAngles.rz  = angle_between( mRaw, &tmp );
}


/*********************************************************************
 ******************   PRECONDITIONING  FUNCTIONS     *****************
 *********************************************************************/
BOOL  NoLinearAcceleration=FALSE;

/* Processes raw data from the CAN message */ 
void process_accel( BOOL ShowData )
{
	// If near 1g, then stationary or rotational only!
	// (yes there is a loophole here:  freefall & 1g horizontal to name 1)
	// (gyro can verify this however.  If not validated, then...)
	float mag = magnitude(&RawxyzAccel);
	if ((mag>0.9) && (mag<1.1))
		NoLinearAcceleration = TRUE;
	else 
		NoLinearAcceleration = FALSE;

	// Update Time slice:		
	AccelAnglesPrev      = AccelAngles;			
	AccelAnglesDeltaPrev = AccelAnglesDelta;
	
	// Compute new Angles/Deltas.
	accel_stationary_angles( cast_f &RawxyzAccel );	// updates "AccelAngles"
	subtract( cast_f &AccelAngles, cast_f &AccelAnglesPrev, cast_f &AccelAnglesDelta );
	if (ShowData)
	{
		printf("ACCEL\t:");
		print_avector(  &AccelAngles );
		printf("\t Delta\t:");
		print_avector(  &AccelAnglesDelta );
	}
}

/* Processes raw data from the CAN message */
void process_gyro( BOOL ShowData )
{
	GyroReadingPrev = GyroReading;
	gyro_angles ( &GyroReading );
	if (ShowData)
	{
		printf("ACCUM\t:");
		print_avector( &GyroAngles );
	}
}

/* Processes raw data from the CAN message */ 
void process_magnet( BOOL ShowData )
{
	magnet_angles( &RawxyzMagnet );
	if (ShowData)
	{
		printf("MAGNET\t:");
		print_vector ( &RawxyzMagnet );
		printf("\t:");
		print_avector( &MagnetAngles );
	}
}

/*********************************************************************
 ****************** TOP LEVEL PROCESSING  FUNCTIONS  *****************
 *********************************************************************/

/* a vector constant used to find m & b for gyro */
struct fXYZ c1;
struct fXYZ c2;
struct fXYZ c3;

/*float compute_c1()
{
	struct fXYZ tmp;
	struct fXYZ tmp2;
	v_product( cast_f &AccelAnglesDelta,     cast_f &GyroReading,     &tmp  );
	v_product( cast_f &AccelAnglesDeltaPrev, cast_f &GyroReadingPrev, &tmp2 );
	scale( cast_f &tmp2, LSE_Gain, &tmp2 );
	add( &tmp, &tmp2, &c1 );
}*/
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

void compute_accelGyro_fusion()
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
}
