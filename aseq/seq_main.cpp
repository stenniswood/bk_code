/****************************************************
Sequencer Main : 

Analog Board Configuration should be:
	0x05  0x11  0x05 0x42
****************************************************/
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
#include <regex>

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
#include "sequencer_memory.hpp"
#include "SEQUENCER_protocol.hpp"
#include "nlp_sentence.hpp"
#include "roboclaw.hpp"


SequencerIPC  sequencer_mem;

#define fifty_ms    50000000

volatile int seq_counter = 0;
volatile int seq_counter_handled = 0;

bool FirstIssued = false;
struct timeval	start_ts;
struct timeval tv;
struct timeval starttime;


#if (MOTOR_DRIVER==USE_ROBOCLAW)
RoboClaw mot1("/dev/ttyACM0", 50000000);
RoboClaw mot2("/dev/ttyACM1", 10000);
#elif (MOTOR_DRIVER==USE_BK_QM)

#elif (MOTOR_DRIVER==USE_BK_CAN)

#endif


void print_sig_info( siginfo_t *si )
{
	printf("siginfo_t si= %x\n", si );
	printf("si_signo=%d\n",  si->si_signo );
	printf("si_errno=%d\n",  si->si_errno );
	printf("si_code=%d\n",   si->si_code );
	//printf("si_trapno=%d\n", si->si_trapno );
	
	printf("si_pid=%d\n", 	 si->si_pid );
	printf("si_uid=%d\n", 	 si->si_uid );
	printf("si_status=%d\n", si->si_status );
							
/*	printf("si_status=%d\n", si->si_status );
	clock_t  si_utime;     * User time consumed *
	clock_t  si_stime;     * System time consumed *
	sigval_t si_value;     * Signal value *
	int      si_int;       * POSIX.1b signal *
	void    *si_ptr;       * POSIX.1b signal *
	int      si_overrun;   * Timer overrun count;
							 POSIX.1b timers *
	int      si_timerid;   * Timer ID; POSIX.1b timers *
	void    *si_addr;      * Memory location which caused fault *
	long     si_band;      * Band event (was int in
							 glibc 2.3.2 and earlier) *
	int      si_fd;        * File descriptor *
	short    si_addr_lsb;  * Least significant bit of address
							 (since Linux 2.6.32) *
	void    *si_call_addr; * Address of system call instruction
							 (since Linux 3.5) *
	int      si_syscall;   * Number of attempted system call
							 (since Linux 3.5) *
	unsigned int si_arch;  * Architecture of attempted system call
									 (since Linux 3.5) */
}
struct		sigaction sa_sequencer;
struct		sigevent  sev_sequencer;

float compute_elapsed_time()
{
	// COMPUTE ELAPSED TIME from start until now:
//	assert      ( robot.limbs.size() == robot.seq.limbs.size() );
	gettimeofday( &tv, NULL );
	float dtime = (tv.tv_sec - starttime.tv_sec);
//	dtime       += ((float)(tv.tv_usec) - (float)(starttime.tv_usec))/1000000.;

	// SHOW (Green Letters!)
	printf( "\n%sSEQ: New Vector: timestamp=%6.3f  %d/%d%s\n", KGRN, dtime,						
			robot.seq.Current_Vindex, robot.seq.limbs[0].vectors.size(), KNRM );
	printf("\n %s SEQ: New Vector: %d/%d%s\n", KGRN, robot.seq.Current_Vindex, robot.seq.limbs[0].vectors.size(), KNRM );
	return dtime;
}

void activate_vector( float dtime )
{
	// ACTIVATE:	
	if (robot.seq.data_type == DUTY_VECTORS )
		robot.set_duty( &(robot.seq), robot.seq.Current_Vindex, dtime );
	else {
		robot.set_new_destinations( &(robot.seq), dtime );	  // set DestinatinPotValue 
		robot.print_current_positions();
	}
}

