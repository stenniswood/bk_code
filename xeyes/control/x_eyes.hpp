#ifndef _X_EYES_HPP_
#define _X_EYES_HPP_

#define REVERSE_LEFT_EYE_UPDOWN 1
#define REVERSE_LEFT_EYE_LEFTRIGHT 1

// Indices into Servo INFO:
#define LEFT_UD 0
#define LEFT_LR 1
#define LEFT_EYELID 4
#define RIGHT_UD 2
#define RIGHT_LR 3
#define RIGHT_EYELID 5
extern struct stServo_info Servo_info[6];

//extern float PW_Left_up_center_offset  ;
//extern float PW_Right_up_center_offset ;


struct stServo_info {
	int min_pw;
	int max_pw;
	int PW_range;		// calculated based on above.	
	int zero_pw;		// zero degree angle (ie. looking straight ahead)
	bool reverse_direction;	// ie. a positive angle goes to smaller PW.	
};

void eye_init();
void adjust_eye_center( int mEyeIndex, int mRelativeOffset );

void* serial_setup(void*);
void  update_eye_positions(int x, int y, float mWidth, float mHeight);

float calc_left_right_angle( float mX, 
							 struct stServo_info& si,
							 float mScreenMin, float mScreenMax );
float calc_left_eye_angle ( float mMouseX, float mMouseY );
float calc_right_eye_angle( float mMouseX, float mMouseY );

float set_LeftEye_angle	 ( char* mBuffer, float mAngle );
float set_RightEye_angle ( char* mBuffer, float mAngle );

// UP/DOWN:
float  calc_eye_up_down_angle(  float mX,
								struct stServo_info& si,
								float mXMin, float mXMax );
void set_left_eye_up_angle	( char* mBuffer, float mDegrees );
void set_right_eye_up_angle	( char* mBuffer, float mDegrees );

// EYE LIDS:
int set_left_eyelid_angle	( char* mBuffer, float mAngle );
int set_right_eyelid_angle	( char* mBuffer, float mAngle );
void set_eyelids_angle		( float mAngle );
void open_eyelids 			( );
void close_eyelids			( );

#endif
