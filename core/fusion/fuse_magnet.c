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
#include "fuse_magnet.h"
#include "global_funcs.h"


// See ../core/msg/parser_tilt.c   for RawxyzAccel and routine for extracting from CAN

struct frXYZ MagnetAngularPosition;			// 
struct frXYZ MagnetAngularPositionPrev;		// 
struct frXYZ MagnetAngularVelocity;			// degrees per second (dps)

/* Need to establish Reference Vector when level & 
	pointing due North.
   Reading taken when orientation is 0 for all rotations  */
struct fXYZ magnetReference;

// Time:
struct timeval  mag_start, mag_end;
float mag_time_delta_prev = 0;
float mag_time_delta 	  = 0.1;

void fuse_magnet_init()
{
	/* Sensor measurements taken when sensor was sitting on table with
	   a weight on top (so it could not move). */ 
	short tmp; 	  	  
	tmp = 0xff8d;		magnetReference.x = tmp;
	tmp = 0xfe33;		magnetReference.y = tmp;
	tmp = 0xff1e;		magnetReference.z = tmp;
	print_magrefence();
}

void print_mag_angles()
{
	printf("Magnet Angles (degrees): \t\t\t\trotate about xyz = <%6.3f, %6.3f, %6.3f>\n",
		MagnetAngularPosition.rx, MagnetAngularPosition.ry, MagnetAngularPosition.rz );	
}
void print_mag_velocity()
{
	printf("Magnet Velocity (degrees/second): \t\t\trotate about xyz = <%6.3f, %6.3f, %6.3f>\n",
		   MagnetAngularVelocity.rx, MagnetAngularVelocity.ry, MagnetAngularVelocity.rz );	
}
void print_magrefence()
{
	printf("\nMagnet Reference: <%6.3f, %6.3f, %6.3f>\n",
		magnetReference.x, magnetReference.y, magnetReference.z );	
}

/* MagnetReference is a vector with 3 axes pointing to the
	   magnet north pole. 	   
	We'll store a baseline vector which is correlated to
	   	level components up side, connector facing due north.   */
/* 
INPUT:  Data from CAN msg which has been put into the float of the fXYZ structure
OUTPUT: Result stored in file scope variable: MagnetAngularPosition
DESCRIPTION: Calculates the dot product of the incoming measurement 
		against the reference vector for each of the x=0, y=0, and
		z=0 planes.
*/
void magnet_angles( struct fXYZ* mRaw )
{	
	// Similar to accel_
	// Normalize the Raw Magnet vector
	//  Compared to 1 earth magnet field strength
	struct fXYZ ref = magnetReference;

/*		Project the baseline vector and the current vector 
		onto each plane (x=0,y=0,z=0), 
		mRaw dot_product Ref = cos(angle);
		Solve for the angle between them.		
*/
	// Compare to baseline vector, not the unit vectors!
	ref = magnetReference;
	ref.x = 0;		// yz plane
	float S=mRaw->x;  mRaw->x=0;  
	float S2=ref.x;   ref.x = 0;
	MagnetAngularPosition.rx  = rad_to_deg(angle_between( mRaw, &ref ));
	mRaw->x=S;
	ref.x = S2;

	ref = magnetReference;
	ref.y = 0;		// xz plane
	S=mRaw->y; mRaw->y=0;
	S2=ref.y;   ref.y = 0;	
	MagnetAngularPosition.ry  = rad_to_deg(angle_between( mRaw, &ref ));
	mRaw->y=S;
	ref.y = S2;	

	ref = magnetReference;
	ref.z = 0;		// xy plane
	S=mRaw->z;	mRaw->z=0;
	S2=ref.z;   ref.z = 0;
	MagnetAngularPosition.rz  = rad_to_deg(angle_between( mRaw, &ref ));
	mRaw->z=S;	
	ref.z = S2;


	MagnetAngularPosition.rz = degrees( atan2( mRaw->x,  mRaw->y ) );	

	// Need to Deal with Quadrants!?
	// sensor is responding somewhat correctly.  X is length of board.
}

void compute_mag_angular_velocity()
{
	// Now Compute Deltas (for comparison to Gyro)		
	//  (this is change of degrees)
	struct fXYZ tmpDelta;	
	subtract( cast_f &MagnetAngularPosition, cast_f &MagnetAngularPositionPrev, &tmpDelta );

	// Divide the time to get degrees/second (dps)
	float freq = 1.0/mag_time_delta;
	scale( &tmpDelta, freq, cast_f &MagnetAngularVelocity );
}

void get_magtime_update()
{
	// update Time between messages:
	gettimeofday( &mag_end, NULL );
	long seconds  = mag_end.tv_sec  - mag_start.tv_sec;
	long useconds = mag_end.tv_usec - mag_start.tv_usec;
	mag_time_delta_prev = mag_time_delta;
	mag_time_delta 	    = seconds + ((float)useconds/1000000.0);
	mag_start = mag_end;
}

/**********************************************************************
NAME:	process_magnet()
INPUT:  RawxyzMagnet - Data from CAN msg in a float fXYZ structure
OUTPUT: Result stored in file scope variable: MagnetAngularPosition
DESCRIPTION: Calculates the dot product of the incoming measurement 
		against the reference vector for each of the x=0, y=0, and
		z=0 planes.
**********************************************************************/
void process_magnet( BOOL ShowData )
{
	get_magtime_update();
	if (ShowData) 
		printf("*** MAGNET DELTA TIME=%10.4f\n", mag_time_delta);
		
	MagnetAngularPositionPrev = MagnetAngularPosition;	// Prepare for new data!

	// parse_magnet_msg() called in the Tilt callback handler fills
	// the RawxyzMagnet variable.
	magnet_angles( &RawxyzMagnet );
	compute_mag_angular_velocity();
	
	if (ShowData)
	{
		print_mag_angles  ();		
		print_mag_velocity();
	}
}



/* Need to establish Reference Vector when level &
	pointing due North.
   Reading taken when orientation is 0 for all rotations
*/
//struct fXYZ magnetReference;
/*void magnet_angles( struct fXYZ* mRaw )
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
}*/

/* Processes raw data from the CAN message 
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
}*/
