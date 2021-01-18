/*********************************************************************
 *											   						 *
 *    DESCRIPTION:  Coordinate Transforms 			 	 			 *
 *											  						 *
 *	  CATEGORY	:  Low leve APP										 *
 *											  						 *
 *    AUTHOR	:  Stephen Tenniswood, Embedded Engineer 	 		 *
 *											   						 *
 *    PURPOSE	:  Routines to convert Servo positions into Angles.	 *
 *			   	   and to convert Angles into an <X,Y,Z> coordinate. *
 *																	 *
 *	  DEFINITION:  XY origin is located directly under the base servo*
 *					center of rotation.								 *
 *				:  Z origin is level with the table top				 *
 *				:													 *
 *																	 *
 *	  DATE		:  Oct 27,2008										 *
 *********************************************************************/
#include <math.h>
#include <stdio.h>

//#include "skp_bsp.h"
//#include "DEV_OCpwm.h"
//#include "MID_FilterServo.h"
//#include "APP_ArmPositionStorage.h"
#include "APP_CoordinateTransform.h"
#include "APP_InverseCoordinates.h"
#include "APP_Calibrations.h"
#include "APP_Servos.h"


typedef struct stServoSet ServoResult;

// The following angles are the actual angles only!
// CT_Convert_Radians_to_Servo() takes care of the calibrated zero offsets 
/********************* INPUT **********************/
float Approach_Angle = 0.0;		// Port of the radius covered by the shoulder piece.

/********************* OUTPUT **********************/
/*float Total_Radius = 0.0;		// Intermediate Calculations - can be input for cylindrical
float Total_Height = 0.0;		// Intermediate Calculations - can be input for cylindrical
*/
/*float Base_Angle	  = 0.0;	// Angle of the Base to the XY coordinates.  There is no Radius for base!
float Shoulder_Radius = 0.0;	// Portion of the radius covered by the shoulder piece
float Shoulder_Angle  = 0.0;	// Angle of the shoulder needed to attain the radius above
float Elbow_Radius 	  = 0.0;	// Projection of the radius covered by the shoulder piece
float Elbow_Angle  	  = 0.0;	// Angle of the shoulder needed to attain the radius above
float Wrist_Radius 	  = 0.0;	// Projection of the radius covered by the shoulder piece
float Wrist_Height 	  = 0.0;	// Projection of the radius covered by the shoulder piece
float Wrist_Angle 	  = 0.0;	// Angle of the shoulder needed to attain the radius above
*/

uint8_t 	Feasibility		= TRUE;
uint16_t base_servo 		= 0x0000;
uint16_t shoulder_servo 	= 0x0000;
uint16_t elbow_servo 	= 0x0000;
uint16_t wrist_servo 	= 0x0000;

struct stFull_XYZ_Coordinate  ActiveCoord;
struct stFull_XYZ_Coordinate  NewCoord; 


struct stXYZ_Coordinate;
struct stBRH_Coordinate;



/*****************************************************************************
Name		:   INV_Convert_XYZ_to_BRH()
Parameters	:	X,Y,Z given in inches.
Return		:   
Description	:   This converts to cylindrical coordinates.
*****************************************************************************/
void INV_Transform_XYZ_to_BRH(float mX, float mY, float mZ, struct stBRH_Coordinate* mResult)
{
	mResult->B = atan2(mY,mX);
	mResult->R = sqrt(mX*mX + mY*mY);
	mResult->H =	mZ;		
}

/*****************************************************************************
Name		:   INV_Convert_BRH_to_XYZ()
Parameters	:	BRH given in inches.
Return		:   XYZ
Description	:   This converts to cartesian coordinates.
*****************************************************************************/
void INV_Transform_BRH_to_XYZ(float mB, float mR, float mH, struct stXYZ_Coordinate* mResult)
{
	mResult->Y = sin(mB)*mR;
	mResult->X = cos(mB)*mR;
	mResult->Z = mH;
}

/*****************************************************************************
Name		:   INV_Convert_XYZ_to_BRH()
Parameters	:	X,Y,Z given in inches.
Return		:   
Description	:   This converts to cylindrical coordinates.
*****************************************************************************/
void INV_sTransform_XYZ_to_BRH( struct stXYZ_Coordinate* mInput, struct stBRH_Coordinate* mResult)
{
	mResult->B = atan2(mInput->Y, mInput->X);
	mResult->R = sqrt(mInput->X * mInput->X + mInput->Y * mInput->Y);
	mResult->H = mInput->Z;
}

