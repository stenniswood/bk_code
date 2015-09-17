/**** MotorPack is an equivalent cpp class ****

	We may choose to setup several appendages for the robot.
	ie. LeftLeg, Right Leg, LeftArm, Right Arm
	This class will manage 1 such appendage.

	how many vectors and how many boards in each depends on the application.
	So how can we make this dynamically changeable?
	
*/
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <vector>
#include <string.h>
#include <string>
#include "bk_system_defs.h"
#include "can_eid.h"
#include "CAN_Interface.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "vector_file.h"
#include "motor.hpp"
#include "Appendage.hpp"
#include "robot.hpp"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "packer_motor.h"
#include "motor_vector.h"
#include "can_txbuff.h"
#include "config_file.h"


Appendage::Appendage( )
{	
	Initialize();
}
	
Appendage::Appendage( int mActuators )
{	
	Initialize();
}

void Appendage::Initialize()
{
	Reads = 0;				
	ReadsAllowed = 0;		
	ElementsFilled = 0;		
	Enable = true;
	strcpy (Name, "noname");
}

/*********************************************************************
 Scan one of the Left & Right Leg Appendages to find the 
	one containing the instance number.  
INPUT :	 mAppendageIndex	- Element of the Appendages array.
	  :  mInstance  		- Instance number to find.
RETURN:  Index within the Appendages[].actuators[__] which matches.  
		 -1 for no match
 *********************************************************************/
int Appendage::find_actuator_instance( byte mInstance )
{
	if (Enable==false)  return -1;
	for (byte a=0; a < actuators.size(); a++)
	{
		if ( actuators[a].Instance == mInstance )
			return a;
	}
	return -1;	
}

void Appendage::update_submitted_timestamps( struct timeval mts )
{
	if (Enable==false)  return;
	for (byte a=0; a < actuators.size(); a++)
		actuators[a].submitted = mts;
}

bool Appendage::is_done_averaging( )
{
	bool retval = false;
	for (byte a=0; a < actuators.size(); a++)
	{
		bool result = ( actuators[a].Reads <= actuators[a].ReadsAllowed );		
		if (result)
			return false;		
	}
	return true;
}

void Appendage::start_measurement_averaging( int mNumSamples )
{
	Reads = 0;
	ReadsAllowed = mNumSamples;

	for (byte a=0; a < actuators.size(); a++)
	{
		actuators[a].SumCurrCounts = 0;
		actuators[a].Reads = 0;
		actuators[a].ReadsAllowed = mNumSamples;
	}
}


const int HIP_ROTATE_INDEX 	 = 9;
const int HIP_FA_SWING_INDEX = 0;
const int KNEE_SWING_INDEX   = 1;
const int ANKLE_SWING_INDEX  = 2;

// from highest to lowest actuator.	( for swing leg )
/* Measurements come from torso gyro. 
mTorso_fa_Angle - should be measured from horizon (=0.0 deg)
mTorso_io_Angle - should be measured from horizon (=0.0 deg)
*/
void Appendage::propogate_gravity_angle_down( float mTorso_fa_Angle, float mTorso_io_Angle )	
{
	// Almost always propogate down the legs, b/c we have the gyros in the torso.
	actuators[HIP_FA_SWING_INDEX].gravity_angle = mTorso_fa_Angle;
	
	/* Note: For now we don't have any Hip Rotate motor.  This angle means we would have
			to use the _io_ angle also!  Add this later after we know the algorithm works
			without side to side tipping.			 
	*/
	float angle = actuators[HIP_FA_SWING_INDEX].gravity_angle + actuators[HIP_FA_SWING_INDEX].CurrAngle;

	actuators[KNEE_SWING_INDEX].gravity_angle  = angle;
	angle = actuators[KNEE_SWING_INDEX].gravity_angle + actuators[KNEE_SWING_INDEX].CurrAngle;	
	actuators[ANKLE_SWING_INDEX].gravity_angle = angle;	
}
	
/*********************************************************************

*********************************************************************/
BOOL Appendage::is_destination_reached( )
{
	if (Enable==false)  return TRUE;

	// When PotValues equal Vector values (within 10%) for all actuators.
	BOOL result = TRUE;
	// For all actuators : 
	for (int a=0; a<actuators.size(); a++)
	{
		bool reached = actuators[a].destination_reached( 20. );
		if (reached==false)
			result = FALSE;
	}	
	return result;
}

void Appendage::clear_reads( int mNumExpected )
{
	Reads = 0;	
	ReadsAllowed = mNumExpected;
	for (int a=0; a<actuators.size(); a++)  {
		actuators[a].Reads = 0;
		actuators[a].ReadsAllowed = mNumExpected;
	}
}

// at least 1 update on all enabled motors.
bool Appendage::is_vector_fully_read( )
{
	if (Enable==false)  return true;

	for (int a=0; a<actuators.size(); a++)
		if (actuators[a].MotorEnable)
			if (actuators[a].Reads < actuators[a].ReadsAllowed)
				return false;

	return 	true;
}

