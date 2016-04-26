/***************************************************************
 Sensor Fusion - Accelerometer, Gyro, and Magnetometer.
 Accel - Good at absolute angles when stationary.
 Gyro  - 
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
#include "vector_math.h"
#include "packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "parser_tilt.h"
#include "fuse_ag.h"
#include "fuse_gyro.h"

#define DEBUG 0

// See ../core/msg/parser_tilt.c   for RawxyzGyro and routine for extracting from CAN

struct frXYZ  GyroAdjustedPrev;		// prev sensor reading (for linefit)
struct frXYZ  GyroAdjusted;			// dps (after zero offset)

struct frXYZ GyroAnglesPrev;		// prev sensor reading (for linefit)
struct frXYZ	 GyroAngles;			// Integration

struct frXYZ  GyroReading;			// dps
struct frXYZ  GyroAccumulated;		// dps for ~1 second (after zero offset)
struct fXYZ  GyroOffsetDelta;		// dps for ~1 second scaled by number of accumulations.
struct fXYZ  GyroOffset_A;			// Holds the offset as measured during trial stationary data collection.
struct fXYZ  GyroOffset;			// Holds the moving average offset.

struct fXYZ	 KnownValue;			// For finding variance.
struct fXYZ	 GyroDeviation;			// Difference of just 1 measurement.
struct fXYZ	 GyroVariance;			// Holds the variance as measured during trial stationary data collection.
struct fXYZ	 SumGyroDeviations;
struct fXYZ  AR_weight_gyro; 		// Angular Rate Weights 

/* Steady accumulation (integration) of the angle deltas */
int NumberOffsetAccumulations =0;
int NumberReadingAccumulated  =0;
int NumberVariancesAccumulated=0;

BOOL sensor_not_moving = TRUE;
#define ACCUMULATOINS_PER_PERIOD 15

/* Time Stamp Variables & Function */
// struct timeval  start, end; 
float  gyro_time_delta_prev = 0;
float  gyro_time_delta 	    = 0.1;

/* Time Stamp Variables & Function */
struct timeval prev_gyro_timestamp;
struct timeval latest_gyro_timestamp;

/*void calc_gyro_time_delta( BOOL mShowResult )	 * Output:  gyro_time_delta *
{
	// GET TIME:
	//gettimeofday(&end, NULL);
	// SUBTRACT PREVIOUS TIME:
	long seconds  = end.tv_sec  - start.tv_sec;
	long useconds = end.tv_usec - start.tv_usec;    
	gyro_time_delta = seconds + ((float)useconds/1000000.0);
	// GET READY FOR NEXT CALL:
	start = end;
	if (mShowResult)
		printf("*** GYRO DELTA TIME=%10.4f\n", gyro_time_delta);
}*/

void fuse_gyro_init() 
{
	GyroOffset.x = 17.2082; 	// calibration value :  Gyro_offset_x
	GyroOffset.y = 57.9942;		// Gyro_offset_y
	GyroOffset.z = -88.5690;	// Gyro_offset_z
	
	GyroVariance.x = Gyro_variance_x;
	GyroVariance.y = Gyro_variance_y;
	GyroVariance.z = Gyro_variance_z;

	GyroAccumulated.rx = 0;
	GyroAccumulated.ry = 0;
	GyroAccumulated.rz = 0;
	
	v_reciprocal( &GyroVariance, &AR_weight_gyro );
}

void accumulate(struct fXYZ* mRaw, struct fXYZ* mAccumd, int *mNumberOfTimes )
{
	add( mRaw, mAccumd, mAccumd );
	(*mNumberOfTimes)++;
}

/* Uses a moving average.  Compare error (compared to zero)		*/
void adjust_zero_offset()
{
	printf("\n******** NumberOfAccumulations=%d\n", NumberOffsetAccumulations	); 
	printf("\nAccumulated  (dps): ");		print_avector( &GyroAccumulated  );

	scale (cast_f &GyroAccumulated, 1.0/NumberOffsetAccumulations, cast_f &GyroOffsetDelta );
	add   (cast_f &GyroOffsetDelta, cast_f &GyroOffset, cast_f &GyroOffset_A );
	// GyroOffset_A is the offset that would have produced zero net angular change during the period.
	
	// We add this into the moving average of the offset:
	float alpha = 0.2;
	mix( &GyroOffset_A, &GyroOffset, 0.2, &GyroOffset);

	printf("\nNew Gyro Offset  (dps): ");	print_vector( &GyroOffset );
	NumberOffsetAccumulations=0;
	zero( cast_f &GyroAccumulated );
}