/*****************************************************************************
Name		:   INV_Convert_BRH_to_XYZ()
Parameters	:	BRH given in inches.
Return		:   XYZ
Description	:   This converts to cartesian coordinates.
*****************************************************************************/
void INV_sTransform_BRH_to_XYZ( struct stBRH_Coordinate* mInput, struct stXYZ_Coordinate* mResult)
{
	mResult->Y = sin(mInput->B) * mInput->R;
	mResult->X = cos(mInput->B) * mInput->R;
	mResult->Z = mInput->H;
}
/****************************************************************************************/
/********************************** END OF TRANSFORMS ***********************************/
/****************************************************************************************/


/*****************************************************************************
Name		:   INV_Calc_BaseRotation()
Parameters	:	X,Y given in inches
Return		:   Base angle in radians
Description	:   This function performs the inverse calculations for finding
				the base rotation which will position the griper towards the
				specified position.				
*****************************************************************************/
float INV_Calc_BaseRotation(float mX, float mY)
{
	// atan2 takes care of the quadrant issue - good for 0 to 180 degrees.
	return atan2(mY,mX);
}

/*****************************************************************************
Name		:   INV_Calc_RadiusProjection()
Parameters	:	X,Y,Z given in inches.
Return		:   Projection of the radius in the XY plane
Description	:   This function performs the inverse calculations for finding
				the servo motor positions which will position the griper at
				the specified X,Y,Z position.
*****************************************************************************/
float INV_Calc_RadiusProjection(float mX, float mY)
{
	return sqrt(mX*mX + mY*mY);
}

/*****************************************************************************
Name		:   INV_Calc_Gripper_BRH()
Parameters	:	X,Y,Z given in inches.
Return		:   
Description	:   Calculates the amount of Radius&Height taken up by the wrist
*****************************************************************************/
void INV_Calc_Gripper_RH(float *mR, float *mH, float mApproachAngle)
{
	*mR = LENGTH_WRIST * cos(mApproachAngle);
	*mH = LENGTH_WRIST * sin(mApproachAngle);
}

/*****************************************************************************
Name		:   INV_Calc_Gripper_XYZ()
Parameters	:	Base angle of the arm radians
			:   Approach Angle in radians.
Return		:   X,Y,Z given in inches
Description	:   Calculates the amount of XYZ taken up by the wrist
*****************************************************************************/
void INV_Calc_Gripper_XYZ(float mBase, float *mX, float *mY, float *mZ, float mApproachAngle)
{
	float R;
	float H;
	INV_Calc_Gripper_RH( &R,&H,mApproachAngle );
	
	*mX = R * cos(mBase);
	*mY = R * sin(mBase);
	*mZ = H;
}

/*****************************************************************************
Name		:   INV_Subtract_Gripper_RH()
Parameters	:	X,Y,Z given in inches
Return		:   
Description	:   The coordinate frame is on the table top.  So whatever Z above
				table top is desired, we need to subtract the height that the base
				contributes.
*****************************************************************************/
void INV_Subtract_Gripper_RH(float *mB, float *mR, float *mH, float mApproachAngle)
{
	*mR -= LENGTH_WRIST * cos(mApproachAngle);
	*mH -= LENGTH_WRIST * sin(mApproachAngle);
}

/*****************************************************************************
Name		:   INV_Subtract_Gripper_XY()
Parameters	:	X,Y,Z designate the postion of the tip of the gripper (inches)
Return		:   TRUE if arm position can be Reached with the arm 
				FALSE if Unreachable
Description	:   The coordinate frame is on the table top.  So whatever Z above
				table top is desired, we need to subtract the height that the base
				contributes.
	
	 The approach angle is relative to the XYZ coordinate frame
	 Therefore no other angles are needed.  
	 0 is horizontal
	 -AA is a downward angle
	 +AA is an upward angle
*****************************************************************************/
void INV_Subtract_Gripper_XYZ(float *mX, float *mY, float *mZ, float mApproachAngle)
{
	float B = INV_Calc_BaseRotation( *mX, *mY );
	float X,Y,Z;
	INV_Calc_Gripper_XYZ( B, &X, &Y, &Z, mApproachAngle );
	
	*mX = *mX - X;
	*mY = *mY - Y;
	*mZ = *mZ - Z;
}

/*****************************************************************************
Name		:   INV_Subtract_BaseHeight()
Parameters	:	mZ the Z height in inches
Return		:   mZ - origin is now even with the top of the base servo.
Description	:   Subtracts the stored height of the base.
*****************************************************************************/
void INV_Subtract_BaseHeight( float* mZ )
{
	*mZ -= BASE_HEIGHT;
}


