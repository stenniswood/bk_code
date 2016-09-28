#include <cstdio>
#include <string.h>
#include <math.h>
#include "serial.hpp"
#include "x_eyes.hpp"


const int MIN_SERVO_PW = 553;
const int MAX_SERVO_PW = 2300;

struct stServo_info Servo_info[6];
char  txt [120];
char  txtR[120];
float yScale = 11.0;
float xScale = 22.0;


// EYE SERVO DEFINES : 
float X_LeftEye_coordinate = xScale/2.0 - 2.125/2.0;			// inches
float Y_LeftEye_coordinate = 0.0;	// inch distance away from eyes to the origin (tip of nose)
float LeftEye_MinAngle 	   = -90.0;
float LeftEye_MaxAngle 	   = +90.0;
float LeftEye_Range    	   = (LeftEye_MaxAngle-LeftEye_MinAngle);

float X_RightEye_coordinate = xScale/2.0 + 2.125/2.0;			// inches
float Y_RightEye_coordinate = 0.0;	// inch distance away from eyes to the origin (tip of nose)
float RightEye_MinAngle = -90.0;
float RightEye_MaxAngle = +90.0;
float RightEye_Range    = (RightEye_MaxAngle-(-90));


void adjust_eye_center( int mEyeIndex, int mRelativeOffset )
{
	Servo_info[mEyeIndex].zero_pw  +=  mRelativeOffset;	
}

void eye_init()
{	
	Servo_info[LEFT_UD].min_pw = MIN_SERVO_PW;
	Servo_info[LEFT_UD].max_pw = 2500;
	Servo_info[LEFT_UD].zero_pw = 0;
	Servo_info[LEFT_UD].reverse_direction = false;			

	Servo_info[LEFT_LR].min_pw = MIN_SERVO_PW;
	Servo_info[LEFT_LR].max_pw = MAX_SERVO_PW;
	Servo_info[LEFT_LR].zero_pw = 0;
	Servo_info[LEFT_LR].reverse_direction = false;			

	Servo_info[RIGHT_UD].min_pw = MIN_SERVO_PW;
	Servo_info[RIGHT_UD].max_pw = 2500;
	Servo_info[RIGHT_UD].zero_pw = 0;
	Servo_info[RIGHT_UD].reverse_direction = true;			

	Servo_info[RIGHT_LR].min_pw = MIN_SERVO_PW;
	Servo_info[RIGHT_LR].max_pw = MAX_SERVO_PW;
	Servo_info[RIGHT_LR].zero_pw = 0;
	Servo_info[RIGHT_LR].reverse_direction = true;

	// 
	Servo_info[LEFT_EYELID].min_pw = MIN_SERVO_PW;
	Servo_info[LEFT_EYELID].max_pw = MAX_SERVO_PW;
	Servo_info[LEFT_EYELID].zero_pw = 0;
	Servo_info[LEFT_EYELID].reverse_direction = false;			

	Servo_info[RIGHT_EYELID].min_pw = MIN_SERVO_PW;
	Servo_info[RIGHT_EYELID].max_pw = MAX_SERVO_PW;
	Servo_info[RIGHT_EYELID].zero_pw = 0;
	Servo_info[RIGHT_EYELID].reverse_direction = false;
	
	for (int i=0; i<6; i++)
		Servo_info[i].PW_range = (Servo_info[i].max_pw - Servo_info[i].min_pw);
	for (int i=0; i<6; i++)
		Servo_info[i].zero_pw = Servo_info[i].min_pw + (Servo_info[i].PW_range/2);
}

/*  PW for left to right movement.  Works for both eyes.
	Return : PW (Pulse Width)
 */
float calc_proportion( float mX, float mXMin, float mXMax,
						struct stServo_info& si )
{
	float PW;
	float Xcenter = (mXMax - mXMin)/2.0 + mXMin;
	float multiplier = (si.max_pw - si.min_pw) / (mXMax - mXMin);
	if (si.reverse_direction)
		PW = si.zero_pw - (mX - Xcenter)*multiplier;
	else
		PW = si.zero_pw + (mX - Xcenter)*multiplier;
	return PW;
}

/*  PW for left to right movement.  Works for both eyes.
	Return : PW (Pulse Width)
 */
