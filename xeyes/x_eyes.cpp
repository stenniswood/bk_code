#include <cstdio>
#include <string.h>
#include <math.h>
#include "serial.hpp"


char txt[80];
char txtR[80];
float yScale = 11.0;
float xScale = 22.0;


// SERVO DEFINES : 
const int MIN_PW = 500;
const int MAX_PW = 2500;
int PW_range = (MAX_PW - MIN_PW);

// EYE SERVO DEFINES : 
float X_LeftEye_coordinate = xScale/2.0 - 2.125/2.0;				// inches
float Y_LeftEye_coordinate = 0.0;	// inch distance away from eyes to the origin (tip of nose)
float LeftEye_MinAngle 	   = -90.0;
float LeftEye_MaxAngle 	   = +90.0;
float LeftEye_Range    	   = (LeftEye_MaxAngle-LeftEye_MinAngle);
int   LeftEye_CenterCount  = MIN_PW + PW_range/2.0;

float X_RightEye_coordinate = xScale/2.0 + 2.125/2.0;			// inches
float Y_RightEye_coordinate = 0.0;	// inch distance away from eyes to the origin (tip of nose)
float RightEye_MinAngle = -90.0;
float RightEye_MaxAngle = +90.0;
float RightEye_Range    = (RightEye_MaxAngle-RightEye_MinAngle);
int   RightEye_CenterCount  = MIN_PW + PW_range/2.0;

float LeftEyeUpMax 		= 2500;
float LeftEyeUpMin 		=  500;
float LeftEyeUpAngle   = +90;
float LeftEyeDownAngle = -90;

float RightEyeUpMax 	= 2500;
float RightEyeUpMin 	=  500;
float RightEyeUpAngle   = +90;
float RightEyeDownAngle = -90;

float calc_left_eye_angle( float mMouseX, float mMouseY )
{
	float x = mMouseX/540*xScale;
	float y = mMouseY/350*yScale;
	float SimpleY = (y - Y_LeftEye_coordinate);
	float SimpleX = (x - X_LeftEye_coordinate);
	float angle = atan2( SimpleX, SimpleY )*180./3.14159;
	return angle;
}
float calc_right_eye_angle( float mMouseX, float mMouseY )
{
	float x = mMouseX/540*xScale;
	float y = mMouseY/350*yScale;
	float SimpleY = (y - Y_RightEye_coordinate);
	float SimpleX = (x - X_RightEye_coordinate);	
	float angle = atan2( SimpleX, SimpleY )*180./3.14159;
	return angle;
}

float set_LeftEye_angle	( char* mBuffer, float mAngle )
{
//    Angle			    PW
//  -----------  =  -----------
//  Angle Range		 PW Range
	float PW = (((mAngle)*PW_range) / LeftEye_Range) + LeftEye_CenterCount;
	int PWi  = trunc(PW);
	sprintf(mBuffer, "#1P%d", PWi);
	return PW;
}
float set_RightEye_angle( char* mBuffer, float mAngle )
{
//    Angle			    PW
//  -----------  =  -----------
//  Angle Range		 PW Range
	float PW = (((mAngle)*PW_range) / RightEye_Range) + RightEye_CenterCount;
	int PWi = trunc(PW);
	sprintf(mBuffer, "#0P%d\r", PWi);
	return PW;
}


/******** LOOK UP/DOWN ROUTINES *********/
void set_left_eye_up_angle	( char* mBuffer, float mDegrees )
{
	float PW = (mDegrees*PW_range) / (LeftEyeUpMax-LeftEyeUpMin);
	sprintf(mBuffer, "#2P%d", trunc(PW));
}

void set_right_eye_up_angle	( char* mBuffer, float mDegrees )
{
	float PW = (mDegrees*PW_range) / (RightEyeUpMax-RightEyeUpMin);
	sprintf(mBuffer, "#3P%d", trunc(PW));
}

