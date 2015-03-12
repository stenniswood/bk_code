#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "mcp2515.h"
#include "pican_defines.h"
#include "CAN_Interface.hpp"
#include "packer_lcd.h"
#include "can_txbuff.h"
#include "packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "motor_vector.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "help.h"
#include "help_configs.h"
#include "config_options.h"
#include "OS_Dispatch.h"
#include "OS_timers.h"
#include <unistd.h>
#include <pthread.h>

// Wiring PI pin number (gpio pin 15)
#define CAN_INT_PIN 3
#define BUTTON1_INT_PIN 4
#define BUTTON2_INT_PIN 5
#define BUTTON3_INT_PIN 6


short MsgCounter   = 0;
byte  MsgIDCounter = 0;
// Each module sends a message to identify itself (type of device, and 
// SerialNumber)


// CREATE THREADS: 
pthread_t unknown_thread_id;
void* unknown_thread(void* n)
{
	while (1)
	{	
		//printf("unknown_thread()\n");
		OS_Event_TIC_Counter++;
		//System_Dispatch();
		OS_Dispatch();
		usleep( 1000 );
	}	
}

void create_threads()
{	
	// CREATE TIMER THREAD :
	const char *message1 = "second";
	int iret1 = pthread_create( &unknown_thread_id, NULL, unknown_thread, (void*) message1);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}

byte last_stop=0;
struct sEndPoint
{
	long int angle;		// FPA type;  Degrees*100  
	word 	 value;		// Pot value
} EndPoint;
word extract_word_be( byte* mData )
{
	long int retval = 0;
	retval |= (mData[0]<<8);
	retval |= mData[1];
	return retval;
}
long int extract_long_int_be( byte* mData)
{
	long int retval = 0;
	retval |= (((unsigned long)mData[0])<<24);
	retval |= (((unsigned long)mData[1])<<16);
	retval |= (mData[2]<<8);
	retval |=  mData[3];
	return retval;
}
void can_proc_set_stop_msg( sCAN* mMsg )
{
	last_stop 	   = mMsg->data[0];
	EndPoint.angle = extract_long_int_be( &(mMsg->data[1]) );
	EndPoint.value = extract_word_be	( &(mMsg->data[5]) );
}

BOOL can_msg_callback( struct sCAN* mMsg )
{
	float angle =0.;
	switch (mMsg->id.group.id)
	{
	case ID_SYSTEM_CONFIGURE : 
		print_message( mMsg );	
		return TRUE;
		break;
	case ID_SYSTEM_CONFIGURE_REPORT : 
		print_message( mMsg );	
		return TRUE;
		break;
	case ID_CALIBRATED_STOP:	
		print_message( mMsg );	
		can_proc_set_stop_msg( mMsg );
		angle = EndPoint.angle / 100.0;
		printf("Stop#%d : Angle=%6.2f; Potentiometer=%x\n", last_stop, angle, EndPoint.value );
		return TRUE;	
		break;
	default: break; 
	}
	return FALSE;
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
	//gpio_init();

	
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
	set_system_rx_callback( callback_board_presence );
	set_model_rx_callback ( can_msg_callback 		);	

	// Button boards set to 0x0E, 0x04, 0x13 which is 250 Kbps :
	write_register( CANINTE, 0x00   );
	CAN_init	  ( CANSPEED_250, 0 );

	//printf("\nPiCAN Board present & Responding.\n" );
	//printf("*****************************************\n");
	write_register( CANINTE,  0x1F );	// enable 2 TXs & 1 RX	
	write_register( CANCTRL,  0x10 );	// Abort any transmitting msgs...
	write_register( CANCTRL,  0x00 );	// 
	
	OS_InitTask();			// System Timer Thread is created here!
}

