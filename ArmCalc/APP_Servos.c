#include <stdint.h>
#include "APP_Servos.h"
#include "APP_InverseCoordinates.h"
#include "APP_CoordinateTransform.h"


struct stServoInfo ServoInfo [NUM_SERVOS];



/*****************************************************************************
Name		:   INV_Angles_To_Servos()
Parameters	:	*_Angle must be the requested Angle in radians.
Return		:   none 				
Description	:   Convert all Angles (in Radians) to Servo timer counts.
*****************************************************************************/
void INV_Angles_To_Servos( struct stServoAnglesSet* mAngles, struct stServoSet* mServos )
{
	mServos->s1	= CT_Convert_Radians_to_Servo(mAngles->s1, 1);	
	mServos->s2 = CT_Convert_Radians_to_Servo(mAngles->s2, 2);	
	mServos->s3 = CT_Convert_Radians_to_Servo(mAngles->s3, 3);		
	mServos->s4 = CT_Convert_Radians_to_Servo(mAngles->s4, 4);		
}




/**********************************************************************************
Name		:   CT_Convert_Radians_to_Servo()
Parameters	:	Angle of the servo with calibration reference
Returns 	:   Servo Position Count
Description	:   Convert the angle in radians to a servo motor position.
				References the calibrated reference angles.
************************************************************************************/
uint16_t CT_Convert_Radians_to_Servo( float mRadians, uint8_t mServoNum )
{
	float Angle;
	float RadiansPerCount;
	uint16_t ServoPos;
	
	// Linearly interpolate between Min and Max.
	switch (mServoNum)
	{
		case 1 :
				RadiansPerCount = ((float)SERVO_1_RANGE / (float)MAX_RADIANS_SERVO_1 );
				ServoPos = (mRadians * RadiansPerCount);
				ServoPos += ZERO_OFFSET_COUNTS_SERVO_1;
				break;
		case 2 :
				mRadians = CT_Convert_FlipDirection( mRadians );
				RadiansPerCount = ((float)SERVO_2_RANGE / (float)MAX_RADIANS_SERVO_2 );
				ServoPos = (mRadians * RadiansPerCount);
				ServoPos += ZERO_OFFSET_COUNTS_SERVO_2;
				break;
		case 3 :
				RadiansPerCount = ((float)SERVO_3_RANGE / (float)MAX_RADIANS_SERVO_3 );
				ServoPos = (mRadians * RadiansPerCount);
				ServoPos += ZERO_OFFSET_COUNTS_SERVO_3;
				break;
		case 4 :
				mRadians = CT_Convert_FlipDirection( mRadians );
				RadiansPerCount = ((float)SERVO_4_RANGE / (float)MAX_RADIANS_SERVO_4 );
				ServoPos = (mRadians * RadiansPerCount);
				ServoPos += ZERO_OFFSET_COUNTS_SERVO_4;
				break;
		case 5 :
				RadiansPerCount = ((float)SERVO_5_RANGE / (float)MAX_RADIANS_SERVO_5 );
				ServoPos = (mRadians * RadiansPerCount);
				ServoPos += ZERO_OFFSET_COUNTS_SERVO_5;
				break;
		case 6 :
				RadiansPerCount = ((float)SERVO_6_RANGE / (float)MAX_RADIANS_SERVO_6 );
				ServoPos = (mRadians * RadiansPerCount);
				ServoPos += ZERO_OFFSET_COUNTS_SERVO_6;
				break;
		case 7 :
				RadiansPerCount = ((float)SERVO_7_RANGE / (float)MAX_RADIANS_SERVO_7 );
				ServoPos = (mRadians * RadiansPerCount);
				ServoPos += ZERO_OFFSET_COUNTS_SERVO_7;
				break;
		case 8 :
				// Note SERVO_x_RANGE is the number of timer counts which is in the working range of the servo.
				// In otherwords beyond the MIN and MAX counts no movement of the servo results.
				// Angle = (MAX_RADIANS / MAX_COUNT) * (ServoPos - ZeroOffsetCount)
				RadiansPerCount = ((float)SERVO_8_RANGE / (float)MAX_RADIANS_SERVO_8 );
				ServoPos = (mRadians * RadiansPerCount);
				ServoPos += ZERO_OFFSET_COUNTS_SERVO_8;
				// Note2:  We add the ZERO_OFFSET_COUNTS_SERVO_1 calibration position to 
				//			rather than the MIN_SERVO_1_PW.  This way the Angle is according to the calibrated
				//			horizontal positions.  See Get_Raw_servo_angle.
				break;				
		default:
				break;
	}
	return ServoPos;
}
