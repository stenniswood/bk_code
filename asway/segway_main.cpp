#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "pican_defines.h"
#include "CAN_Interface.hpp"
#include "packer_lcd.h"
#include "can_txbuff.h"
#include "can_rxbuff.hpp"
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
#include "fuse_gyro.h"
#include "preferences.hpp"
#include "help.h"
#include <pthread.h>
#include "packer_motor.h"
#include "fuse_accel.h"
#include "sway_config.hpp"
#include "mcp2515.h"
#include "mcp2515_defs.h"
#include "sway_memory.h"
#include "app_tilt.hpp"


#define GYRO_THRESHOLD = 0.1;		// dps



/**************  CAN SEND THREAD   *************************************/
pthread_t can_timer_thread_id;
void* can_timer_thread(void* n)
{
	while (1)
	{
		SendNext();		// Transmit next can message		// delay(10); 
	}
}

void create_threads()
{
	// CREATE TIMER THREAD :
	const char *message1 = "second";
	int iret1 = pthread_create( &can_timer_thread_id, NULL, can_timer_thread, (void*) message1);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}
/************************************************************************/
void button_init()
{
	pinMode( BUTTON_1, INPUT );
	pinMode( BUTTON_2, INPUT );
	pinMode( BUTTON_3, INPUT );
}
void Button1_isr()
{
	// Restart params:
	sc.load_file();
}
void Button2_isr()
{	
	sc.print_all_params();
}
void Button3_isr()
{	
	if (EmergencyStop==false)
	{
		// Stop Motors:
		pack_move_speed( &msg1, sc.get_LeftMotorInstance(),  0.0 );
		pack_move_speed( &msg2, sc.get_RightMotorInstance(), 0.0 );
		AddToSendList( &msg1 );
		AddToSendList( &msg2 );
		EmergencyStop = true;
	}
	else 
		EmergencyStop = false;		// resume!
}

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
	digitalWrite( TX0RTS, 1 	 );
	digitalWrite( TX1RTS, 1 	 );
	digitalWrite( TX2RTS, 1 	 );
	button_init();
	init_leds();
	fuse_init();
	//set_tiltsensor_callback( fusion_complete 	   );
	set_model_rx_callback  ( callback_tilt_reading );

	sc.load_file((char*)"segway.ini");	printf("Load_file completed.\n");
	sc.print_all_params();				printf("All params printed.\n");

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
	CAN_init( CANSPEED_250, 0 );
	create_threads();
	//printf("\nPiCAN Board present & Responding.\n" );
	//printf("*****************************************\n");
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

void process_CAN()
{
	struct sCAN* msg = NULL;
	int msg_ready    = isRxMessageAvailable();
	if (msg_ready)
	{
		msg = GetNextRxMsg();
		printf("prc_CAN():");		print_message( msg );
	}
	byte tec= read_register( TEC );
	byte rec= read_register( REC );
	if ((tec) ||(rec))
		printf("prc_CAN(): rec=%x;tec=%x\n", rec,tec);
} 

/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.		*/
int main( int argc, char *argv[] )
{
	float thrust = 0.0;
	float alpha  = 0.0;
	float longitude, latitude;
	int   first_param = 1;		// sudo ./pican cfg

	//print_args( argc, argv);
	if (argc>1)
	{
		if ((strcmp(argv[first_param], "help") == 0))
		{
			help();
			return 0;
		}
	}
	init();
	
	float* AngularRate = &(BestAngularRate.rx);
	AngularRate        = &(BestAngularRate.ry);
	printf("================= Main Loop ================================\n" );

	// 5 degree tilt 
	// 100% at 90 degrees.  So alpha = 100 / 90 
	// at 20 deg 50%        so 50/20 = 5/2 = 2.5 
	while (1) 
	{ 
		// Tilt measurements come in the   callback  function
		// Result will be stored in BestAngularRate  
		//process_CAN();

		/* Main Loop.  Keep processing Tilt until quit */
		// Here we just adjust the wheels speeds.
		// We concern ourselves only with axis - front/back (defined in config parameter x or y)
		//if ( (*AngularRate) > GYRO_THRESHOLD)
		// takes actual percent [-100.0 to +100.0]

		// Pause
		//delay(100);
	};	
}

