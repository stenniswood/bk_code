/*  *** Robot class ***

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
#include "motor.hpp"
#include "Appendage.hpp"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "packer_motor.h"
#include "motor_vector.h"
#include "can_txbuff.h"
#include "vector_file.h"
#include "Appendage.hpp"
#include "robot.hpp"
#include "config_file.h"


Robot	robot;


Robot::Robot( )
{	
	Initialize();
}
	
void Robot::Initialize()
{
	Reads = 0;				
	ReadsAllowed = 0;			
	Enable = true;
	strcpy (Name, "Onesimus");
}

// handles incoming msg	
int Robot::handle_CAN_message( struct sCAN* mMsg )	
{
	for (byte l=0; l < limbs.size(); l++)
	{
		limbs[l].handle_CAN_message( mMsg );
	}	
}

/*********************************************************************
 Scan one of the Left & Right Leg Appendages to find the 
	one containing the instance number.  
INPUT :	 mAppendageIndex	- Element of the Appendages array.
	  :  mInstance  		- Instance number to find.
RETURN:  Index within the Appendages[].actuators[__] which matches.  
		 -1 for no match
 *********************************************************************/
int Robot::find_actuator_by_instance( byte mInstance, int* Aindex, int* actuator_index )
{	
	if (Enable==false)  return -1;
	for (*Aindex=0; *Aindex < limbs.size(); (*Aindex)++)
	{
		*actuator_index = limbs[*Aindex].find_actuator_instance( mInstance );
		if (*actuator_index != -1)
			return TRUE;
	}
	return -1;	
}

void Robot::update_submitted_timestamps( struct timeval mts )
{
	if (Enable==false)  return;
	for (byte l=0; l < limbs.size(); l++)
	{
		limbs[l].update_submitted_timestamps( mts );
	}		
}

/*********************************************************************
When all limbs are at their destinations.
*********************************************************************/
BOOL Robot::is_destination_reached( )
{
	if (Enable==false)  return TRUE;

	// When PotValues equal Vector values (within 10%) for all actuators.
	BOOL result = TRUE;
	// For all actuators : 
	for (int l=0; l<limbs.size(); l++)
	{
		bool reached = limbs[l].is_destination_reached( );
		if (reached==false)
			result = FALSE;
	}	
	return result;
}

void Robot::set_vectors_limbs(  )
{
	seq.set_limbs( *this );
}

void Robot::print_current_positions(  )
{
	if (Enable==false)  return ;
	
	printf("Robot positions : \n");
	for (int l=0; l < limbs.size(); l++)
	{
		if (limbs[l].Enable)
		{
			printf("%d Limb[%d] : \n", seq.Current_Vindex, l );
			limbs[l].print_current_positions();
		}
	}
	printf("\n");
}

void Robot::print_vector( int mIndex, bool mAngles )
{
	if (mIndex == -1)  mIndex = seq.Current_Vindex;

	printf("Vector[%d] \n", mIndex );
	if (mAngles)
		for (int l=0; l<limbs.size(); l++)		
		{	if (limbs[l].Enable) {
				printf("Limb[%d] ", l);
				seq.limbs[l].vectors[mIndex].print_vector( );
			}
		}
	else
		for (int l=0; l<limbs.size(); l++)
		{	if (limbs[l].Enable) {
				printf("Limb[%d] ", l);
				seq.limbs[l].vectors[mIndex].print_counts( );	
			}
		}
}

void Robot::next_vector( )
{
	seq.Current_Vindex++;
	// Update to New Vector index : 
	if ( seq.Current_Vindex >= seq.limbs[0].vectors.size() )
	{		
			seq.Current_Vindex      =0;
			seq.iterations_completed++;
	}
}

void Robot::set_new_destinations(  )
{
	if (Enable==false)  return ;
	for (int l=0; l<limbs.size(); l++)
	{
		limbs[l].set_new_destinations( seq.limbs[l], seq.Current_Vindex );
	}
}

/*********************************************************************
Send Motor_Speed commands to each instance in the vector.
INPUT :	 speeds will be in the Actuator (SpeedTimesTen)
RETURN:  none 
 *********************************************************************/
void Robot::send_speed_messages( )
{
	if (Enable==false)  return ;
	for (int l=0; l<limbs.size(); l++)
	{
		// speeds are in the actuator - "SpeedTimesTen"
		// This also computes the speed!
		limbs[l].send_speed_messages();
	}
}

/*********************************************************************
Send Motor_Speed commands to each instance in the vector.
INPUT :	 speeds will be in the Actuator (SpeedTimesTen)
RETURN:  none 
 *********************************************************************/
void Robot::compute_speeds( )
{
	if (Enable==false)  return ;
	for (int l=0; l<limbs.size(); l++ )
	{
		// speeds are in the actuator - "SpeedTimesTen"
		limbs[l].compute_speeds( );
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
void Robot::send_moveto_angle_messages(  )
{
	if (Enable==false)  return ;
	for (int l=0; l < limbs.size(); l++ )
	{
		limbs[l].send_moveto_angle_messages( );		
	}
}

// should stop all motors.
void Robot::set_current_position_as_destination( )
{
	if (Enable==false)  return ;
	for (int l=0; l<limbs.size(); l++)
	{
		limbs[l].set_current_position_as_destination( );
	}
}

/************************************************************************* 
Configure all BigMotor boards associated with this motor vector.
INPUT:
	mRate		: 
	mReports	: Which reports to send (VALUE, ANGLE, etc)
return 			: void
*************************************************************************/
void Robot::configure_motor_reports( byte mRate, byte mReports )
{
	if (Enable==false)  return ;
	for (int l=0; l<limbs.size(); l++)
	{
		limbs[l].configure_motor_reports( mRate, mReports );
	}
}




