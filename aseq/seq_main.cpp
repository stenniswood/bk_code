#include <string.h>
#include <errno.h>
#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <vector>
#include <pthread.h>
#include <assert.h> 
#include <string>

#include "gpio.h"
#include "mcp2515.h"
#include "pican_defines.h"
#include "gpio.h"
#include "CAN_Interface.hpp"
#include "packer_lcd.h"
#include "can_txbuff.h"
#include "packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "motor.hpp"
#include "motor_control.hpp"
#include "motor_vector.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "OS_Dispatch.h"
#include "OS_timers.h"
#include "Appendage.hpp"
#include "bigmotor_callbacks.hpp"
#include "timer.h"
#include "vector_file.h"
#include "robot.hpp"
#include "teach_pendant.hpp"
#include "seq_init.hpp"


#define fifty_ms    50000000

int counter = 0;
//char user_input[255];
//char cmd[4];
//byte instance;
//byte value;
bool FirstIssued = false;
struct timeval	start_ts;
struct timeval tv;
struct timeval starttime;




/* 
	Here we marry a vector in the sequencer with the robot limbs.
*/
void next_sequence_handler(int sig, siginfo_t *si, void *uc)
{
	if (FirstIssued==false) {
		FirstIssued = true;		
		robot.activate_enabled_outputs();
		robot.limbs[0].print_active_outputs();
		robot.limbs[1].print_active_outputs();		
		printf(" FIRST ISSUE:  activate_enabled_outputs\n");		
	}
	counter++;

	// COMPUTE ELAPSED TIME from start until now:
	assert( robot.limbs.size() == robot.seq.limbs.size() );
	gettimeofday(&tv,NULL);
	float dtime = tv.tv_sec - starttime.tv_sec;
	dtime  += (tv.tv_usec - starttime.tv_usec)/1000;
	robot.update_submitted_timestamps( tv );	// goes into each actuator "submitted" timestamp.
			// SHOW (Green Letters!)		
	printf( "\n%sSEQ: New Vector: timestamp=%6.3f  %d/%d%s\n", KGRN, dtime,						
			robot.seq.Current_Vindex, robot.seq.limbs[0].vectors.size(), KNRM );

	// PLACE values INTO ACTUATORS : 
	robot.print_vector( robot.seq.Current_Vindex, false );
	printf("next_sequence_handler() print_vector Done \n");	
	robot.set_new_destinations( &(robot.seq), dtime );	  // set DestinatinPotValue 
	printf("next_sequence_handler() set_new_destinations Done \n");	
	robot.print_current_positions();
	printf("next_sequence_handler() print_current_positions Done \n");	
	robot.next_vector();			// Advance to the next vector
	printf("next_sequence_handler() next_vector Done \n");
	
/*	Note: Sequencer of vectors is stored in "sRobotVector seq" of robot.hpp		
	      speeds will get updated on next messages received
	
	   Our algorithm will work as follows :
			Issue the vectors on a periodic basis.  Ie this timer.
			Compare a timestamp from point of issue, to the destination reached.
			If reached early, adjust the duty to go slower next time.
			If reached late (ie. not reached before the timer goes off again),
				then speed up the weights.			

	We will focus first on reaching the destinations under constant load (ie no gravity)
	(acceleration/decel calculated)
	And later subtract the effect of gravity.		
	*/
}

void setup_scheduler()
{	
	long period_ns = robot.seq.limbs[0].playback_period_ms * 1000000;	
	printf("===== playback_period_ms = %lu \n", robot.seq.limbs[0].playback_period_ms );	
	//long period_ns = 1000000;
	
	gettimeofday( &starttime, NULL );

	// SETUP OS timer:
	set_handler ( &sa,  next_sequence_handler );
	create_timer( &sev, &timerid 			  );
	start_timer ( &its, period_ns, timerid     );
}


// void help() is in "seq_init.cpp"


/************************************************************************* 
Configure all BigMotor boards associated with this limb.
INPUT:
	mRate		: 
	mReports	: Which reports to send (VALUE, ANGLE, etc)
return 			: void
*************************************************************************/
void configure_motor_reports( bool mOn )
{
	byte mReports = MODE_SEND_POSITION_RAW; 
	byte mRate    = MODE_SEND_UPDATES_20ms; 
	if (mOn==false) {
		mReports = 0;
		mRate    = MODE_SEND_UPDATES_NONE;
	}
	
	robot.configure_motor_reports( mRate, mReports );			
}