/*****************************************************************************
Name		:   INV_Calc_ShoulderAndElbow_Angles()
Parameters	:	X,Y,Z given in inches.
Return		:   Fills in S2 and S3 of the mResult structure (radians)
Description	:   This function performs the inverse calculations for finding
				the servo motor positions which will position the griper at
				the specified X,Y,Z position.
*****************************************************************************/
void INV_Calc_ShoulderAndElbow_Angles(float mRadius, float mHeight, struct stServoAnglesSet* mResult )
{
	float t1,t2,t3;
	float Direct_SW_Angle1,Angle2;
	float Numerator;
	float Direct_SW_LineLength;
	
	Direct_SW_LineLength = sqrt(mRadius*mRadius + mHeight*mHeight);		// Spherical Radius.
	Direct_SW_Angle1     = atan2( mHeight, mRadius );						// Elevation angle

	// Law of Cosines : 
	t1 = (LENGTH_SHOULDER * LENGTH_SHOULDER);
	t2 = (LENGTH_ELBOW * LENGTH_ELBOW);
	t3 = (Direct_SW_LineLength * Direct_SW_LineLength);
	float numerator = (t1-t2+t3);
	float denominator = (2*LENGTH_SHOULDER*Direct_SW_LineLength);
	
	if (numerator>denominator)
	{
		printf("ERROR: POSITION IS OUT OF REACH!\n");
	}
//	printf("Law of Cosines:\n  a=%6.3f\n  b=%6.3f\n  c=%6.3f\n", LENGTH_SHOULDER, LENGTH_ELBOW, Direct_SW_LineLength);
//	printf("INV_Calc_ShoulderAndElbow_Angles:  Numerator=%6.3f;  Denom=%6.3f\n",(t1-t2+t3),
//	  		(2*LENGTH_SHOULDER*Direct_SW_LineLength) );
	  
	Angle2 = acos( numerator / denominator );

	// Shoulder Angle : 
	mResult->s2 = Direct_SW_Angle1 + Angle2;		// in radians
	//printf("INV_Calc_ShoulderAndElbow_Angles:  Shoulder Angle=%6.3f degs;\n", CT_Convert_Radians_to_Degrees(mResult->s2) );


	// Elbow_Angle - Law of Cosines:
	mResult->s3 = acos( (t1+t2-t3)/(2*LENGTH_SHOULDER*LENGTH_ELBOW) );
	//printf("INV_Calc_ShoulderAndElbow_Angles:  Elbow Angle=%6.3f degs;\n", CT_Convert_Radians_to_Degrees(mResult->s3) );
	mResult->s3 = - CT_Convert_FlipDirection(mResult->s3);
}

/*****************************************************************************
Name		:   INV_Calc_Wrist()
Parameters	:	Approach_Angle (radians)
			:	Shoulder and Elbow angles must already be properly filled in 
			:   the mAngleSet structure (radians)
Return		:   mAngleSet->s4 (Wrist Angle in radians)
Description	:   The coordinate frame is on the table top.  So whatever Z above
				table top is desired, we need to subtract the height that the 
				base contributes.
*****************************************************************************/
void INV_Calc_WristAngle(float Approach_Angle, struct stServoAnglesSet* mAngleSet)
{
	mAngleSet->s4 = (Approach_Angle - mAngleSet->s2 - mAngleSet->s3);
}


/*****************************************************************************
Name		:   INV_Determine_Feasibility()
Parameters	:	X,Y,Z given in inches
			:   mApproach in degrees
Return		:   0 - if arm position can be Reached with the arm 
				1 - Out of Reach
				2 - Base 		Rotation not possible
				3 - Shoulder 	Rotation out of range
				4 - Elbow 		Rotation out of range
				5 - Wrist 		Rotation out of range				
Description	:   This function performs the inverse calculations for finding
				the servo motor positions which will position the griper at
				the specified X,Y,Z position.
*****************************************************************************/
uint8_t INV_Determine_Feasibility(float mRadius, float mHeight, struct stServoAnglesSet* mAngleSet)
{
	float tmpAngle;
	float Hypotenus = sqrt(mRadius*mRadius + mHeight*mHeight);
	
	// Wrist has been subtracted!
	// If the hypotenus of the larger triangle (user specified) exceeds L1+L2. Not possible.
	// I think any other position can be reached.  That is to say any position inside the 
	// circle defined by Sqrt(L1+L2)
	
	if (Hypotenus > (LENGTH_SHOULDER+LENGTH_ELBOW) )
		return 1;			// Out of Reach
		
	// Now we know that were within a circle which is less than our L1 and L2 combined.
	// Check the angles of each servo motor:		
	tmpAngle = CT_Convert_Radians_to_Degrees(mAngleSet->s1);
	if ((tmpAngle < MIN_BASE_ANGLE) || (tmpAngle > MAX_BASE_ANGLE))
		return 2;

	tmpAngle = CT_Convert_Radians_to_Degrees(mAngleSet->s2);	
	if ((tmpAngle < MIN_SHOULDER_ANGLE) || (tmpAngle > MAX_SHOULDER_ANGLE))
		return 3;

	tmpAngle = CT_Convert_Radians_to_Degrees(mAngleSet->s3);		
	if ((tmpAngle < MIN_ELBOW_ANGLE) || (tmpAngle > MAX_ELBOW_ANGLE))
		return 4;
		
	tmpAngle = CT_Convert_Radians_to_Degrees(mAngleSet->s4);	
	if ((tmpAngle < MIN_WRIST_ANGLE) || (tmpAngle > MAX_WRIST_ANGLE))
		return 5;
		
	return 0;
}

