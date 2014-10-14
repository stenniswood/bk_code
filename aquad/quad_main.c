#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>

#include "pican_defines.h"
#include "CAN_Interface.h"
#include "packer_lcd.h"
#include "can_txbuff.h"
#include "packer.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "catagorization.h"
#include "fuse_ag.h"
#include "quad_ini.h"
#include "timer.h"
#include "power.h"


#define one_ms 	     1000000
#define fifty_ms    50000000
#define hundred_ms 100000000

char  QuadINIFileName[] = "quadracopter.ini";

short MsgCounter   = 0;
byte  MsgIDCounter = 0;
long  SerialNumber_      	 = 0x56789CD;
int   counter = 0;
char  user_input[255];
char  cmd[4];
byte  instance;
byte  value;

// Wiring PI pin number (gpio pin 15)
//#define CAN_INT_PIN 	3
int  fd;
BOOL ShowAccelerometerData 	= TRUE;
BOOL ShowGyroData 			= TRUE;
BOOL ShowMagnetData 		= TRUE;
BOOL ShowCANData 			= TRUE;
BOOL Classify 				= FALSE;

int count_accel = 0;
int count_gyro  = 0;

BOOL callback_tilt_reading( struct sCAN* mMsg )
{
	BOOL retval = FALSE;
	switch (mMsg->id.group.id)
	{
	case ID_ACCEL_XYZ : count_accel++;
		if (ShowAccelerometerData && ShowCANData)	print_message(mMsg);
		parse_accel_msg(mMsg, FALSE);		
		process_accel(ShowAccelerometerData);
		if (ShowAccelerometerData) printf("\n");
		retval= TRUE;
		break;
	case ID_GYRO_XYZ : count_gyro++;
		if (ShowGyroData && ShowCANData)		print_message(mMsg);		
		parse_gyro_msg(mMsg, ShowGyroData);
		process_gyro(ShowGyroData);
		if (ShowGyroData) printf("\n");		
		retval=TRUE;
		break;
	case ID_MAGNET_XYZ : 
		if (ShowMagnetData && ShowCANData)		print_message(mMsg);		
		parse_magnet_msg(mMsg, ShowMagnetData);
		process_magnet(ShowMagnetData);
		if (ShowMagnetData) printf("\n");		
		retval= TRUE;
		break;
	default:
		retval= FALSE;
		break;
	}
	if ((count_accel==1) && (count_gyro==1))
	{
		compute_accelGyro_fusion();
		count_accel=0;
		count_gyro=0;
	}
	return retval;
}

void timeslice_handler(int sig, siginfo_t *si, void *uc)
{
	counter++;
	if ((counter%20)==0)  // Every Second, update Thrust Reqested
	{
		printf( "timeslice Adjusting Thrust c=%d\n", counter);		
	}

}

void init()
{
	printf("Reading INI file\n");
	read_ini( QuadINIFileName );
	
	int result = wiringPiSetup();
	// Ready-to-send, Receive buffer full
	pinMode		( CAN_INT_PIN, INPUT );
	pinMode		( RX0BF, INPUT 	 	 );
	pinMode		( RX1BF, INPUT 	 );
	pinMode		( TX0RTS, OUTPUT );
	pinMode		( TX1RTS, OUTPUT );
	pinMode		( TX2RTS, OUTPUT );	
	digitalWrite( TX0RTS, 1 );
	digitalWrite( TX1RTS, 1 );
	digitalWrite( TX2RTS, 1 );
	button_init();
	init_leds();
	set_model_rx_callback( callback_tilt_reading );

	// Button boards set to 0x0E, 0x04, 0x13 which is 250 Kbps: 

	// set Pin 17/0 generate an interrupt on high-to-low transitions
	// and attach myInterrupt() to the interrupt
	if ( wiringPiISR(CAN_INT_PIN, INT_EDGE_FALLING, &CAN_isr) < 0 ) {
	  fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
	  exit(1);
	}
	if ( wiringPiISR(BUTTON1_INT_PIN, INT_EDGE_FALLING, &Button1_isr) < 0 ) {
	  fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
	  exit(1);
	}
	if ( wiringPiISR(BUTTON2_INT_PIN, INT_EDGE_FALLING, &Button2_isr) < 0 ) {
	  fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
	  exit(1);
	}
	if ( wiringPiISR(BUTTON3_INT_PIN, INT_EDGE_FALLING, &Button3_isr) < 0 ) {
	  fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
	  exit(1);
	}
	//set_system_rx_callback( callback_board_presence );
	Init( CANSPEED_250, 0 );
	
	printf("Starting Timer \n");
	// SETUP OS timer
	set_handler ( &sa,  timeslice_handler	);
	create_timer( &sev, &timerid 			);
	//start_timer ( &its, fifty_ms, timerid   );
	printf("done with init\n");
}

void help()
{
	printf("Quadracopter Flier \n"										 );
	printf(" 	\n"														 );
	printf("This app Runs the quadracopter basic flight operations.   \n");
	printf(" No GPS functionality.\n"									 );
	printf(" 	\n"														 );
	printf("Option List : \n\n"											 );
	printf(" 	thrust [percent:0..100.0]\n"							 );
	printf(" 	alpha  [0..1.0]  reaction to tilt angles.\n"			 );

	printf("	X  [instance] Tilt sensor X axis goes with which board.\n");
	printf("	Y  [instance] Tilt sensor Y axis goes with which board.\n");
}

void print_args(int argc, char *argv[])
{
	printf("ARGS: %d", argc);
	for (int i=0; i< argc; i++)
	{
		printf(" %s ", argv[i] );
	}
	printf("\n");
}


/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.		*/
int main( int argc, char *argv[] )
{
	BOOL  maintain_balance=FALSE;
	float thrust = 0.0;
	float alpha = 0.0;
	float longitude, latitude;
	byte x_instance;
	byte y_instance;
	int first_param = 1;		// sudo ./pican cfg
	int value    	= 0;
	//print_args( argc, argv);
	if (argc>1)
	{
		if ((strcmp(argv[first_param], "help") == 0))
		{
			help();
			return 0;
		}
	}
	printf("===============================================\n");
	init();
	printf("Done with init.  Starting thrust\n");
	if (argc>1) {
		if (strcmp(argv[first_param], "config") == 0)
		{	
			printf("Configuring Motor Boards (free_spin & 2 motors)\n");
			configure_motor_boards( instance1, instance2 );	
		} 
		if (strcmp(argv[first_param], "thrust") == 0)	
		{	
			// User can change thrust from cmd line one at a time.
			thrust = atof(argv[first_param+1]);
			printf("Thrust=%9.7f\n", thrust );
			AdjustThrust( thrust );
			power_timeslice_50ms();
		} 
		if (strcmp(argv[first_param], "balance") == 0)	
		{	
			maintain_balance=TRUE;
			// User can change thrust from cmd line one at a time.
			thrust = atof( argv[first_param+1] );
			AdjustThrust( thrust );
			power_timeslice_50ms();
		} 
		if (strcmp(argv[first_param], "GPS") == 0)
		{		
			// coordinate longitude, lattitude
			longitude  = atof(argv[first_param+1]);
			latitude  = atof(argv[first_param+2]);
		} 
		if (strcmp(argv[first_param], "file") == 0)
		{		
			// filename with gps path defined.
			// filename = argv[first_param+1];
		}		
	}
	printf("Entering while loop\n");
	while (maintain_balance) 
	{ 
		/* Main Loop.  Keep processing Tilt until quit */ 
		//delay(100);
		sleep(1000);
		//if (Classify)
			//classify_timeslice_100ms();
	};

}

