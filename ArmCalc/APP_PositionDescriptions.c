/*********************************************************************
 *											   						 *
 *    DESCRIPTION:  Common positions (Hardcoded Description based)	 *
 *											  						 *
 *	  CATEGORY	:  APP										 		 *
 *											  						 *
 *    AUTHOR	:  Stephen Tenniswood, Embedded Engineer 	 		 *
 *											   						 *
 *    PURPOSE	:  Routines to convert Common Arm positions to 		 *
 *				   Angles then to positions.  Positions should be 	 *
 *				   independant of calibrations						 *
 *																	 *
 *	  DATE		:  Dec 6,2008										 *
 *********************************************************************/
#include <math.h>
#include "skp_bsp.h"
#include "DEV_OCpwm.h"
#include "MID_FilterServo.h"
#include "APP_ArmPositionStorage.h"
#include "APP_CoordinateTransform.h"
#include "APP_InverseCoordinates.h"

/*********************************************************************************/
/*************** HARD CODED POSITIONS ********************************************/
/*********************************************************************************/
#define GRIPPER_FULL_OPEN 	MAX_SERVO_6_PW
#define GRIPPER_FULL_CLOSED 	MIN_SERVO_6_PW



/**********************************************************************************
Name		:   POS_StraightUp()
Parameters	:	none
Returns 	:   none
Description	:   Place the arm straight up in the air.
************************************************************************************/
void POS_StraightUp()
{
	U16 tmpServo;
	float radians = CT_Convert_Degrees_to_Radians( 90.0 );
	
	tmpServo = CT_Convert_Radians_to_Servo( radians, SHOULDER_SERVO_NUM	);
	SB_Servo_1_Set_Position( &tmpServo );
	
	tmpServo = CT_Convert_Radians_to_Servo( radians, ELBOW_SERVO_NUM 	);
	SB_Servo_2_Set_Position( &tmpServo );
	
	tmpServo = CT_Convert_Radians_to_Servo( radians, WRIST_SERVO_NUM 	);
	SB_Servo_3_Set_Position( &tmpServo );
	
}

/**********************************************************************************
Name		:   POS_StraightHorizontal()
Parameters	:	none
Returns 	:   none
Description	:   Place the arm straight up in the air.
Independant :   Base, WristRotate, Gripper.
************************************************************************************/
void POS_StraightHorizontal()
{
	U16 tmpServo;
	
	tmpServo = CT_Convert_Radians_to_Servo( 0, SHOULDER_SERVO_NUM	);
	SB_Servo_1_Set_Position( &tmpServo );
	
	tmpServo = CT_Convert_Radians_to_Servo( 0, ELBOW_SERVO_NUM 	);
	SB_Servo_2_Set_Position( &tmpServo );
	
	tmpServo = CT_Convert_Radians_to_Servo( 0, WRIST_SERVO_NUM 	);
	SB_Servo_3_Set_Position( &tmpServo );
}

/**********************************************************************************
Name		:   POS_Rest()
Parameters	:	none
Returns 	:   none
Description	:   Place the arm straight up in the air.
Independant :   Base, WristRotate, Gripper.
************************************************************************************/
void POS_Rest()
{
	U16 tmpServo;
	float radians = CT_Convert_Degrees_to_Radians( 180.0 );
	
	tmpServo = CT_Convert_Radians_to_Servo( radians, SHOULDER_SERVO_NUM	);
	SB_Servo_1_Set_Position( &tmpServo );
	
	radians = CT_Convert_Degrees_to_Radians( -135.0 );
	tmpServo = CT_Convert_Radians_to_Servo( radians, ELBOW_SERVO_NUM 	);
	SB_Servo_2_Set_Position( &tmpServo );
	
	radians = CT_Convert_Degrees_to_Radians( -90.0 );
	tmpServo = CT_Convert_Radians_to_Servo( radians, WRIST_SERVO_NUM 	);
	SB_Servo_3_Set_Position( &tmpServo );
}


/**********************************************************************************
Name		:   POS_GripperOpen()
Parameters	:	none
Returns 	:   none
Description	:   Open the gripper, don't change any other servo!
Independant :   Base, WristRotate, Gripper.
************************************************************************************/
U8 POS_GripperOpen( struct stServoSet* mServoSet )
{
	U8 Valid;
	// OPEN GRIPPER:
	mServoSet->s6 = GRIPPER_FULL_OPEN;
	Valid = AddToQueue( mServoSet );
	if (!Valid)
		return TRUE;
	
	return FALSE;
}

/**********************************************************************************
Name		:   POS_GripperClosed()
Parameters	:	none
Returns 	:   none
Description	:   Need a pressure sensor for this!! To prevent breaking the servo's
			:   gears.
************************************************************************************/
U8 POS_GripperClosed( struct stServoSet* mServoSet )
{
	U8 Valid;
	// CLOSE GRIPPER:
	mServoSet->s6 = GRIPPER_FULL_CLOSED;
	Valid = AddToQueue( mServoSet );
	if (!Valid)
		return TRUE;
		
	return FALSE;
}