BOOL gyro_is_near_zero()
{
	float TimesSigma = 3.0;
	
	// Evaluate each axis independantly.  All 3 have to be near 0 to return TRUE.
	float variance = fabs(GyroVariance.x * TimesSigma);
	if (fabs(GyroAdjusted.rx) > variance) 	return FALSE;

	variance = fabs(GyroVariance.y * TimesSigma);
	if (fabs(GyroAdjusted.ry) > variance) 	return FALSE;

	variance = fabs(GyroVariance.z * TimesSigma);
	if (fabs(GyroAdjusted.rz) > variance) 	return FALSE;
	
	return TRUE;
}

void print_gyro_result()
{
	printf("Gyro  AngularRate [t=%6.3f](degrees/sec):\t\t  Rate about xyz = <%6.2f, %6.2f, %6.2f>\n",
		gyro_time_delta, GyroAdjusted.rx, GyroAdjusted.ry, GyroAdjusted.rz );
	printf("Gyro  Accumulated [t=%6.3f](degrees/sec):\t\t  Rotation about xyz = <%6.2f, %6.2f, %6.2f>\n",
		gyro_time_delta, GyroAccumulated.rx, GyroAccumulated.ry, GyroAccumulated.rz );
}
void print_gyro_results()
{
	print_gyro_result();
	//if (DEBUG) { printf("\nGyro Reading  (dps): \t\t");		print_vector( &GyroReading );	}
	//if (DEBUG) { printf("\nGyro Adjusted (dps): \t\t");		print_vector( &GyroAdjusted);	}
	//if (DEBUG) { printf("\nGyro Offset   (dps): \t\t");		print_vector( &GyroOffset  );	}
}

// Gyro:  1 bit = 0.375 degree per second	
float bits_to_dps = 0.375;

/* Processes raw data from the CAN message 
INPUT:
	GyroReading - is the input to this function.  
	Extracted from the CAN message in 
		Tilt sensor callback()						*/
void process_gyro( BOOL ShowData )
{
	if (DEBUG) printf("============Inside Process Gyro===============\n");
	
	// TIME STAMP:
	prev_gyro_timestamp = latest_gyro_timestamp;
	gettimeofday(&latest_gyro_timestamp, NULL);
	gyro_time_delta = calc_time_delta( &prev_gyro_timestamp, &latest_gyro_timestamp );
	if (ShowGyroData)
		printf("*** GYRO TIME DELTA=%10.4f\n", gyro_time_delta );		
	//calc_gyro_time_delta(DEBUG);

	// Scale & Subtract offset:
	scale( cast_f &RawxyzGyro,    bits_to_dps,   cast_f &GyroReading  );
	//subtract  ( cast_f &GyroReading,   cast_f &GyroOffset,      cast_f &GyroAdjusted );	
	//accumulate( cast_f &GyroAdjusted,  cast_f &GyroAccumulated, &NumberOffsetAccumulations );
	accumulate( cast_f &GyroReading,  cast_f &GyroAccumulated, &NumberOffsetAccumulations );	
	GyroAngles = GyroAccumulated;

	if (NumberOffsetAccumulations > ACCUMULATOINS_PER_PERIOD)		// gyro is assumed to be not moving during this time period!!
	{
		//adjust_zero_offset();
	}

	// Assess the GYRO's repeatability:
	if (sensor_not_moving)
	{
		// Accumulate Variances (ie. stddev)
		//subtract  ( cast_f &GyroAdjusted,	cast_f &KnownValue, cast_f &GyroDeviation);		
		//accumulate( cast_f &GyroDeviation, 	cast_f &SumGyroDeviations, &NumberVariancesAccumulated );
		//scale     ( cast_f &SumGyroDeviations, 1.0/NumberVariancesAccumulated,  cast_f &GyroVariance  );
	}

	// Gyro's best estimate of the sensor orientation angles (wrt gravity).
	/* Steady accumulation (integration) of the angle deltas */
	//accumulate( cast_f &GyroAdjusted, cast_f &GyroAngles, &NumberReadingAccumulated );
	// May still be some drift in GyroAngles.  Correct against the accelerometer.

	//printf("AR_weight_gyro:");	print_vector( &AR_weight_gyro  );	printf("\n");
	//printf("AR_weight_accel:");	print_vector( &AR_weight_accel );	printf("\n");
	//add   ( &tmp, &AR_weight_accel, &SumDeltaGyroVariances );

	// Adjust for temperature (offset)
	// Adjust for cross-axis sensitivity

	if (ShowData)
	{
		print_gyro_results();
		//printf("\nACCUM\t:");
		//print_avector( &GyroAngles );
	}	
	GyroAdjustedPrev = GyroAdjusted;
}


	/*  Gyro gives (degrees per second) 
				Scale by time_period ==> degrees change. 
				time between process_gyro() calls. 
		AccelAnglesDelta gives (degrees change). 
	struct fXYZ tmp;	
	scale ( &GyroReading,    gyro_time_delta, &GyroReading 	);
	scale ( &AR_weight_gyro, gyro_time_delta, &tmp 			);	*/