float calc_left_right_angle( float mX,
							 struct stServo_info& si,
							 float mScreenMin, float mScreenMax )
{
	float multiplier = (si.max_pw - si.min_pw) / (mScreenMax - mScreenMin);
	float PW;
	if (si.reverse_direction)
		PW = si.zero_pw + si.max_pw - (mX - mScreenMin)*multiplier;
	else
		PW = si.zero_pw + si.min_pw + (mX - mScreenMin)*multiplier;
	return PW;
}

/* Not going to deal with actual angles right now! */
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

// Places into mBuffer.
float set_LeftEye_angle	( char* mBuffer, float PW )
{
//    Angle			    PW
//  -----------  =  -----------
//  Angle Range		 PW Range
	//float PW = (((mAngle)*PW_range) / LeftEye_Range) + LeftEye_CenterCount;
	int   PWi  = trunc(PW);
	sprintf(mBuffer, "#0P%d", PWi);
	return PW;
}
// Places into mBuffer.
float set_RightEye_angle( char* mBuffer, float PW )
{
	int PWi  = trunc(PW);
	sprintf(mBuffer, "#1P%d", PWi);
	return PW;
}

/*****************  LOOK UP/DOWN ROUTINES  ******************/
/* Return : Pulse width in ms.
 */
float calc_eye_up_down_angle (  float mX,
								struct stServo_info& si,
								float mXMin, float mXMax  )
{
	float PW;
	float multiplier = (si.max_pw - si.min_pw) / (mXMax - mXMin);
	if (si.reverse_direction)
	{
		PW = si.zero_pw + si.max_pw - (mX - mXMin)*multiplier;
	} else 
		PW = si.zero_pw + si.min_pw + (mX - mXMin)*multiplier;
	return PW;
}

void set_left_eye_up_angle( char* mBuffer, float mPulseWidth )
{
	if (REVERSE_LEFT_EYE_UPDOWN)
	{
	}
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
    float l_up_down_angle, r_up_down_angle;

	// Send Left/Right :	
	left_angle      = calc_left_right_angle ( x, Servo_info[LEFT_LR], 0, mWidth  );
	right_angle     = calc_left_right_angle ( x, Servo_info[RIGHT_LR], 0, mWidth );	
	l_up_down_angle = calc_eye_up_down_angle( mHeight-y, Servo_info[LEFT_UD], 0, mHeight  );
	r_up_down_angle = calc_eye_up_down_angle( mHeight-y, Servo_info[RIGHT_UD], 0, mHeight );

	set_LeftEye_angle ( txt, left_angle );
	char* ptr = txt + strlen( txt );
	set_RightEye_angle( ptr, right_angle );

	// Now Send Up/Down :
	ptr += strlen( ptr );
	set_left_eye_up_angle( ptr, l_up_down_angle + Servo_info[LEFT_UD].zero_pw );
	ptr += strlen( ptr );
	set_right_eye_up_angle( ptr, r_up_down_angle + Servo_info[RIGHT_UD].zero_pw );

	strcat(txt, "\r");
	si.my_write( txt, strlen(txt)   );
}

/* Angle should be 0 to 90. */
int set_left_eyelid_angle	( char* mBuffer, float mAngle )
{
	float PW  = calc_proportion( mAngle, 0, 90, Servo_info[LEFT_EYELID] );
	int   PWi = trunc(PW);
	sprintf(mBuffer, "#4P%d", PWi);
	return PWi;	
}
int set_right_eyelid_angle	( char* mBuffer, float mAngle )
{
	float PW  = calc_proportion( mAngle, 0, 90, Servo_info[RIGHT_EYELID] );
	int   PWi = trunc(PW);
	sprintf(mBuffer, "#5P%d", PWi);
	return PWi;
}
void set_eyelids_angle		( float mAngle )
{
	set_left_eyelid_angle	( txt, mAngle );
	char* ptr = txt + strlen( txt );
	set_right_eyelid_angle	( ptr,  mAngle );
	strcat(txt, "\r");
	si.my_write( txt, strlen(txt)   );	
}
void open_eyelids 			( )
{
	set_eyelids_angle( 0.0 );
}
void close_eyelids			( )
{
	set_eyelids_angle( 90.0 );
}

