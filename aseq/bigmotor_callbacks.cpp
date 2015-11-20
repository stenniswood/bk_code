#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "bk_system_defs.h"
#include "can_eid.h"
#include "CAN_Interface.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "packer_motor.h"
#include "motor_vector.h"
#include "can_txbuff.h"
#include "motor.hpp"
#include "vector_file.h"
#include "Appendage.hpp"
#include "robot.hpp"
#include "config_file.h"
#include "bigmotor_callbacks.hpp"
#include "robot.hpp"
#include "teach_pendant.hpp"

word latest_pot = 0;

struct timeval ts;
extern struct timeval start_ts;
word latest_pot31;
word latest_pot32;
word latest_pot33;

extern TeachPendant 	teach_pendant;

BOOL get_appendage_actuator( int* Aindex, int* actuator_index, byte mInstance )
{
	*Aindex = 0;
	for (int a=0; a<robot.limbs.size(); a++)
	{
		*actuator_index = robot.limbs[*Aindex]->find_actuator_instance( mInstance );
		if (*actuator_index != -1)
			return TRUE;
	}	
	return FALSE;			// not found
}


BOOL can_position_test_responder( struct sCAN* mMsg )
{
	struct stMotorStateInfo tmp;
	/* Is the CAN msg a MOTOR ANGLE? */
	if ( id_match( mMsg->id, create_CAN_eid	(ID_MOTOR_VALUE, 0)) )
	{
		// PARSE TELEGRAM : 
		can_parse_motor_value( mMsg, &(tmp.PotValue), 
							   &(tmp.CurrentTimesTen), 
							   (short*)&(tmp.SpeedTimesTen) );

		gettimeofday( &ts, NULL );
		long seconds  = ts.tv_sec  - start_ts.tv_sec;
		long useconds = ts.tv_usec - start_ts.tv_usec;
		float usec = (float)useconds / 1000.0;
		float mtime = ((seconds) + useconds/1000000.0) ;		

		if ((mMsg->id.group.instance == 31) )
			latest_pot31 = tmp.PotValue;
		
		if ((mMsg->id.group.instance == 32) )
			latest_pot32 = tmp.PotValue;
		if ((mMsg->id.group.instance == 33) )
		{
			latest_pot33 = tmp.PotValue;
			printf("Enc/Pot:, %5d, %5d, %5d, %4.6f\n", latest_pot31, latest_pot32, 
								latest_pot33, mtime );
			robot.limbs[0]->Reads++;
		}
	}	
}


/************************************************************
INCOMING CAN BIGMOTOR MESSAGE CALLBACK(): 
	Call back from within the CAN ISR().  
	Established in seq_main.c : init_interrupts() 
		set_model_rx_callback( can_motor_position_responder )		
************************************************************/
BOOL can_motor_position_responder( struct sCAN* mMsg )
{
	// Distribute the message to the robot:
	BOOL retval = robot.handle_CAN_message( mMsg );		// limbs, then actuators, etc.	
	if (retval==FALSE)
		retval = teach_pendant.handle_CAN_message( mMsg );

// 	When all actuators on a particular limb have been received,
//		if (robot.limbs[Aindex].vector_fully_read()) 
//		{
//			can_vector_read_complete_responder( Aindex );	// Moves to next vector!
//		}
	return retval;
}

/*********************************************************************
 When all of the motors have been updated with a new Position...
 	ie. 3 motors ==> 3 actuators 
 RETURN:  none
 *********************************************************************/
BOOL can_vector_read_complete_responder( byte mAppendageIndex )
{		
	robot.limbs[mAppendageIndex]->ElementsFilled = 0;
	robot.limbs[mAppendageIndex]->Reads++;	
	
	BOOL reached = robot.limbs[mAppendageIndex]->is_destination_reached( );
	if (reached)
	{
			//print_current_positions( mAppendageIndex );
			//printf( "Destn %d reached!\n", Current_Vindex );
			// Stop all motors on this appendage.
			//for (int i=0; i<robot.limbs[mAppendageIndex].actuators.size(); i++)
			//	robot.limbs[mAppendageIndex].actuators[i].SpeedTimesTen = 0;
	}
	//printf( "APPENDAGE READ COMPLETE %d!\n", 	robot.limbs[mAppendageIndex].Reads );
	//robot.limbs[mAppendageIndex].compute_speeds( ); Not Needed! automatically done in next:
	return TRUE;
}


/*BOOL can_position_meas_responder( struct sCAN* mMsg )
{
	// Distribute the message to all limbs of the robot:
	// (the limbs will distribute to each actuator).
	robot.handle_CAN_message( mMsg );
	// Is the CAN msg a MOTOR ANGLE? 
	if ( id_match( mMsg->id, create_CAN_eid	(ID_MOTOR_VALUE, 0)) )
	{
		//if (robot.limbs[Aindex].vector_fully_read()) 			// checks elementsFilled
		{
			//printf("Measurements:, \n" );			
			//robot.limbs[0].Reads++;
		}
	}	
}*/

	/* Is the CAN msg a MOTOR ANGLE? */
/*	if ( id_match( mMsg->id, create_CAN_eid	(ID_MOTOR_VALUE, 0)) )
	{
		// So that no spurious movements occur before we know our requested destination.
		//   This is done by ActiveOutputs now.

		if (FirstIssued==false)
			for (int l=0; l<robot.limbs.size(); l++)
				robot.limbs[l].set_current_position_as_destination( );
*/
