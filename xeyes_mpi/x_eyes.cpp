#include <cstdio>
#include <string.h>
#include <math.h>
#include "serial.hpp"

const int MIN_SERVO_PW = 553;
const int MAX_SERVO_PW = 2300;

char txt[120];
char txtR[120];
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
float LeftEyeUpAngle    = +90;
float LeftEyeDownAngle  = -90;
float PW_Left_up_center_offset = 0.0;

float RightEyeUpMax 	= 2500;
float RightEyeUpMin 	=  500;
float RightEyeUpAngle   = +90;
float RightEyeDownAngle = -90;
float PW_Right_up_center_offset = 0.0;

float calc_left_right_angle( float mX,
								float mPWMin, float mPWMax,
								float mScreenMin, float mScreenMax )
{
	float multiplier = (mPWMax - mPWMin) / (mScreenMax - mScreenMin);
	//printf("lr_angle: multiplier=%6.2f;  PWMin=%6.2f;  xdelta=%6.2f\n",
	//			multiplier, mPWMin, mX-mScreenMin );
	float PW = mPWMin + (mX - mScreenMin)*multiplier;
	return PW;
}

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
	float PW = mAngle;
	//float PW = (((mAngle)*PW_range) / LeftEye_Range) + LeftEye_CenterCount;
	int   PWi  = trunc(PW);
	sprintf(mBuffer, "#0P%d", PWi);
	return PW;
}
float set_RightEye_angle( char* mBuffer, float mAngle )
{
//    Angle			    PW
//  -----------  =  -----------
//  Angle Range		 PW Range
	float PW = mAngle;
	//float PW = (((mAngle)*PW_range) / RightEye_Range) + RightEye_CenterCount;
	int PWi  = trunc(PW);
	sprintf(mBuffer, "#1P%d", PWi);
	return PW;
}

/*****************  LOOK UP/DOWN ROUTINES  ******************/
/* Return : Pulse width in ms.
 */
float calc_eye_up_down_angle (  float mX,
								float mPWMin, float mPWMax,
								float mXMin, float mXMax  )
{
	float multiplier = (mPWMax - mPWMin) / (mXMax - mXMin);
	float PW = mPWMin + (mX - mXMin)*multiplier;
	return PW;
}

void set_left_eye_up_angle( char* mBuffer, float mPulseWidth )
{
	int pw = trunc(mPulseWidth);
	sprintf(mBuffer, "#2P%d", pw );
}

void set_right_eye_up_angle( char* mBuffer, float mPulseWidth )
{
	int pw = trunc(mPulseWidth);
	sprintf(mBuffer, "#3P%d", pw );
}


void* serial_setup(void*)				// serial port used for arduino connections & GPS.
{
	static char  app_name  [12];
	static char  read_r    [4];
	static char  no_write_w[4];
	static char  device_p  [4];
	static char  read_     [20];
	static char  no_write_ [20];
	static char  device_   [20];

	sprintf(app_name,   "./xeyes");
	sprintf(read_r,     "-R");
	sprintf(no_write_w, "-w");
	sprintf(device_p,   "-p");

	sprintf(read_,     "ascii");
	sprintf(no_write_, "0");

	sprintf(device_,   "/dev/ttyUSB0");
	si._cl_rx_dump = 1;
	si._cl_rx_dump_ascii = 1;		
	si._cl_port = strdup("/dev/ttyUSB0");
	si._cl_tx_bytes = 0;

	char * argv[] = { app_name, read_r, read_, no_write_w, no_write_, device_p, device_ }; 
	
	si.process_options( 7, argv );	
	si.serial_main    ( );
	return NULL;
}

void update_eye_positions(int x, int y, float mWidth, float mHeight)
{
    float left_angle;
    float right_angle;
    float up_down_angle;

	// Send Left/Right :
	right_angle   = left_angle = calc_left_right_angle( x, MIN_SERVO_PW, MAX_SERVO_PW, 0, mWidth );
	up_down_angle = calc_eye_up_down_angle            ( mHeight-y, MIN_SERVO_PW, MAX_SERVO_PW, 0, mHeight );
	
	set_LeftEye_angle ( txt, left_angle ); 
	char* ptr     = txt + strlen( txt ); 
	set_RightEye_angle( ptr, right_angle ); 

	// Now Send Up/Down :
	ptr += strlen( ptr );
	set_left_eye_up_angle ( ptr, up_down_angle + PW_Left_up_center_offset );
	ptr += strlen( ptr );
	set_right_eye_up_angle( ptr, up_down_angle + PW_Right_up_center_offset );

	strcat(txt, "\r");
	si.my_write( txt, strlen(txt)   );
//	printf("%d,%d : %s\n", x,y, txt );
//	printf("%d,%d : \n", x,y );
}

