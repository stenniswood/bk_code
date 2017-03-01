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
#include "preferences.hpp"
#include "help.h"
#include <pthread.h>
#include "packer_motor.h"
#include "atmel_spi.h"
//#include "ipc_mem_bkinstant.h"
#include "shared_mem_extractor.h"

/**************  CAN SEND THREAD   *************************************/
//pthread_t can_timer_thread_id;
pthread_t bk_instant_thread_id;
pthread_t bk_camera_thread_id;		// maybe not
void* bk_instant_thread(void* n)
{
}
void* bk_camera_thread(void* n)
{
}
void create_threads()
{	
	// CREATE INSTANT THREAD :
	const char *message1 = "second";
	int iret1 = pthread_create( &bk_instant_thread_id, NULL, bk_instant_thread, (void*) message1);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}

	// CREATE CAMERA THREAD :
	iret1 = pthread_create( &bk_camera_thread_id, NULL, bk_camera_thread, (void*) message1);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}

int   cmd  = 0;
/************************************************************************/
//int channel = 0;		// CEO
void init()
{
	int result = wiringPiSetup();

    int spi_speed = 500000;
    if( wiringPiSPISetup(0, spi_speed)==-1)
    {
		printf("Could not initialise SPI\n");
		printf("Make sure you run this as root: sudo ./aconf\n");
		return ;
    }
    else printf("SPI Port opened at %d bps\n", spi_speed );

	pinMode		( TX2RTS, OUTPUT );	
	digitalWrite( TX0RTS, 1 );
	button_init();
	init_leds();
	set_model_rx_callback( callback_tilt_reading );

	// Generate an interrupt on high-to-low transitions
	// and attach myInterrupt() to the interrupt
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
	create_threads();	
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

struct servo_calibration
{
	float MinAngle;
	float MaxAngle;	
	float Min_Pulsewidth_ms 	;
	float Max_Pulsewidth_ms		;
	float Neutral_Pulsewidth_ms ;
};

struct config_info
{
	struct servo_calibration servo1;
	struct servo_calibration servo2;
	//float analog_scales[8];	perhaps multipliers by the raw analog values.	
} ci;

void receive_instant_messages()
{	
	// Check semaphore and update atmel data.
}

void send_atmel_messages()
{
	printf("Sending pan=%6.2f;  tilt=%6.2f\n", Pan, Tilt );
	update_pan_tilt_angles( Pan, Tilt );	
	//update_low_side_drivers(LS_Values);
	//read_all_analogs();
}

/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.		*/
int main( int argc, char *argv[] )
{
	int first_param = 1;		// sudo ./pican cfg
	print_args( argc, argv);
	if (argc>1)
	{
		if ((strcmp(argv[first_param], "help") == 0))
		{
			help();
			return 0;
		}
		else 
		{
			Pan  = atof( argv[first_param]   );
			Tilt = atof( argv[first_param+1] );
			if (argc>3)
				cmd  = atoi( argv[first_param+2] );
			printf("Pan=%6.2f;\t\tTilt=%6.2f\n", Pan, Tilt );
			send_atmel_messages();
			delay(500);
			exit(1);
		}
	}
	init();

	printf("Establishing Shared Memory\n");		
	/* Allocate a shared memory segment. *
	int segment_id = allocate_memory();
	attach_memory(segment_id);
	save_segment_id( "shared_memory_segment_id.cfg" );
	ipc_print_mem();
*/
	// TEST SHARED MEMORY with ATILTCAM
	/* while (1)
	{	
		delay(1000);
	} */
	printf("===============================================\n");	

	Preferences  pref((char*)"config.ini");
	if (pref.file_exists()==FALSE)
	{
		ci.servo1.MinAngle 				= -60.0;
		ci.servo1.MaxAngle 				= +60.0;
		ci.servo1.Min_Pulsewidth_ms 	=   0.9;
		ci.servo1.Max_Pulsewidth_ms		=   2.1;
		ci.servo1.Neutral_Pulsewidth_ms =   1.5;

		ci.servo2.MinAngle 				= -60.0;
		ci.servo2.MaxAngle 				= +60.0;
		ci.servo2.Min_Pulsewidth_ms 	=   0.9;
		ci.servo2.Max_Pulsewidth_ms		=   2.1;
		ci.servo2.Neutral_Pulsewidth_ms =   1.5;
	}
	else 
	{		
		pref.open(TRUE);
		pref.readln_nb();		ci.servo1.MinAngle  		 = pref.getFloatValue();
		pref.readln_nb();		ci.servo1.MaxAngle  		 = pref.getFloatValue();		
		pref.readln_nb();		ci.servo1.Min_Pulsewidth_ms  = pref.getFloatValue();
		pref.readln_nb();		ci.servo1.Max_Pulsewidth_ms  = pref.getFloatValue();
		pref.readln_nb();		ci.servo1.Neutral_Pulsewidth_ms = pref.getFloatValue();

		pref.readln_nb();		ci.servo1.MinAngle  		 = pref.getFloatValue();
		pref.readln_nb();		ci.servo1.MaxAngle  		 = pref.getFloatValue();		
		pref.readln_nb();		ci.servo1.Min_Pulsewidth_ms  = pref.getFloatValue();
		pref.readln_nb();		ci.servo1.Max_Pulsewidth_ms  = pref.getFloatValue();
		pref.readln_nb();		ci.servo1.Neutral_Pulsewidth_ms = pref.getFloatValue();		
	}
	//=====================================================================

	while (1) 
	{
		// Read & Write to Shared Memory (shared with bkInstant)
		update_variables( );
	
		// Send & Receive over SPI PiCamScan board.	
		send_atmel_messages();

	    //test_command( cmd );
		ipc_print_mem();

		// Pause
		delay(500);
	};	
}

