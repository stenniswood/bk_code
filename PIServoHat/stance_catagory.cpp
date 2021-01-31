#include "stance_catagory.hpp"
#include "MPU6050.h"

//extern float ax, ay, az, gr, gp, gy; //Variables to store the accel, gyro and angle values


enum robotStances
{ RB_STANDING, RB_FACEDOWN, RB_BELLY_UP, RB_SIDE_RIGHT, RB_SIDE_LEFT };

#define MIN_FACE_DOWN_THRESHOLD -80
#define MAX_FACE_DOWN_THRESHOLD -125

#define MIN_FACE_UP_THRESHOLD 77
#define MAX_FACE_UP_THRESHOLD -270

#define MIN_SIDE_LEFT_THRESHOLD 77
#define MAX_SIDE_LEFT_THRESHOLD -270

#define MIN_SIDE_RIGHT_THRESHOLD 77
#define MAX_SIDE_RIGHT_THRESHOLD -270


enum robotStances RobotPosition;

enum robotStances catagorize_MPU( MPU6050_Velocity mpu )
{
/*	if ((mpu._angle[1]>MIN_SIDE_LEFT_THRESHOLD) && (mpu._angle[1]<MAX_SIDE_LEFT_THRESHOLD) )
	{
		RobotPosition = RB_SIDE_RIGHT;
	}
	else if ((mpu._angle[1]>MIN_SIDE_RIGHT_THRESHOLD) && (mpu._angle[1]<MAX_SIDE_RIGHT_THRESHOLD) )
	{
		RobotPosition = RB_SIDE_LEFT;
	}*/

	if ((mpu._angle[0]>MIN_FACE_DOWN_THRESHOLD) && (mpu._angle[0]<MAX_FACE_DOWN_THRESHOLD) )
	{
		RobotPosition = RB_FACEDOWN;
	}
	else if ((mpu._angle[0]>MIN_FACE_UP_THRESHOLD) && (mpu._angle[0]<MAX_FACE_UP_THRESHOLD) )
	{
		RobotPosition = RB_BELLY_UP;
	}
	else	
	{
		RobotPosition = RB_STANDING;
	}

};