/*********************************************************************************/
/*************** HARD CODED APPROACHES ********************************************/
/*********************************************************************************/

/**********************************************************************************
Name		:   POS_FromTop_Approach()
Parameters	:	none
Returns 	:   none
Description	:   Place the arm straight up in the air.
************************************************************************************/
void POS_FromTop_Approach(float mX, float mY, float mZ)
{
	struct stFull_XYZ_Coordinate XYZ;
	U8 Valid;

	XYZ.X = mX;
	XYZ.Y = mY;
	XYZ.Z = mZ;
	XYZ.Approach = -PI/2;
	XYZ.wrist_rotate = 0;

	Valid = INV_Add_XYZ_To_Queue( &XYZ );
}

/**********************************************************************************
Name		:   POS_FromBottom_Approach()
Parameters	:	none
Returns 	:   none
Description	:   Place the arm straight up in the air.
************************************************************************************/
void POS_FromBottom_Approach(float mX, float mY, float mZ)
{
	struct stFull_XYZ_Coordinate XYZ;
	U8 Valid;

	XYZ.X = mX;
	XYZ.Y = mY;
	XYZ.Z = mZ;
	XYZ.Approach = +PI/2;
	XYZ.wrist_rotate = 0;

	Valid = INV_Add_XYZ_To_Queue( &XYZ );
}

/**********************************************************************************
Name		:   POS_FromFront_Approach()
Parameters	:	none
Returns 	:   none
Description	:   Place the arm straight up in the air.
************************************************************************************/
void POS_FromFront_Approach(float mX, float mY, float mZ)
{
	struct stFull_XYZ_Coordinate XYZ;
	U8 Valid;

	XYZ.X = mX;
	XYZ.Y = mY;
	XYZ.Z = mZ;
	XYZ.Approach = 0;
	XYZ.wrist_rotate = 0;
	
	Valid = INV_Add_XYZ_To_Queue( &XYZ );
	
}


/**********************************************************************************
Name		:   POS_FromFront_Approach()
Parameters	:	none
Returns 	:   >0 if error.
Description	:   a) Place the arm over the XY coordinate as high as possible.  
			:	b) Open gripper
			:   c) Lower to Z
			:   d) Closer gripper
************************************************************************************/
U8 POS_GrabFromTop(float mX, float mY, float mZ)
{
	struct stServoSet ServoSet;
	float Z = INV_FindHighestAbove_XY(mX,mY);
	struct stFull_XYZ_Coordinate XYZ;
	U8 Valid;

	// OVER ABOVE OBJECT:
	XYZ.X = mX;
	XYZ.Y = mY;
	XYZ.Z = Z;
	XYZ.Approach = 0;
	XYZ.wrist_rotate = 0;
	
	Valid = INV_Add_XYZ_To_Queue_with_Result( &XYZ, &ServoSet );
	if (!Valid)
		return TRUE;
	
	// OPEN GRIPPER:
	ServoSet.s6 = GRIPPER_FULL_OPEN;
	Valid = AddToQueue( &ServoSet );
	if (!Valid)
		return TRUE;
		
	// MOVE DOWN TO OBJECT:
	XYZ.Z = mZ;
	Valid = INV_Add_XYZ_To_Queue_with_Result( &XYZ, &ServoSet );
	if (!Valid)
		return TRUE;
		
	// CLOSE GRIPPER:
	ServoSet.s6 = GRIPPER_FULL_CLOSED;		// In MID_FilterServo.c the force sensor check should be done
											// And servo s6 stopped when desired tensions is reached.
	Valid = AddToQueue( &ServoSet );
	if (!Valid)
		return TRUE;
	
	return 0;
}

/**********************************************************************************
Name		:   POS_FromTop_Approach()
Parameters	:	none
Returns 	:   none
Description	:   Once a part has been grasped - ie. prior to this call an XYZ from 
				top has been specified and the gripper has been closed.
				Now we specify a new XYZ.  The gripper therefore is already 
				angled straight down and the orientation of the part will stay the 
				same.  To do this we turn the wrist rotate exactly 1 degree for 
				each and every turn of the base.
************************************************************************************/
void CMOVE_FromTop_Maintain_PartOrientation(float mX, float mY, float mZ)
{
	// 
	//INV_XYZ_To_Angles( mX, mY, mZ, -90 );
	
	
}


/**********************************************************************************
Name		:   POS_TimeSlice()
Parameters	:	none
Returns 	:   none
Description	:   a) Place the arm over the XY coordinate as high as possible.  
			:	b) Open gripper
			:   c) Lower to Z
			:   d) Closer gripper
			:
			:	This will command a servo position.  Wait for it's completion.
			:   then command the next move.
************************************************************************************/
void CMOVE_TimeSlice()
{
	// keep calling FS_
	
}

/* 
*   corner brewery   720 Norris St Ypsilanti 48198 
*   7pm
*/
