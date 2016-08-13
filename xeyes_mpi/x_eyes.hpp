



void* serial_setup(void*);
void update_eye_positions(int x, int y, float mWidth, float mHeight);


float calc_left_right_angle( float mX, float mPWMin, float mPWMax,
							 float mScreenMin, float mScreenMax );

float calc_left_eye_angle ( float mMouseX, float mMouseY );
float calc_right_eye_angle( float mMouseX, float mMouseY );

float set_LeftEye_angle	 ( char* mBuffer, float mAngle );
float set_RightEye_angle ( char* mBuffer, float mAngle );

// UP/DOWN:
float  calc_eye_up_down_angle(  float mX,
								float mYMin, float mYMax,
								float mXMin, float mXMax );
void set_left_eye_up_angle	( char* mBuffer, float mDegrees );
void set_right_eye_up_angle	( char* mBuffer, float mDegrees );


extern float PW_Left_up_center_offset  ;
extern float PW_Right_up_center_offset ;


extern char txt[];
extern char txtR[];