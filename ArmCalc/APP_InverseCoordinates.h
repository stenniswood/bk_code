#ifndef _APP_INVERSE_COORDS_H_
#define _APP_INVERSE_COORDS_H_


#include <math.h>
#include <stdint.h>

#include "APP_Servos.h"


struct stBRH_Coordinate			// Robot cylindrical coordinate.
{
	float B;
	float R;
	float H;
};

struct stFull_BRH_Coordinate	// Robot cylindrical coordinate.
{
	float B;
	float R;
	float H;
	float Approach;
	float wrist_rotate;		// This is not calculated in the Find_XYZ function
};

struct stXYZ_Coordinate			// Robot cartesian coordinate.
{
	float X;
	float Y;
	float Z;
};

struct stFull_XYZ_Coordinate	// Robot cartesian coordinate.
{
	float X;
	float Y;
	float Z;
	float Approach;			// Radians
	float wrist_rotate;		// Radians  -  This is not calculated in the Find_XYZ function
};

extern struct stFull_XYZ_Coordinate  ActiveCoord;
extern uint8_t 	Feasibility;


//**************************** CALIBRATEABLE CONSTANTS **************************************
// These may be able to be automatically calibrated.  This would be done with the help of
// the tilt sensors.  By moving to 0 degrees, slightly and progressively decreasing the angle
// until no more movement is found.  Quickly backing off away from this position once found
// to releive stress on the gears of the servos.
#define MIN_BASE_ANGLE 0
#define MAX_BASE_ANGLE 180

#define MIN_SHOULDER_ANGLE 0
#define MAX_SHOULDER_ANGLE 180

#define MIN_ELBOW_ANGLE -150
#define MAX_ELBOW_ANGLE 30

#define MIN_WRIST_ANGLE -90
#define MAX_WRIST_ANGLE 90


/* The following define the maximum and minimum swings of the servos.
* The values are correct for the JR 821 digital servos 
* If other servos are used, or if it's desired to restrict the ranges beyond
*  the mechanical limits (depending on the application:  
*  update these values.
*/

#define MAX_SERVO_1_PW  0x0C6F			/* Base Rotation */
#define MIN_SERVO_1_PW  0x0600
#define MAX_SERVO_2_PW  0x0C6F			/* Shoulder Rotation */
#define MIN_SERVO_2_PW  0x0600
#define MAX_SERVO_3_PW  0x0C6F			/* Elbow Rotation */
#define MIN_SERVO_3_PW  0x0600
#define MAX_SERVO_4_PW  0x0C6F			/* Wrist Angle */
#define MIN_SERVO_4_PW  0x0600

#define MAX_SERVO_5_PW  0x0C6F			/* Wrist Rotation */
#define MIN_SERVO_5_PW  0x0600
#define MAX_SERVO_6_PW  0x0C6F			/* Gripper		  */
#define MIN_SERVO_6_PW  0x0600
#define MAX_SERVO_7_PW  0x0C6F
#define MIN_SERVO_7_PW  0x0600
#define MAX_SERVO_8_PW  0x0C6F
#define MIN_SERVO_8_PW  0x0600


#define SERVO_1_RANGE 		(MAX_SERVO_1_PW - MIN_SERVO_1_PW)
#define SERVO_1_INTERCEPT 	MIN_SERVO_1_PW
#define SERVO_2_RANGE 		(MAX_SERVO_2_PW - MIN_SERVO_2_PW)
#define SERVO_2_INTERCEPT 	MIN_SERVO_2_PW

#define SERVO_3_RANGE 		(MAX_SERVO_3_PW - MIN_SERVO_3_PW)
#define SERVO_3_INTERCEPT 	MIN_SERVO_3_PW
#define SERVO_4_RANGE 		(MAX_SERVO_4_PW - MIN_SERVO_4_PW)
#define SERVO_4_INTERCEPT 	MIN_SERVO_4_PW

#define SERVO_5_RANGE 		(MAX_SERVO_5_PW - MIN_SERVO_5_PW)
#define SERVO_5_INTERCEPT 	MIN_SERVO_5_PW
#define SERVO_6_RANGE 		(MAX_SERVO_6_PW - MIN_SERVO_6_PW)
#define SERVO_6_INTERCEPT 	MIN_SERVO_6_PW

#define SERVO_7_RANGE 		(MAX_SERVO_7_PW - MIN_SERVO_7_PW)
#define SERVO_7_INTERCEPT 	MIN_SERVO_7_PW
#define SERVO_8_RANGE 		(MAX_SERVO_8_PW - MIN_SERVO_8_PW)
#define SERVO_8_INTERCEPT 	MIN_SERVO_8_PW



#define TRUE 1
#define FALSE 0



//******************************* FUNCTIONS *******************************



// TRANSFORMATION FUNCTIONS:
void INV_Transform_XYZ_to_BRH(float mX, float mY, float mZ, struct stBRH_Coordinate* mResult);
void INV_Transform_BRH_to_XYZ(float mB, float mR, float mH, struct stXYZ_Coordinate* mResult);
void INV_sTransform_XYZ_to_BRH( struct stXYZ_Coordinate* mInput, struct stBRH_Coordinate* mResult);
void INV_sTransform_BRH_to_XYZ( struct stBRH_Coordinate* mInput, struct stXYZ_Coordinate* mResult);

// INVERSE EQUATIONS:
uint8_t INV_XYZ_To_Angles(struct stFull_XYZ_Coordinate* XYZ,    struct stServoAnglesSet* mResult);
uint8_t INV_BRH_To_Angles(struct stFull_BRH_Coordinate* mInput, struct stServoAnglesSet* mResult);

// AUX CALCS:
uint8_t INV_MaintPartOrient_CalcDestWristRotation(float mInitialBase, float mFinalBase, float mCurrentWristRotate, float* mFinalWR);
float INV_FindHighestAbove_XY(float mX,float mY);
float INV_FindHighestAbove_Radius(float mRadius);


// Helper Functions:
//void INV_Actuate_Position();
void INV_Find_BaseRotation(float mX, float mY);
void INV_Calc_ShoulderAndElbow_Angles(float mRadius, float mHeight, struct stServoAnglesSet* mResult );
void INV_RetainPosition( struct stFull_XYZ_Coordinate* mCoordinate );

void INV_Subtract_Gripper_RH(float *mB, float *mR, float *mH, float mApproachAngle);
void INV_Subtract_Gripper_XYZ(float *mX, float *mY, float *mZ, float mApproachAngle);
void INV_Subtract_BaseHeight( float* mZ );

void INV_Calc_Gripper_BRH(float *mR, float *mH, float mApproachAngle);
void INV_Calc_Gripper_XYZ(float mBase, float *mX, float *mY, float *mZ, float mApproachAngle);
void INV_Calc_ShoulderAndElbow_Angles(float mRadius, float mHeight, struct stServoAnglesSet* mResult );


//uint8_t INV_Add_XYZ_To_Queue(struct stFull_XYZ_Coordinate* XYZ);									// Equations and Add to queue.
//uint8_t INV_Add_XYZ_To_Queue_with_Result(struct stFull_XYZ_Coordinate* XYZ, struct stServoSet* mServoSet);

#endif
