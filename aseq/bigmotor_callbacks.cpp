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

word latest_pot = 0;

struct timeval ts;
extern struct timeval start_ts;
word latest_pot31;
word latest_pot32;
word latest_pot33;
	

BOOL get_appendage_actuator( int* Aindex, int* actuator_index, byte mInstance )
{
	*Aindex = 0;
	for (int a=0; a<robot.limbs.size(); a++)
	{
		*actuator_index = robot.limbs[*Aindex].find_actuator_instance( mInstance );
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
			robot.limbs[0].Reads++;
		}

	}	
}

BOOL can_position_meas_responder( struct sCAN* mMsg )
{
	struct stMotorStateInfo tmp;
	int Aindex         = 0;
	int actuator_index = 0;
	
	/* Is the CAN msg a MOTOR ANGLE? */
	if ( id_match( mMsg->id, create_CAN_eid	(ID_MOTOR_VALUE, 0)) )
	{
		// WHICH APPENDAGE & WHICH ACTUATOR ?
		BOOL found = get_appendage_actuator( &Aindex, &actuator_index, mMsg->id.group.instance );				
		// RETURN IF NOT ENABLED:
		if (robot.limbs[Aindex].Enable==false)
			return FALSE;
		if (robot.limbs[Aindex].actuators[actuator_index].MotorEnable==FALSE)
			return FALSE;

		// PARSE TELEGRAM : 
		can_parse_motor_value( mMsg, &(tmp.PotValue), &(tmp.CurrentTimesTen),
							   (short*)&(tmp.SpeedTimesTen) );
		robot.limbs[Aindex].actuators[actuator_index].CurrCount = tmp.PotValue;

		gettimeofday( &ts, NULL );
		long seconds  = ts.tv_sec  - start_ts.tv_sec;
		long useconds = ts.tv_usec - start_ts.tv_usec;
		float usec    = (float)useconds / 1000.0;
		float mtime   = ((seconds) + useconds/1000000.0);

		robot.limbs[Aindex].ElementsFilled |= (1<<actuator_index);
		if (robot.limbs[Aindex].vector_fully_read()) 			// checks elementsFilled
		{
			printf("Measurements:, \n" );			
			for (int a=0; a<robot.limbs[0].actuators.size(); a++)
				printf(" %4d, ", robot.limbs[0].actuators[a].CurrCount );
			robot.limbs[0].Reads++;
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
	int Aindex         = 0;
	int actuator_index = 0;

	/* Is the CAN msg a MOTOR ANGLE? */
	if ( id_match( mMsg->id, create_CAN_eid	(ID_MOTOR_VALUE, 0)) )
	{
		// WHICH APPENDAGE & WHICH ACTUATOR ?
		BOOL found = get_appendage_actuator( &Aindex, &actuator_index, mMsg->id.group.instance );				
		// RETURN IF NOT ENABLED:
		if (robot.limbs[Aindex].Enable==false)
			return FALSE;
		if (robot.limbs[Aindex].actuators[actuator_index].MotorEnable==FALSE)
			return FALSE;
		//printf("ID_MOTOR_VALUE\n");

		if (FirstIssued==false)
			robot.limbs[Aindex].set_current_position_as_destination( );
				
		robot.limbs[Aindex].actuators[actuator_index].update_position( mMsg );

		// Print DEBUG:
/*		if ((mMsg->id.group.instance == 31) || (mMsg->id.group.instance == 32) || (mMsg->id.group.instance == 33))
			printf(" %d PotRead=%d; spd=%5.2f\t", 
					mMsg->id.group.instance, 
					robot.limbs[Aindex].actuators[actuator_index].CurrCount,
					robot.limbs[Aindex].actuators[actuator_index].SpeedTimesTen );*/

		robot.limbs[Aindex].ElementsFilled |= (1<<actuator_index);		// Mark it 
		if (robot.limbs[Aindex].vector_fully_read()) 			// checks elementsFilled
		{
			//printf( "APPENDAGE FULLY READ %d !\n", LimbVectors.size() );
			can_vector_read_complete_responder( Aindex );	// Moves to next vector!
			robot.limbs[Aindex].ElementsFilled = 0;
		}		
		return TRUE;
	}
	else if ( id_match( mMsg->id, create_CAN_eid	(ID_MOTOR_ANGLE, 0)) )
	{	
	}
	else if ( id_match( mMsg->id, create_CAN_eid	(ID_MOTOR_SPEED, 0)) )
	{
		//can_parse_motor_speed( mMsg, &tmp.Angle, &tmp.CurrentTimesTen, (short*)&tmp.SpeedTimesTen );
		return TRUE;
	}
	return FALSE;
}

/*********************************************************************
 When all of the motors have been updated with a new Position...
 	ie. 3 motors ==> 3 actuators 
 RETURN:  none
 *********************************************************************/
BOOL can_vector_read_complete_responder( byte mAppendageIndex )
{		
	robot.limbs[mAppendageIndex].Reads++;	
	
	BOOL reached = robot.limbs[mAppendageIndex].is_destination_reached( );
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