/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.
*/
int main( int argc, char *argv[] )
{
	char* SequenceFileName = "vectors_raw.ini";		// was "vectors_raw.ini"
	
	print_args( argc, argv );
	int first_param = 1;		// sudo ./pican cfg
	int value    	= 0;
	int resample_iterations = 0;
	delay(10);
	
	if (argc > 1)
	{
		if ((strcmp(argv[1], "help") == 0) )
		{
				help();
				return 0;
		}		
		init();			
		create_threads();		

		if ( (strcmp(argv[1], "set_stop") == 0) ||
		     (strcmp(argv[1], "calibrate") == 0) )		
		{
			if (argc>2)
			{
				byte instance = atoi( argv[2] );
				if (argc>3)
				{
					byte stop_number = atoi( argv[3] );
					if (argc>4)
					{
						float angleDegrees = atof( argv[4] );
						if (argc>5)
						{
							word value = atoi( argv[5] );					
				    		pack_mark_motor_stop( &msg1, instance, stop_number, angleDegrees, value );						 		  
						 	print_message( &msg1 );
							AddToSendList( &msg1 );	  
						}
					}				 	
				}
			}
		}	// end of "set_stop"	
		if (strcmp(argv[1], "config_motors") == 0) 
		{
			if (strcmp(argv[2], "on") == 0) 
				// Configure board to send their raw positions : 
				configure_motor_reports( true );
			else
				configure_motor_reports( false );
		}
		if ( (strcmp(argv[1], "read_stop") == 0) ||
		     (strcmp(argv[1], "read") == 0) )
		{
			byte stop  = atoi( argv[2] );							
			word value = atoi( argv[3] );					
			//pack_mark_motor_stop( &msg1, instance, stop, angleDegrees, value );		
			print_message( &msg1 );
			AddToSendList( &msg1 );	  		
		}
		if ((strcmp(argv[1], "test") == 0) || (strcmp(argv[1], "speedtest") == 0))		
		{
			float speed;
			if (argc>2)	speed = atof(argv[2]);
			set_model_rx_callback( can_position_test_responder );			
			pack_move_speed( &msg1, 32, speed );
			AddToSendList( &msg1 );	  		
			gettimeofday( &start_ts, NULL );
			
			char c=0;
			while (c==0) { c = getchar(); };		// wait till pot reaches max/min.
			printf("Space pressed\n");			
			c = 0;
									// or keyboard press.						
			pack_move_speed( &msg2, 32, 0.0 );
			print_message  ( &msg2 );
			AddToSendList  ( &msg2 );
			while (c==0) { c = getchar(); };		// wait till pot reaches max/min.
		}

		if ((strcmp(argv[1], "meas") == 0) || (strcmp(argv[1], "measure") == 0))		
		{
			/* Measure should not move the motors at all.  
				The normal callback_board_presence move them. */
			robot.deactivate_outputs();
			robot.start_measurement_averaging( 20 );
			set_model_rx_callback( can_motor_position_responder );

			/* Here we'll read the Pot values for all motors.  Average 10 samples.
			   and print the results. */			
			while (robot.done_averaging()==false)	{	}

			printf("Data collected.  Average positions:\n");			
			robot.print_averages();		
		}
		if ( strcmp(argv[1], "jog") == 0 )
		{
			jog();		
		}
		
		if ( strcmp(argv[1], "play") == 0 )
		{
			printf("Playing sequence ...\n");
			if (argc > 2)
				SequenceFileName = argv[2];
			if (argc > 3)
				robot.seq.iterations_requested = atoi(argv[3]);	// -1 means infinite

			robot.seq.read_vector_file( SequenceFileName );
			robot.set_vectors_limbs();
			setup_scheduler();
			printf("Repeating %d iterations\n", robot.seq.iterations_requested );
			printf("====================looping===========================\n");

			while ((robot.seq.iterations_requested==-1) || 
					(robot.seq.iterations_completed < robot.seq.iterations_requested))
			{ /* timer based - next_sequence_handler() is called periodically 
			     CAN_isr() callback handles the positions.  */
			}
			printf("===All iterations requested have completed.  Done.===\n");
		}
		
		if ( strcmp(argv[1], "run") == 0 )
		{
			set_model_rx_callback ( can_motor_position_responder );
			printf("Running ...\n");
			if (argc > 2)
			{
				SequenceFileName = argv[2];
			}
			if (argc > 3)
				robot.seq.iterations_requested = atoi(argv[3]);	 // -1 means infinite

			// Deactivate Outputs:
			robot.deactivate_outputs();
			robot.clear_reads( 1 );
			bool result = false;
			while (result==false) {
				result = robot.are_reads_completed();
			};
			
			robot.seq.read_vector_file( SequenceFileName );			
			robot.set_vectors_limbs   (  );

			printf("MotorEnable 0,1,2 =%d,%d,%d \n", 
					robot.limbs[1].actuators[0].MotorEnable,
					robot.limbs[1].actuators[1].MotorEnable,
					robot.limbs[1].actuators[2].MotorEnable );

			robot.limbs[0].print_active_outputs();
			robot.limbs[1].print_active_outputs();

			setup_scheduler();					// sets a timer for  next_sequence_handler()

			printf("Repeating %d iterations\n", robot.seq.iterations_requested );
			printf("====================looping===========================\n");

			while ((robot.seq.iterations_requested==-1) || 
					(robot.seq.iterations_completed < robot.seq.iterations_requested))
			{
			  /* timer based - next_sequence_handler(); 
			     is called periodically 
			     CAN_isr() callback handles the positions. 
			    */
			   // printf("Looping...\n");
			   //teach_pendant.print();
			   // update_lcd();
			   // usleep(500000);
			}
			printf("all iterations requested have completed.  Done.\n");		
		}
	}
}