/* 
	Here we marry a vector in the sequencer with the robot limbs.
*/
void next_sequence_handler(int sig, siginfo_t *si, void *uc)
{
	printf("next_sequence_handler(%d, %x)\n", sig,  uc );
	print_sig_info( si );
	
	if (FirstIssued==false) {
		FirstIssued = true;		
		//robot.activate_enabled_outputs();
		//robot.limbs[0]->print_active_outputs();
		//robot.limbs[1]->print_active_outputs();		
		//robot.update_submitted_timestamps( tv );	// goes into each actuator "submitted" timestamp.
		// PLACE values INTO ACTUATORS : 
		//	robot.print_vector( robot.seq.Current_Vindex, false );
		//printf(" FIRST ISSUE:  activate_enabled_outputs\n");		
	}
	seq_counter++;

	//printf("next_sequence_handler() next_vector Done \n");
	//signal (sig, next_sequence_handler);
	//sigaction(SIG, msa, NULL);
	//set_handler ( &sa_sequencer,  next_sequence_handler );
	
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

void multiply_now( )
{	
	long int  ppm        = 5000;
	float     fperiod_ns = (float)ppm * 1000000.;
	long long period_ns  = ((long long)ppm * 1000000);
	printf("===== playback_period_ms = %lu ", ppm        );
	printf("===== %lld  %6.2f \n", period_ns, fperiod_ns );
}

void setup_scheduler()
{	
	long int ppm = robot.seq.limbs[0].playback_period_ms;
	long long period_ns = ((long long)ppm * 1000000);
	printf("===== playback_period_ms = %lu ", ppm  );
	printf("===== %lld \n", period_ns );
	
	gettimeofday( &starttime, NULL );

	// SETUP OS timer:
	set_handler ( &sa_sequencer,  next_sequence_handler );
	create_timer( &sev_sequencer, &timerid 	);
	start_timer ( &its, period_ns, timerid  );
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

void activate_outputs()
{
		printf("%x\n", robot.limbs[0]->actuators[0] );
		printf("%x\n", robot.limbs[0]->actuators[1] );
		printf("%x\n", robot.limbs[0]->actuators[2] );
		
		robot.limbs[0]->actuators[0]->ActiveOutputs = 1; //actuators[a].MotorEnable;
		robot.limbs[0]->actuators[1]->ActiveOutputs = 1; //actuators[a].MotorEnable;
		robot.limbs[0]->actuators[2]->ActiveOutputs = 1; //actuators[a].MotorEnable;

		robot.limbs[1]->actuators[0]->ActiveOutputs = 0; //actuators[a].MotorEnable;
		robot.limbs[1]->actuators[1]->ActiveOutputs = 1; //actuators[a].MotorEnable;
		robot.limbs[1]->actuators[2]->ActiveOutputs = 1; //actuators[a].MotorEnable;
}

void init_roboclaws()
{
	if (mot1.connected == true)
		 printf("Roboclaw #1 - Available\n"    );
	else printf("Roboclaw #1 - not available\n");
	
	if (mot2.connected == true)	
		 printf("Roboclaw #2 - Available\n"    );
	else printf("Roboclaw #2 - not available\n");

}


/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.
*/
int main( int argc, char *argv[] )
{
	char* SequenceFileName = "vectors_raw.ini";		// was "vectors_raw.ini"
	print_args( argc, argv );
	
	sequencer_mem.connect_shared_memory(TRUE); 
	//ServerHandler mh;
	Sentence theSentence;
	 
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
		if ( strcmp(argv[1], "remote") == 0 )
		{
			printf("Inside REMOTE\n");
			//setup_scheduler();					// sets a timer for  next_sequence_handler()
			while (1)
			{
				if (sequencer_mem.is_new_sentence())
				{
					theSentence.set( sequencer_mem.get_sentence(), false );
					printf("NEW SENTENCE: %s\n", sequencer_mem.get_sentence() );					
					//printf("RAW SENTENCE: %s\n", theSentence.m_raw_sentence );						
					Parse_Sequencer_Statement( theSentence );
					sequencer_mem.ack_sentence_counter();	
									
				}
			}
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
			// INTENDED TO RUN THRU VECTORS WITHOUT ACTUATING! (to verify calculations, etc)
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
		if ( strcmp(argv[1], "stop") == 0 )
		{	
			sRobotVector zero;
			sVectorSet   zset;
			sOneVector	 zvec;
			
			zvec.Data.push_back( 0.0 );
			zvec.Data.push_back( 0.0 );
			zvec.Data.push_back( 0.0 );
			
			zero.data_type = 4;
			zset.data_type = 4;
			zvec.data_type = 4;			
			zset.vectors.push_back(zvec);
			zset.vectors.push_back(zvec);
			zero.limbs.push_back  (zset);
			zero.limbs.push_back  (zset);
			
			zero.set_limbs( robot );
			robot.activate_enabled_outputs();
			robot.set_duty( &zero, 0, 1.0 );			
			usleep(1000000);
		}
		
		//if (1==1)
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
/*			bool result = false;
			while (result==false) {
				result = robot.are_reads_completed();
			}; */

			robot.seq.read_vector_file( SequenceFileName );
			robot.set_vectors_limbs   (  );

			printf("MotorEnable 0,1,2 =%d,%d,%d \n", 
					robot.limbs[1]->actuators[0]->MotorEnable,
					robot.limbs[1]->actuators[1]->MotorEnable,
					robot.limbs[1]->actuators[2]->MotorEnable );

			robot.limbs[0]->print_active_outputs();
			robot.limbs[1]->print_active_outputs();

//			setup_scheduler();					// sets a timer for  next_sequence_handler()

			printf("Repeating %d iterations\n", robot.seq.iterations_requested );
			printf("====================looping===========================\n");

			long int  ppm = robot.seq.limbs[0].playback_period_ms;
			long long period_us = ((long long)ppm * 1000);

						robot.activate_enabled_outputs();
			while (1)						
//			while ((robot.seq.iterations_requested==-1) || 
//					(robot.seq.iterations_completed < robot.seq.iterations_requested))
			{
			    printf("Looping...\n");
//				if (seq_counter > seq_counter_handled)
				{
//					if (seq_counter == 1)  {
//					}
					float dtime = compute_elapsed_time();
					robot.print_vector( robot.seq.Current_Vindex, false );
					activate_vector(dtime);	
					robot.next_vector();			// Advance to the next vector
					//seq_counter_handled = seq_counter;

					printf("next_sequence_handler() next_vector Done \n");					
					usleep(period_us);
				}
			  /* timer based - next_sequence_handler(); 
			     is called periodically 
			     CAN_isr() callback handles the positions. 
			    */

			   //teach_pendant.print();
			   // update_lcd();
			   // usleep(500000);
			}
			printf("all iterations requested have completed.  Done.\n");		
		}
	}
	
}

