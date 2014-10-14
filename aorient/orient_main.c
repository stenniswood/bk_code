#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "pican_defines.h"
#include "bk_system_defs.h"
#include "CAN_base.h"
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
#include "fuse_magnet.h"
#include "fuse_gyro.h"
#include "fuse_log.h"

// Wiring PI pin number (gpio pin 15)
//#define CAN_INT_PIN 	3
int  fd; 

void init()
{
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
	fuse_init();
	set_model_rx_callback( callback_tilt_reading );

	// Button boards set to 0x0E, 0x04, 0x13 which is 250 Kbps :
	open_log_file();
	
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
	CAN_init( CANSPEED_250, 0 );

	//printf("\nPiCAN Board present & Responding.\n" );
	//printf("*****************************************\n");
}
  
short MsgCounter    = 0;
byte  MsgIDCounter  = 0;
long  SerialNumber_ = 0x56789CD;
int   counter       = 0;
char  user_input[255];
char  cmd[4];
byte  instance;
byte  mask;
byte  value;

void help()
{
	printf("Orientation (wii like sensor) \n"							);
	printf(" 	\n"														);
	printf("This app calculates the best fit orientation of an Adrenaline Tilt sensor   \n");
	printf(" fusing the accelerometer, gyro & magnetometer measurements into a single set\n");
	printf(" 	\n"														);
	printf("Option List : \n\n"											);
	printf(" 	[a,g,m]		Suppress display of Accel, Gyro, or Magnet\n");
	printf(" 	\n"														);

	printf("sensitivity :  [accel/gyro/magnet] [level] change sensor sensitivity settings.\n");
	
	printf("instance [instance] [operation] [new instance:opt]\n"		 );
	printf("\t: restart - clear the boards instance & confirmed status\n");
	printf("\t: reassign - Directly change the boards instance number\n" );

	printf("lcd [instance] \n"			   			);
	printf("\t: contrast - change the contrast [percent on (0.0..100.0)]\n");
	printf("\t: beep - [on ms] [off ms] [number] \n");
	printf("\t: screen - Show test screen\n"		);
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
	int first_param = 1;		// sudo ./pican cfg
	int value    	= 0;
	print_args( argc, argv);
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

    if (argc>1) {
		if (strchr(argv[first_param], 'a') > 0)		
		{		
			ShowAccelerometerData = FALSE;
		} 
		if (strchr(argv[first_param], 'g') > 0)		
		{		
			ShowGyroData = FALSE;
		} 
		if (strchr(argv[first_param], 'm') > 0)		
		{	
			ShowMagnetData = FALSE;
		} 
		if (strchr(argv[first_param], 'c') > 0)		
		{		
			ShowCANData = FALSE;
		}
		
		if (argc>2) {
			if (strcmp(argv[first_param+1], "classify") == 0)
			{
				Classify = TRUE;
			}
		}
		if (strcmp(argv[first_param], "screen") == 0)	
		{
			//send_angles_to_screen1( instance );
		}
	}

	while (1) 
	{ 
		/* Main Loop.  Keep processing Tilt until quit */ 
		delay(100);
		if (Classify)
			classify_timeslice_100ms();
	}
}