/*****************************************************************************
Name		:   INV_FindHighestAboveXY()
Parameters	:	mRadius - X,Y,Z given in inches
			:   mApproach in degrees
Return		:   Heighest Z coordinate
Description	:   Finds the Highest Z possible above a given Radius
*****************************************************************************/
float INV_FindHighestAbove_Radius(float mRadius)
{
	float Height;
	float MaxLength = (LENGTH_SHOULDER+LENGTH_ELBOW);
	
	Height = sqrt(MaxLength*MaxLength - mRadius*mRadius);
	return Height;	
}

/*****************************************************************************
Name		:   INV_FindHighestAbove_XY()
Parameters	:	X,Y given in inches
Return		:   Heighest Z coordinate
Description	:   Finds the Highest Z possible above a given XY coordinate
			:   XY is the WRIST Position!  In other words approach is already 
			:	defined and distance subtracted.
*****************************************************************************/
float INV_FindHighestAbove_XY(float mX,float mY)
{
	float Radius = sqrt(mX*mX + mY*mY);
	return INV_FindHighestAbove_Radius( Radius );
	
}

/*****************************************************************************
Name		:   INV_Find_XYZ()
Parameters	:	X,Y,Z given in inches.
			:   mApproach in radians.
Return		:   mResult Structure is filled in. (the Servo Angles Set)
			:   TRUE if arm position can be Reached with the arm 
			:	FALSE if Unreachable
Description	:   This function performs the inverse calculations for finding
				the servo motor positions which will position the griper at
				the specified X,Y,Z position.
*****************************************************************************/
//uint8_t INV_Find_XYZ(float mX, float mY, float mZ, float mApproach_degrees, struct stServoAnglesSet* mResult)
uint8_t INV_XYZ_To_Angles(struct stFull_XYZ_Coordinate* XYZ, struct stServoAnglesSet* mResult)
{
	// Begin Calculations:
	float Radius;
	mResult->s1	 = INV_Calc_BaseRotation( XYZ->X, XYZ->Y );
	
	Radius 		 = INV_Calc_RadiusProjection( XYZ->X, XYZ->Y );
	
	// Subtract the gripper segment to find the Wrist XYZ
	//INV_Subtract_Gripper_RH( &(mResult->s1), &Radius, &(XYZ->Z), XYZ->Approach );			
	//INV_Subtract_BaseHeight( &(XYZ->Z) );

	INV_Calc_ShoulderAndElbow_Angles(Radius, XYZ->Z, mResult);
	INV_Calc_WristAngle( XYZ->Approach, mResult );

	Feasibility = INV_Determine_Feasibility( Radius, XYZ->Z, mResult );
	if (Feasibility > 0)			// if error
		return FALSE;				// not reachable!

	// At this point, only *_Angles have been updated.  No intermediate servo positions
	// or actual servos.  Call Actuate if this is desired.
	return TRUE;		// Position is feasible
}

void INV_RetainPosition( struct stFull_XYZ_Coordinate* mCoordinate )
{
	// Since Valid Position, Store.  This is retained 
	NewCoord.X = mCoordinate->X;
	NewCoord.Y = mCoordinate->Y;
	NewCoord.Z = mCoordinate->Z;
	NewCoord.Approach = mCoordinate->Approach;
	NewCoord.wrist_rotate = mCoordinate->wrist_rotate;
}

