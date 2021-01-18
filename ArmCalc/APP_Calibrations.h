#ifndef _APP_CALIBRATIONS_H_
#define _APP_CALIBRATIONS_H_

#include "APP_InverseCoordinates.h"



/*struct stXYZ_Coordinate			// Robot cartesian coordinate.
{
	float X;
	float Y;
	float Z;
};*/

extern struct stXYZ_Coordinate ArmOrigin;


extern float LENGTH_SHOULDER;
extern float LENGTH_ELBOW;
extern float LENGTH_WRIST;
extern float BASE_HEIGHT;



void file_open		();
void read_origin	();
void read_arm_sizes	();
void read_zero_points();
void read_min_points();
void read_max_points();

void file_read();

void subtract_origin( float* mX, float* mY, float* mZ );
void add_origin		( float* mX, float* mY, float* mZ );

#endif

