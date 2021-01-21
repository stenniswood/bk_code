

#include "stance_catagory.hpp"

extern float ax, ay, az, gr, gp, gy; //Variables to store the accel, gyro and angle values


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

void catagorize_MPU()
{
	if ((gp>MIN_SIDE_LEFT_THRESHOLD) && (gp<MAX_SIDE_LEFT_THRESHOLD) )
	{
		RobotPosition = RB_SIDE_RIGHT;
	}
	else if ((gp>MIN_SIDE_RIGHT_THRESHOLD) && (gp<MAX_SIDE_RIGHT_THRESHOLD) )
	{
		RobotPosition = RB_SIDE_LEFT;
	}


	if ((gp>MIN_FACE_DOWN_THRESHOLD) && (gp<MAX_FACE_DOWN_THRESHOLD) )
	{
		RobotPosition = RB_FACEDOWN;
	}
	else if ((gp>MIN_FACE_UP_THRESHOLD) && (gp<MAX_FACE_UP_THRESHOLD) )
	{
		RobotPosition = RB_BELLY_UP;
	}
	else	
	{
		RobotPosition = RB_STANDING;
	}

};

