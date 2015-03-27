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
#include "config_file.h"


// Wiring PI pin number (gpio pin 15)
#define CAN_INT_PIN 3
#define BUTTON1_INT_PIN 4
#define BUTTON2_INT_PIN 5
#define BUTTON3_INT_PIN 6

#define fifty_ms    50000000

short MsgCounter   = 0;
byte  MsgIDCounter = 0;
// Each module sends a message to identify itself (type of device, and 
// SerialNumber)
const byte AdrenalineEdgeID  = 0x01;
const byte AdrenalineBlueID  = 0x02;
const byte AdrenalinePWMID   = 0x03;
const byte AdrenalinePowerID = 0x04;
const byte AdrenalineRawID   = 0x05;
long  SerialNumber_      	 = 0x56789CD;

int counter = 0;
char user_input[255];
char cmd[4];
byte instance;
byte value;
bool FirstIssued = false;

struct timeval start_ts;

void Button2r_isr()
{
	CAN_init( CANSPEED_250, 0 );
	printf("Button 2 Pressed - Reset CAN \n");	
}
void Button1r_isr()
{
	printf("Button 1 Pressed Interrupt - reg_dump()\n");
	register_dump   ( );
	tx_register_dump(0);	
} 

/*void Button3_isr()
{
	printf("Button 3 Pressed Interrupt\n");
	printf("--------------->>>>>>>\n");
	char txt[] = {"hello!!!"};
	pack_lcd_write_text( &msg1, 0, txt);
	AddToSendList( &msg1 );
	printf("--------------->>>>>>>\n");
} */

/////////////////////////////////////////////////////////////////
// CREATE CAN THREAD: 
pthread_t scheduler_thread_id;
void* scheduler_thread(void* n)
{
	while (1)
	{
		can_tx_timeslice();
		usleep( 100 );
	}
}

void create_threads()
{
	// CREATE TIMER THREAD :
	const char *message1 = "second";
	int iret1 = pthread_create( &scheduler_thread_id, NULL, scheduler_thread, (void*) message1 );
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}

/* Set Callback rate at 20Hz 
	Use this to dispatch the vectors only.
	The speeds/pids will be updated when position messages arrive. */
struct timeval tv;
bool do_not_send = false;

