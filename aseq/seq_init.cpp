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
#include "seq_init.hpp"



struct sCAN msg;
TeachPendant 	teach_pendant;
char 	ConfigureFileName2[] = "config_new.ini";



void Button1r_isr()
{
	printf("Button 1 Pressed Interrupt - reg_dump()\n");
	register_dump   ( );
	tx_register_dump(0);	
}
void Button2r_isr()
{
	CAN_init( CANSPEED_250, 0 );
	printf("Button 2 Pressed - Reset CAN \n");	
}
void Button3r_isr()
{
	printf("Pause Sequencer.\n");
	printf("--------------->>>>>>>\n");
	char txt[] = {"hello!!!"};
	pack_lcd_write_text( &msg1, 0, txt);
	AddToSendList( &msg1 );
	printf("--------------->>>>>>>\n");
}


/* Set Callback rate at 20Hz 
	Use this to dispatch the vectors only.
	The speeds/pids will be updated when position messages arrive. */

void setup_teach()
{
	teach_pendant.add_dial( 0x16, "Rotate");
	teach_pendant.add_dial( 0x15, "Hip"  );
	teach_pendant.add_dial( 0x14, "Knee" );
	teach_pendant.add_dial( 0x13, "Ankle");
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
	set_system_rx_callback( callback_board_presence  );

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
	if ( wiringPiISR(BUTTON3_INT_PIN, INT_EDGE_FALLING, &Button3r_isr) < 0 ) {
	  fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
	  exit(1);
	}
}


void init()
{
	init_hardware();
	printf("READING CONFIGURE...\n");	
	//read_config( ConfigFileName, robot );
	robot.load_config( ConfigureFileName2 );

	init_interrupts();

	// Button boards set to 0x0E, 0x04, 0x13 which is 250 Kbps :
	CAN_init      ( CANSPEED_250, 0);
	read_register ( CANSTAT );
	write_register( CANINTE,  0x1F );	// enable 2 TXs & 1 RX	
	printf("Finished Init!\n");

	setup_teach();
	
	// SHARED MEM (for receiving commands via Instant) :
	//int result = seq_connect_shared_sequencer_memory(TRUE); see main.cpp
	
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
	
	printf("config_motors\t\tConfigure BigMotor boards to report their rate. \n" );
//	printf("\t[rate:1,2,4,8]\tTime period between reports:  10, 20, 50, 100 ms.\n"	 );
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
	printf("remote\tLoop idle until we receive commands via Instant server.\n"	 		);		
	printf("      \ttext commands are described via Sequencer_protocol.cpp\n");
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


/////////////////////////////////////////////////////////////////
// CREATE CAN THREAD : 
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

void read_cnfs()
{
	printf( "\tCNF1: " );
	int result = read_register( CNF1 );
	printf("\tCNF2: " );
	result = read_register( CNF2 );
	printf("\tCNF3: " );
	result = read_register( CNF3 );
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
			robot.limbs[limb]->actuators[actuator]->DestinationCount = 
						robot.limbs[limb]->actuators[actuator]->ZeroOffset;
			//robot.limbs[limb].actuators[actuator].send_speed_pid();			
			while(1==1) { };			// Fix!
		}
		else {
			// Now pulse it for a short time:
			robot.limbs[limb]->actuators[actuator]->send_speed( duty );
			delay(1000);
			robot.limbs[limb]->actuators[actuator]->send_speed( 0.   );
		}
		c = getchar(); 	 
	};		// wait till pot reaches max/min.
	printf("Space pressed\n");
}

char str[80];	

void update_lcd()
{
	char tmp[20];	
	int instance = 41;
	printf("UpdateLCD\n\n");
	pack_lcd_clear_screen( &msg, instance );
	AddToSendList( &msg );
	usleep(2000);
	
	pack_lcd_cursor_to( &msg, instance, 0, 1, 0 );
	AddToSendList( &msg );	
	usleep(2000);
	sprintf(str, "Teach Pe");
	pack_lcd_write_text	( &msg, instance, str );
	AddToSendList( &msg );
	usleep(2000);
	sprintf(str, "ndant:");
	pack_lcd_write_text	( &msg, instance, str );
	AddToSendList( &msg );
	usleep(2000);
		
	for (int i=0; i<4; i++)
	{
		pack_lcd_cursor_to( &msg, instance, i+2, 2, 0 );
		AddToSendList( &msg );
		usleep(2500);
		
		sprintf(str, "# %d=%d", i, teach_pendant.m_dials[i].CurrCount );
		pack_lcd_write_text	( &msg, instance, str );
		AddToSendList( &msg );
		usleep(2500);
	} 
}

