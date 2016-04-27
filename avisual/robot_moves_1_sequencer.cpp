#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "CAN_Interface.h"
#include "CAN_memory.h"
#include "fuse_ag.h"
#include "fuse_accel.h"
#include "packer_motor.h"
#include "vector_math.h"
#include "balance.hpp"
#include "robot_moves_1_sequencer.hpp"



bool algorithm_running = false;

bool  start_bow = false;		// Set to true to begin the sequence
float bow_angle = 0.0;			// Set to the desired accelerometer position.

bool  start_squat = false;		// Set to true to begin the sequence
float squat_angle = 0.0;		// Set to the desired accelerometer position.
static struct sCAN msg;

	
void do_squat( )
{
	static int state=-1;
	const int ldir = +1;			// Backward:  L,R = + -
	const int rdir = -1;			// Foreward:  L,R = - +	

	float duty = 10;
	bool  fuzzy_compare;
	switch (state)
	{
	case -1: /* idle */  
		if (start_squat)  {	
			state++;			
			start_squat = false;
		}
		break;
	case 0 :
		algorithm_running = true;
		printf("Perform Squat :  squat_angle=%6.2f  duty=%6.2f \n", bow_angle, duty );
		duty = 10;
		// MOVE HIPS (1x)
		pack_move_speed			( &msg, Hip_instanceL, ldir*duty );
		shm_add_can_tx_message	( &msg );	
		pack_move_speed			( &msg, Hip_instanceR, rdir*duty );
		shm_add_can_tx_message	( &msg );
		// MOVE KNEES (2x)
		pack_move_speed			( &msg, Knee_instanceL, ldir*duty*2 );
		shm_add_can_tx_message	( &msg );	
		pack_move_speed			( &msg, Knee_instanceR, rdir*duty*2 );
		shm_add_can_tx_message	( &msg );
		// MOVE ANKLES (2x)
		pack_move_speed			( &msg, Ankle_instanceL, ldir*duty );
		shm_add_can_tx_message	( &msg );	
		pack_move_speed			( &msg, Ankle_instanceR, rdir*duty );
		shm_add_can_tx_message	( &msg );
		state++;
		break;
	case 1 :
		/* Wait until desired Tilt is reached.  */
		/* 		Msgs are captured in amon and posted to CAN_memory. 
				These are retrieved and proc'd in background (visual_main: can_interface)
				
				This is same thread as the tilt processing.  So we can't loop here.
					Just a spot check and must return;
		*/
		printf("Tilt angle = %6.2f \n", BestAngles.rx );
		fuzzy_compare = fuzzy_not_to_exceed( BestAngles.rx, bow_angle, 5. );
		if (fuzzy_compare)
			state++;
		break;
	case 2 :
		printf("Reverse Squat :  squat_angle=%6.2f  duty=%6.2f \n", bow_angle, duty );
		duty = -10;
		// MOVE HIPS (1x)
		pack_move_speed			( &msg, Hip_instanceL, ldir*duty );
		shm_add_can_tx_message	( &msg );	
		pack_move_speed			( &msg, Hip_instanceR, rdir*duty );
		shm_add_can_tx_message	( &msg );
		// MOVE KNEES (2x)
		pack_move_speed			( &msg, Knee_instanceL, ldir*duty*2 );
		shm_add_can_tx_message	( &msg );	
		pack_move_speed			( &msg, Knee_instanceR, rdir*duty*2 );
		shm_add_can_tx_message	( &msg );
		// MOVE ANKLES (2x)
		pack_move_speed			( &msg, Ankle_instanceL, ldir*duty );
		shm_add_can_tx_message	( &msg );	
		pack_move_speed			( &msg, Ankle_instanceR, rdir*duty );
		shm_add_can_tx_message	( &msg );
		state++;
		break;
	case 3 :
		/* Wait until desired Tilt is reached.  */
		/* 		Msgs are captured in amon and posted to CAN_memory. 
				These are retrieved and proc'd in background (visual_main: can_interface)
				
				This is same thread as the tilt processing.  So we can't loop here.
					Just a spot check and must return;
		*/
		printf("Tilt angle = %6.2f \n", BestAngles.rx );
		fuzzy_compare = fuzzy_not_to_exceed( BestAngles.rx, bow_angle, 5. );
		if (fuzzy_compare)
			state++;
		break;
	case 4 :
		algorithm_running = false;
		state = -1;
		break;
	default: state = 0;  break;
	}
}