void next_sequence_handler(int sig, siginfo_t *si, void *uc)
{
	counter++;
	if ((counter%20)==0)  // Every Second, update Thrust Reqested
	{
		assert( robot.limbs.size() == robot.seq.limbs.size() );
		
		// PLACE values into Actuators : 			
		printf( "\nSEQ: New Vector: %d/%d\n", robot.seq.Current_Vindex, robot.seq.limbs[0].vectors.size() ); 
		if (do_not_send==false)
		{
			// Get TimeStamp : 		
			gettimeofday(&tv,NULL);
			robot.update_submitted_timestamps( tv );
		}
		robot.set_new_destinations();	// set DestinatinPotValue
		robot.print_vector( robot.seq.Current_Vindex, false );
		//robot.seq.limbs[0].vectors[i].print_counts( );
		robot.print_current_positions();

		robot.next_vector();			
		FirstIssued = true;
		
		// speeds will get updated on next messages received
		
		//printf( "\nSEQ: New Vector: done.\n" );		
		/* Our algorithm will work as follows :
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
}

void init_hardware()
{
	int result = wiringPiSetup();
	// Ready-to-send, Receive buffer full
	pinMode		( CAN_INT_PIN, INPUT );
	pinMode		( RX0BF,  INPUT 	 );
	pinMode		( RX1BF,  INPUT  );
	pinMode		( TX0RTS, OUTPUT );
	pinMode		( TX1RTS, OUTPUT );
	pinMode		( TX2RTS, OUTPUT );	
	digitalWrite( TX0RTS, 1 );
	digitalWrite( TX1RTS, 1 );
	digitalWrite( TX2RTS, 1 );
}

/* Note:
	Calling this first _before_ CAN_init()
	Fixed the problem of interrupts not working (ie. not receiving data)
	So to explain:  If an interrupt occurred before the handler was there...
		The interrupt trigger is on the falling edge (not the low level!)
		So we miss the first interrupt, and the flag doesn't get cleared to
		let the INT line go high again and catch the next edge!	
*/
void init_interrupts()
{
	set_system_rx_callback( callback_board_presence 	 );

	// set Pin 17/0 generate an interrupt on high-to-low transitions
	// and attach myInterrupt() to the interrupt
	if ( wiringPiISR(CAN_INT_PIN, INT_EDGE_FALLING, &CAN_isr) < 0 ) {
	  fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
	  exit(1);
	}
	if ( wiringPiISR(BUTTON1_INT_PIN, INT_EDGE_FALLING, &Button1r_isr) < 0 ) {
	  fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
	  exit(1);
	}
	if ( wiringPiISR(BUTTON2_INT_PIN, INT_EDGE_FALLING, &Button2r_isr) < 0 ) {
	  fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
	  exit(1);
	}
	if ( wiringPiISR(BUTTON3_INT_PIN, INT_EDGE_FALLING, &Button3_isr) < 0 ) {
	  fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
	  exit(1);
	}

	//printf("\nPiCAN Board present & Responding.\n" );
	//printf("*****************************************\n");

//	write_register( CANCTRL,  0x10 );	// Abort any transmitting msgs...
//	write_register( CANCTRL,  0x00 );	// 
}

void init()
{
	init_hardware();
	printf("READING CONFIGURE...\n");	
	read_config( ConfigFileName, robot );
	
	init_interrupts();
	
	// Button boards set to 0x0E, 0x04, 0x13 which is 250 Kbps :
	CAN_init      ( CANSPEED_250, 0);
	read_register ( CANSTAT );
	write_register( CANINTE,  0x1F );	// enable 2 TXs & 1 RX	
	printf("Finished Init!\n");
}


void setup_scheduler()
{
	robot.limbs[0].ElementsFilled = 0;
	robot.limbs[1].ElementsFilled = 0;
	FirstIssued = true;
	
	// SETUP OS timer:
	set_handler ( &sa,  next_sequence_handler );
	create_timer( &sev, &timerid 			  );
	start_timer ( &its, fifty_ms, timerid     );
}

void read_cnfs()
{
	printf( "\tCNF1: " );
	int result = read_register( CNF1 );
	printf("\tCNF2: " );
	result = read_register( CNF2 );
	printf("\tCNF3: " );
	result = read_register( CNF3 );
}


void help()
{
	printf("\nAdrenaline Motion Sequencer!   \n\n"				   	   );
	printf(" The app takes a configure file and a vector file. "	   );
	printf(" The config file ('sequence.ini' by default)\n specifies the number");
	printf(" of limbs, and actuators on each limb.  For each actuator, the instance ids,");
	printf(" \n enables, zeroOffsets, default direction, and calibrated stops.\n\n"	);
	
	printf("COMMANDS : \n\n");
	printf("help\t\tShow this text.\n\n"	 						  );	
	printf("report\t\tConfigure BigMotor boards to report their:\n" );
	printf("\t[rate:1,2,4,8]\tTime period between reports:  10, 20, 50, 100 ms.\n"	 );
	printf("\t[report type: none,value,angle]\n"	 		);
	printf("\t\tvalue\t: Potentiometer Values\n"	 		);
	printf("\t\tangle\t: Calibrated angles\n"	 			);
	printf("\t\tnone\t: Turn off BigMotor reports\n\n"		);

	printf("meas\t\tMeasures the Pot/Encoder counts (average of 10 samples). Use to find zero Offsets. Motors stopped during test.\n\n");
	printf("test\t\tRuns the motor up to [speed].  records positions and speed during. \n"	);
	printf("\t\t\tPress \"return\" to stop motor.  \n"	);
	printf("\t\t\t[speed] may be [-100 to 100] \n\n"	);
		
	printf("calibrate||set_stop\t\tConfigure a BigMotor stop angle:.\n"	 				);
	printf("\t[instance]\t\tInstance number of the board to calibrate.\n"	 			);
	printf("\t[stop number:0,1]\tStop number to calibrate.\n"	 						);
	printf("\t[float:Angle in Degrees] Angle that the stop corresponds to (degrees).\n"	);
	printf("\t[word: Pot Value]\tPotentiometer Value for the angle (-1 indicates current Pot value)\n\n");

	printf("play\tDisplays the sequecing on the terminal. Use to verify files & app before running.\n");	
	printf("\t[sequence file]\t\tSpecifies .csv file with vector samples.\n"			);
	printf("\t[repeat times]\t\tSpecifies how many times the sequence should be repeated.\n\n");
	
	printf("run\tSpecifies instance numbers & number of vectors.\n"	 					);	
	printf("\t[sequence file]\t\tSpecifies .csv file with vector samples.\n"			);
	printf("	\t(default=\"sequence.csv\") \n"	 									);		
	printf("\t[resample iterations]\tSpecifies n number of resampling iterations.\n"	);
	printf("					  Actual rate will be 2^n number of samples.\n"  		);
	printf("\t[repeat times]\t\tSpecifies how many times the sequence should be repeated.\n");
	printf("			  \t use -1 for infinite.\n"									 );
	printf("\n"	 );
}

void print_args(int argc, char *argv[])
{
	printf("%d ARGS: ", argc);
	for (int i=0; i< argc; i++)
	{
		printf(" %s ", argv[i] );
	}
	printf("\n");
}

// Taken from "AdrenalineFirmware\BigMotorEn\motor.h"
#define MODE_SEND_UPDATES_NONE	0x00	// 
#define MODE_SEND_UPDATES_10ms	0x10	// 
#define MODE_SEND_UPDATES_20ms	0x20	// 
#define MODE_SEND_UPDATES_50ms	0x40	// 
#define MODE_SEND_UPDATES_100ms	0x80	// 
#define MODE_SEND_POSITION_RAW	0x01 	// Measured Pot/Encoder, CurrentLeft, CurrentRight, Speed
#define MODE_SEND_POSITION_CALC 0x02 	// Calculated quantities: Angle (deg*100), Current (Amps*100)
#define MODE_SEND_STATUS		0x04 	// 

/************************************************************************* 
Configure all BigMotor boards associated with this limb.
INPUT:
	mRate		: 
	mReports	: Which reports to send (VALUE, ANGLE, etc)
return 			: void
*************************************************************************/
void configure_motor_reports(  )
{
	byte mReports = MODE_SEND_POSITION_RAW;
	byte mRate = MODE_SEND_UPDATES_20ms; 
	for ( int index=0; index < robot.limbs.size(); index++ )
		// Does all motors on Appendage[index] : 
		robot.limbs[index].configure_motor_reports( mRate, mReports );
}

void jog()
{	
	int limb = 0;
	int actuator = 0;
	float duty = 20.;
	int dir=1;
	
	char c=0;
	while (c != 'q') 
	{
		switch (c)
		{
		case 0: break;
		case '-':   duty -= 10.;	if (duty<10.) duty=10.;		
				printf("duty = %6.2f\n", duty );
				break;
		case '=':   duty += 10.;	if (duty>100.) duty=100.;	
				printf("duty = %6.2f\n", duty );				
				break;		
		
		case 'o': 	limb=0;		actuator=0;		dir=-1;		break;
		case 'p': 	limb=0;		actuator=0;		dir=+1;		break;
		case 'l': 	limb=0;		actuator=1;		dir=-1;		break;
		case ';': 	limb=0;		actuator=1;		dir=+1;		break;
		case '.': 	limb=0;		actuator=2;		dir=-1;		break;
		case '/': 	limb=0;		actuator=2;		dir=+1;		break;

		case 't': 	limb=1;		actuator=0;		dir=-1;		break;
		case 'y': 	limb=1;		actuator=0;		dir=+1;		break;
		case 'g': 	limb=1;		actuator=1;		dir=-1;		break;
		case 'h': 	limb=1;		actuator=1;		dir=+1;		break;
		case 'b': 	limb=1;		actuator=2;		dir=-1;		break;
		case 'n': 	limb=1;		actuator=2;		dir=+1;		break;
		
		// seek home:
		case 'w': 	limb=0;		actuator=0;		dir=0;		break;
		case 's': 	limb=0;		actuator=1;		dir=0;		break;
		case 'x': 	limb=0;		actuator=2;		dir=0;		break;
		case 'e': 	limb=1;		actuator=0;		dir=0;		break;
		case 'd': 	limb=1;		actuator=1;		dir=0;		break;
		case 'c': 	limb=1;		actuator=2;		dir=0;		break;										
		default: break;
		}	
		
		if (dir==0)		// Seek Home!
		{
			robot.limbs[limb].actuators[actuator].DestinationPotValue = 
						robot.limbs[limb].actuators[actuator].ZeroOffset;
			robot.limbs[limb].actuators[actuator].send_speed_pid();
			while(1==1) { };			// Fix!
		}
		else {
			// Now pulse it for a short time:
			robot.limbs[limb].actuators[actuator].send_speed( duty );
			delay(1000);
			robot.limbs[limb].actuators[actuator].send_speed( 0.   );
		}
		c = getchar(); 	 
	};		// wait till pot reaches max/min.
	printf("Space pressed\n");
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
	init();						// 
	create_threads();
	
	// Configure board to send their raw positions : 
	configure_motor_reports();
	
	if (argc > 1)
	{
		if ((strcmp(argv[1], "help") == 0) )
		{
				help();
				return 0;
		}		

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
			/* Measure should not move the motors at all.  The normal callback_board_presence
			   move them. */
			set_model_rx_callback( can_position_meas_responder );			
			const int NUM_SAMPLES = 10;
			
			robot.set_vectors_limbs();
			robot.seq.read_vector_file( SequenceFileName );
			robot.set_vectors_limbs();
						
			/* Here we'll read the Pot values for all motors.  Average 10 samples.
			   and print the results. */			
			while (robot.limbs[0].Reads < NUM_SAMPLES)
			{	}
			printf("Reads = %d\n", robot.limbs[0].Reads );
			float sum_pot_values[100];			
			for (int a=0; a<robot.limbs[0].actuators.size(); a++)
			{
					robot.limbs[0].actuators[a].MotorEnable = FALSE;
					sum_pot_values[a] = 0.0;
			}
			for (int a=0; a<robot.limbs[0].actuators.size(); a++)
					sum_pot_values[a] += robot.limbs[0].actuators[a].CurrPotValue;
			for (int a=0; a<robot.limbs[0].actuators.size(); a++)
					sum_pot_values[a] /= NUM_SAMPLES;
			printf("\nResults Pot Averages (%d samples): \n", NUM_SAMPLES);
			printf(" ZeroOffset ");
			for (int a=0; a<robot.limbs[0].actuators.size(); a++)
					printf(" %5.2f ", sum_pot_values[a] );	
			printf("\n\n");		
		}
		if ( strcmp(argv[1], "jog") == 0 )
		{
			jog();		
		}
		
		if ( strcmp(argv[1], "play") == 0 )
		{
			do_not_send = true;
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
				robot.seq.iterations_requested = atoi(argv[3]);	// -1 means infinite

			robot.seq.read_vector_file( SequenceFileName );			
			robot.set_vectors_limbs(  );	//
			setup_scheduler();

			printf("Repeating %d iterations\n", robot.seq.iterations_requested );
			printf("====================looping===========================\n");

			while ((robot.seq.iterations_requested==-1) || 
					(robot.seq.iterations_completed < robot.seq.iterations_requested))
			{
			  /* timer based - next_sequence_handler(); 
			     is called periodically 
			     CAN_isr() callback handles the positions. 
			    */
			}
			printf("all iterations requested have completed.  Done.\n");		
		}	
		
	}
}