/*****************************************************************************
Name		:   INV_Find_BRH()
Parameters	:	mBase		Servo count position (not degrees, not radians)
				mRadius			
				mHeight 	
			:   mApproach in degrees
Return		:   TRUE if arm position can be Reached with the arm 
				FALSE if Unreachable
Description	:   This function performs the inverse calculations for finding
				the servo motor positions which will position the griper at
				the specified X,Y,Z position
*****************************************************************************/
uint8_t INV_BRH_To_Angles(struct stFull_BRH_Coordinate* mInput, struct stServoAnglesSet* mResult)
{
	mResult->s1 = mInput->B;
	
	// We need to subtract the gripper segment before the following calculations
	INV_Subtract_Gripper_RH( &(mInput->B), &(mInput->R), &(mInput->H), mInput->Approach );
	INV_Subtract_BaseHeight( &(mInput->H) );

	INV_Calc_ShoulderAndElbow_Angles( mInput->R, mInput->H, mResult);
	INV_Calc_WristAngle( mInput->Approach, mResult );

	Feasibility = INV_Determine_Feasibility( mInput->R, mInput->H, mResult );
	if (Feasibility > 0)			// if error
		return FALSE;				// not reachable!

	// At this point, only *_Angles have been updated.  No intermediate servo positions
	// or actual servos.  Call Actuate if this is desired.
	return TRUE;		// Position is feasible
}


/*****************************************************************************
Name		:   INV_MaintPartOrient_CalcDestWristRotation()
Parameters	:	mInitialBase
			:	mFinalBase
			:	mCurrentWristRotate
			:	mFinalWR
Return		:   TRUE if arm position can be Reached with the arm 
				FALSE if Unreachable
Description	:   For picking up an object from the top.  If user wishes to 
			:	maintain the part orientation, this will coordinate the
			:	wrist rotation.
*****************************************************************************/
uint8_t INV_MaintPartOrient_CalcDestWristRotation(float mInitialBase, float mFinalBase, float mCurrentWristRotate, float* mFinalWR)
{
	*mFinalWR = mCurrentWristRotate + (mInitialBase-mFinalBase);

	if (*mFinalWR > MAX_SERVO_5_PW)
	{
		*mFinalWR = MAX_SERVO_5_PW;
		return 0;
	}
	if (*mFinalWR < MIN_SERVO_5_PW)
	{
		*mFinalWR = MIN_SERVO_5_PW;
		return 0;
	}
	return 1;
}



/*****************************************************************************
Name		:   INV_Actuate_Position()
Parameters	:	X,Y,Z given in inches.
			:   mApproach in degrees.
			:   Parameters needed for storing.
Return		:   TRUE if arm position can be Reached with the arm 
				FALSE if Unreachable
Description	:   Move the robot arm to the above calculated positions.
				INV_Find_XYZ() must have already been called!
*****************************************************************************/


/*
Radius is constrained by:
	a) Max cannot exceed the length of L1+L2
	b) Given a Height and Radius,
	   We have two equations and 2 unknowns
	 	 Radius = L1*cos(Sh) + L2*cos(Sh+El)
		 Height = L1*sin(Sh) + L2*sin(Sh+El)
	   
		 or
		 	Elbow = asin( (Height - L1*sin(Sh))/L2 ) - Sh;	
*/



/*****************************************************************************
Name		:   INV_Add_XYZ_To_Queue()
Parameters	:	XYZ struct 
Return		:   TRUE if arm position can be Reached with the arm 
				FALSE if Unreachable
Description	:   Convert from XYZ to proper Servo positions.
				Add to queue if reachable.
*****************************************************************************/
uint8_t INV_Add_XYZ_To_Queue(struct stFull_XYZ_Coordinate* XYZ)
{
	struct stServoAnglesSet ServoAngles;
	struct stServoSet ServoSet;
	
	uint8_t Valid = INV_XYZ_To_Angles( XYZ, &ServoAngles );
	if (Valid)
	{
		INV_Angles_To_Servos( &ServoAngles, &ServoSet );
		//AddToQueue( &ServoSet );
	}
	return Valid;
}

/*****************************************************************************
Name		:   INV_Add_XYZ_To_Queue_with_Result()
Parameters	:	XYZ struct 
Return		:   TRUE if arm position can be Reached with the arm 
				FALSE if Unreachable
Description	:   Convert from XYZ to proper Servo positions.
				Add to queue if reachable.
*****************************************************************************/
uint8_t INV_Add_XYZ_To_Queue_with_Result(struct stFull_XYZ_Coordinate* XYZ, struct stServoSet* mServoSet)
{
	struct stServoAnglesSet ServoAngles;
	
	uint8_t Valid = INV_XYZ_To_Angles( XYZ, &ServoAngles );
	if (Valid)
	{
		INV_Angles_To_Servos( &ServoAngles, mServoSet );
		//AddToQueue( mServoSet );
	}
	return Valid;
}
