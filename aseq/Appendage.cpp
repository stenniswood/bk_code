/*  ***MotorPack is an equivalent cpp class***

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

// at least 1 update on all enabled motors.
bool Appendage::vector_fully_read( )
{
	if (Enable==false)  return true;
	
	byte bitfield = 0;
	// What should it look like? :
	for (int i=0; i<actuators.size(); i++)
		if (actuators[i].MotorEnable)
			bitfield |= (1<<i);

	// Compared to what it looks like : 
	if (ElementsFilled == bitfield)
		return 	true;
	return false;	
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

void Appendage::set_new_destinations( struct sVectorSet& mVectors, int mVectorIndex )
{
	if (Enable==false)  return ;
	for (int a=0; a<actuators.size(); a++)
	{
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