void do_bow( )
{
	static int state=-1;
	static float Direction = 1;
	// Def for Backward:
	const int ldir = +1;			// Backward:  L,R = + -
	const int rdir = -1;			// Foreward:  L,R = - +

	float duty = 10;
	bool fuzzy_compare;
	switch (state)
	{
	case -1: /* idle */  
		if (start_bow)  {	
			state++;
			start_bow = false;
		}
		break;
	case 0 :
		algorithm_running = true;	
		Direction = ( bow_angle < 0 ) ? +1:-1;
		duty = Direction * duty;
		printf("Perform Bow :  bow_angle=%6.2f  duty=%6.2f \n", bow_angle, duty );
		pack_move_speed( &msg, Hip_instanceL, ldir*duty );
		shm_add_can_tx_message( &msg );	
		pack_move_speed( &msg, Hip_instanceR, rdir*duty );
		shm_add_can_tx_message( &msg );
		state++;
		break;
	case 1 :
		/* Wait until desired Tilt is reached.  */
		/* 		Msgs are captured in amon and posted to CAN_memory. 
				These are retrieved and proc'd in background (visual_main: can_interface)
				
				This is same thread as the tilt processing.  So we can't loop here.
					Just a spot check and must return;
					
				+ degrees is backward tilt of robot.
				- degrees is foreward tilt of robot.
		*/
		//printf("%6.2f ", -BestAngles.rx );
		if (bow_angle<0)
		{
			fuzzy_compare = fuzzy_less_than_or_equal_to( -BestAngles.rx, bow_angle, 5. );		
		} else {
			fuzzy_compare = fuzzy_not_to_exceed( -BestAngles.rx, bow_angle, 5. );		
		}
		if (fuzzy_compare) {
			state++;
			printf("Reached Fuzzy end point (%6.2f) = %6.2f \n", bow_angle, -BestAngles.rx );
		}
		break;
	case 2 : 	
		printf("Restore Bow to stand:  duty=%6.2f \n", duty);
		duty *= -Direction;
		pack_move_speed( &msg, Hip_instanceL, ldir*duty );
		shm_add_can_tx_message( &msg );	
		pack_move_speed( &msg, Hip_instanceR, rdir*duty );
		shm_add_can_tx_message( &msg );
		state++;
		break;
	case 3 :
		/* Wait until Tilt is back to near zero.  */
		//printf("Tilt %6.2f\n", -BestAngles.rx );
		//printf("%6.2f ", -BestAngles.rx );		
		if (bow_angle<0)		// which direction are we approaching from?
		{
			fuzzy_compare = fuzzy_not_to_exceed( -BestAngles.rx, 0., 5. );		
		} else {
			fuzzy_compare = fuzzy_less_than_or_equal_to( -BestAngles.rx, 0., 5. );		
		}
		if (fuzzy_compare) {
			state++;
			printf("Reached Fuzzy end point2 (%6.2f) = %6.2f\n", 0.0, -BestAngles.rx );
		}
		break;
	case 4 : 	/* stop */
		printf("Stop Bow, at stand.\n", Hip_instanceL, duty );
		duty = 0;
		pack_move_speed			( &msg, Hip_instanceL, 0.0 );
		shm_add_can_tx_message	( &msg );	
		pack_move_speed			( &msg, Hip_instanceR, 0.0 );
		shm_add_can_tx_message	( &msg );
		state = -1;		// go back to idle.
		start_bow = false;
		algorithm_running = false;
		break;
	default: state = 0;  break;
	}
}

void* bow_now_thread( void* param )
{
	while(1) {
		do_bow();
		usleep(2000);
		//do_squat();
	}
	return NULL;
};