void Appendage::deactivate_outputs( )
{
	for (int a=0; a<actuators.size(); a++)
		actuators[a].ActiveOutputs = FALSE;
}
void Appendage::activate_outputs( )
{
	for (int a=0; a<actuators.size(); a++)
		actuators[a].ActiveOutputs = TRUE;
}
void Appendage::activate_enabled_outputs( )
{
	for (int a=0; a<actuators.size(); a++)
		actuators[a].ActiveOutputs = actuators[a].MotorEnable;
}
	
// handles incoming msg
int Appendage::handle_CAN_message( struct sCAN* mMsg )
{
	if (Enable==false) return 0;
	
	int handled = 0;
	for (int a=0; a<actuators.size(); a++)
	{
		handled = actuators[a].handle_CAN_message( mMsg );
	}	

	// Special Action to be done when all actuators have received an updated position.	
	if (is_vector_fully_read())  {
		Reads++;
		//printf("VECTOR FULLY READ!!!!\n");
	}
}

void Appendage::print_current_angles(  )
{
	if (Enable==false)  return ;
	
	printf("angles: ");
	for (int a=0; a<actuators.size(); a++)
	{
		printf(" %6.2f ", actuators[a].CurrAngle );
	}
	printf("\n");	
}

void Appendage::print_current_positions(  )
{
	if (Enable==false)  return;	
	//printf("counts: ");
	for (int a=0; a < actuators.size(); a++)
	{
		actuators[a].print_positioning();
	}
	printf("\n");	
}

void Appendage::print_averages( )
{
	printf("Limb: %s\n", Name );
	for (int a=0; a < actuators.size(); a++)
	{
		actuators[a].print_average();
	}	
}

void Appendage::set_new_destinations( struct sVectorSet& mVectors, int mVectorIndex )
{
	if (Enable==false)  return ;
	for (int a=0; a<actuators.size(); a++)
	{
		// GET COUNT & CALC REQUESTED SPEED : 
		int tmp = mVectors.vectors[mVectorIndex].get_count( a );
		float speed = mVectors.calc_average_speed_cps( mVectorIndex, a );
		if (speed>MAX_MOTOR_SPEED)
			speed = MAX_MOTOR_SPEED;
		//printf("get_count()=%d\n", tmp);
		actuators[a].set_destination( tmp, speed );
	}
}

/*********************************************************************
Send Motor_Speed commands to each instance in the vector.
INPUT :	 speeds will be in the Actuator 
RETURN:  none 
 *********************************************************************/
void Appendage::send_speed_messages( )
{
	if (Enable==false)  return ;
	for (int a=0; a<actuators.size(); a++)
	{
		// speeds are in the actuator - "DutyPercent"
		// This also computes the speed!
		actuators[a].send_speed_pid();
	}
}

/*********************************************************************
Send Motor_Speed commands to each instance in the vector.
INPUT :	 speeds will be in the Actuator ()
RETURN:  none 
 *********************************************************************/
void Appendage::compute_speeds( )
{
	if (Enable==false)  return ;
	for (int a=0; a<actuators.size(); a++ )
	{
		// speeds are in the actuator - ""
		actuators[a].compute_duty( );
	}
}

/*************************************************************************
  The vector will be mapped according to the MappedID in the board list 
  ie. find MappedID=0, retrieve instance for packing.
  The board list must have a sequential set of MappedID numbers.
  MappedID is the Vector index and the specific instance will be retrieved 
  from the list.
 INPUT :	mVectIndex	- Element of the Appendages array
 RETURN:  	none 
 *************************************************************************/
void Appendage::send_moveto_angle_messages(  )
{
	if (Enable==false)  return ;
	for (int a=0; a < actuators.size(); a++ )
	{
		printf("Instances=%x; NextAngle=%x RequestedSpeed=%3.1f\n", 
			actuators[a].Instance, 
			actuators[a].NextAngleDeg,
			actuators[a].RequestedSpeed  );
		actuators[a].send_moveto_angle( actuators[a].NextAngleDeg );		
	}
}

// should stop all motors.
void Appendage::set_current_position_as_destination( )
{
	if (Enable==false)  return ;
	for (int a=0; a<actuators.size(); a++)
	{
		actuators[a].StartCount = actuators[a].CurrCount;
		actuators[a].DestinationCount = actuators[a].CurrCount;
	}
}

void Appendage::update_position( )
{

}

/************************************************************************* 
Configure all BigMotor boards associated with this motor vector.
INPUT:
	mRate		: 
	mReports	: Which reports to send (VALUE, ANGLE, etc)
return 			: void
*************************************************************************/
void Appendage::configure_motor_reports( byte mRate, byte mReports )
{
	struct sCAN Msg;
	if (Enable==false)  return ;
	for (int a=0; a<actuators.size(); a++)
	{
		configure_motor_set_update_rate( &Msg, actuators[a].Instance, 
										 mRate, mReports );
		print_message( &Msg );
		AddToSendList( &Msg );
	}
}




