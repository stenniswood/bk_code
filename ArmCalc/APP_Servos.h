#ifndef _APP_SERVOS_H_
#define _APP_SERVOS_H_




/********************************************************************
********************	CALIBRATION DATA   **************************
*********************************************************************/
// The following must be measured (or approximated)
// It shows the number of degrees spanned from the 
//		MIN_SERVO_n_PW to the MIN_SERVO_n_PW.  
// This will be used to convert the pulse width to an actual angle 
// in radians or degrees.  
//
//Robot arm consists only of 6 servos!
#define MAX_DEGREES_SERVO_1 180
#define MAX_DEGREES_SERVO_2 180
#define MAX_DEGREES_SERVO_3 180
#define MAX_DEGREES_SERVO_4 180
#define MAX_DEGREES_SERVO_5 180
#define MAX_DEGREES_SERVO_6 180
#define MAX_DEGREES_SERVO_7 180
#define MAX_DEGREES_SERVO_8 180

// Following are the additonal timer counts required to put the segment  
// in Horizontal position.  For example, Shoulder, Elbow, and Wrist.
// See the OneNote document for diagrams.
#define ZERO_OFFSET_COUNTS_SERVO_1  0x0600
#define ZERO_OFFSET_COUNTS_SERVO_2  0x0600
#define ZERO_OFFSET_COUNTS_SERVO_3  0x0C13   // (3183 - 1536)/180*170 = 1555+1536 = 3091   // make around 170deg
#define ZERO_OFFSET_COUNTS_SERVO_4  0x0600
#define ZERO_OFFSET_COUNTS_SERVO_5  0x0600
#define ZERO_OFFSET_COUNTS_SERVO_6	0x0600
#define ZERO_OFFSET_COUNTS_SERVO_7	0x0600
#define ZERO_OFFSET_COUNTS_SERVO_8	0x0600

// Following are calculation based on the CAL data above.
#define MAX_RADIANS_SERVO_1 (((float)MAX_DEGREES_SERVO_1*(float)M_PI)/180.0)
#define MAX_RADIANS_SERVO_2 (((float)MAX_DEGREES_SERVO_2*(float)M_PI)/180.0)
#define MAX_RADIANS_SERVO_3 (((float)MAX_DEGREES_SERVO_3*(float)M_PI)/180.0)
#define MAX_RADIANS_SERVO_4 (((float)MAX_DEGREES_SERVO_4*(float)M_PI)/180.0)
#define MAX_RADIANS_SERVO_5 (((float)MAX_DEGREES_SERVO_5*(float)M_PI)/180.0)
#define MAX_RADIANS_SERVO_6 (((float)MAX_DEGREES_SERVO_6*(float)M_PI)/180.0)
#define MAX_RADIANS_SERVO_7 (((float)MAX_DEGREES_SERVO_7*(float)M_PI)/180.0)
#define MAX_RADIANS_SERVO_8 (((float)MAX_DEGREES_SERVO_8*(float)M_PI)/180.0)


struct stServoSet	// Structure holding all arm servos
{
	uint16_t s1;			// base
	uint16_t s2;			// shoulder
	uint16_t s3;			// elbow
	uint16_t s4;			// wrist
	uint16_t s5;			// wrist_rotate
	uint16_t s6;			// gripper
};

struct stServoAnglesSet	// Structure holding all arm servos  All Angles in Radians
{
	float s1;		// base 
	float s2;		// shoulder
	float s3;		// elbow
	float s4;		// wrist
	float s5;		// wrist_rotate
	float s6;		// gripper
};

struct stServoInfo 
{
	uint16_t min_value;
	uint16_t max_value;
	uint16_t zero_value;
	
	float min_angle;
	float max_angle;
	float zero_angle;		// Not used.
};

#define NUM_SERVOS 6
extern struct stServoInfo ServoInfo[NUM_SERVOS];


uint16_t CT_Convert_Radians_to_Servo( float mRadians, uint8_t mServoNum );
void 	INV_Angles_To_Servos( struct stServoAnglesSet* mAngles, struct stServoSet* mServos );

float 	CT_Convert_Servo_to_Radians( uint16_t mServoPos, uint8_t mServoNum ); 
float 	CT_Convert_Servo_to_Degrees( uint16_t mServoPos, uint8_t mServoNum ); 


#endif