void BeforeExit()
{
	// REMOVE ISR HANDLERS:
	wiringPiISR( CAN_INT_PIN, 	  INT_EDGE_FALLING, NULL );
	wiringPiISR( BUTTON1_INT_PIN, INT_EDGE_FALLING, NULL );
	wiringPiISR( BUTTON2_INT_PIN, INT_EDGE_FALLING, NULL );
	wiringPiISR( BUTTON3_INT_PIN, INT_EDGE_FALLING, NULL );
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

void print_args(int argc, char *argv[])
{
	printf("%d ARGS: ", argc);
	for (int i=0; i< argc; i++)
	{
		printf(" %s ", argv[i] );
	}
	printf("\n");
}


/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.
*/
int main( int argc, char *argv[] )
{
	print_args( argc, argv );
	int first_param = 1;		// sudo ./pican cfg
	byte instance   = 0;
	if ((strcmp(argv[first_param], "help") == 0) || (argc<1))
	{
		help();
		return 0;
	}
	init();
	create_threads();

	//printf(" STOP MOTOR : get ID text 4B %s \n", getID_Text(0x004B) );
	printf("=====================================================\n");

	if ((strcmp(argv[first_param], "cfg") == 0) ||
		(strcmp(argv[first_param], "config") == 0) ||
		(strcmp(argv[first_param], "configure") == 0))
		{
			printf("CONFIGURING...\n");
			if (argc < (first_param+1)) {
					printf( "No instance parameter.\n"); 
					return 0;
			}
			// CONFIG HELP:
			if (strcmp(argv[first_param+1], "help")==0) {
				if (argc>(first_param+1))				
					help_config_info( argv[first_param+2] );	// extract next parameter for specific board 
				else 
					help_config_info((char*)"all");
			}			
			instance = atoi(argv[first_param+1]);

			if (strcmp(argv[first_param+2], "read")==0)
			{
				pack_configure( &msg1, instance, 0x10, 0, 0 );
			} else {
// Better to look up the index and value for each config option.
// Cmd Line may have multiple options.
//byte index = get_config_option_tilt    ( char* mOption, byte* mValue )
//byte index = get_config_option_bigmotor( char* mOption, byte* mValue );
				byte index = atoi(argv[first_param+2]);
				if (index<1) { printf("config bytes start at 1!"); return 0; };
				if (index>4) { printf("Only 4 config bytes!"); return 0; };
				byte Mask  = strtol(argv[first_param+3], NULL, 0);
				byte Value = strtol(argv[first_param+4], NULL, 0);
				pack_configure( &msg1, instance, index, Mask, Value );
			}
			print_message ( &msg1 );
			AddToSendList ( &msg1 );
	}
	else if ( (strcmp(argv[first_param], "reg") == 0) ||
			  (strcmp(argv[first_param], "regs") == 0) ||
			  (strcmp(argv[first_param], "registers") == 0) ||
			  (strcmp(argv[first_param], "reg_dump") == 0) )
	{
		register_dump();
		tx_register_dump(0);
		tx_register_dump(1);
		tx_register_dump(2);
	}
	else if (strcmp(argv[first_param], "dev") == 0)
	{
		proc_dev( argc, argv );
	}
	else if (strcmp(argv[first_param], "led") == 0)
	{
		proc_led( argc, argv, first_param );
	}
	else if (strcmp(argv[first_param], "mot") == 0)
	{
		proc_mot( argc, argv, first_param );
	}
	else if (strcmp(argv[first_param], "vec") == 0)
	{
		proc_vec( argc, argv, first_param );
	}
	else if (strcmp(argv[first_param], "instance") == 0)		
	{		
		proc_instance( argc, argv, first_param );
	} 
	else if (strcmp(argv[first_param], "lcd") == 0)
	{		
		proc_lcd( argc, argv, first_param );
	} 
	else if (strcmp(argv[first_param], "screen") == 0)	
	{
		// GET SPECIFIC INSTANCE NUMBER (255 for all):
		if (argc < (first_param+1)) {
				printf( "No instance parameter.\n"); 
				return 0;
		}
		instance = atoi(argv[first_param+1]);
		printf("SEND SCREEN...\n");
		//send_screen1( instance );
	}

	// PUMP TRANSMIT MESSAGE QUEUE:
	delay(2000);
}
