/* BK Instant main file.  */
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string>
#include <vector>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <pthread.h>
#include "pican_defines.h"
#include "CAN_Interface.hpp"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "can_txbuff.h"
#include "packer.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "catagorization.h"
#include "serverthread.h"
#include "client.h"
#include "can_rxbuff.h"
#include "visual_memory.h"
#include "sway_memory.h"
#include "audio_memory.h"
#include "picamscan_memory.h"
#include "udp_transponder.hpp"
#include "thread_control.h"


// Right now this is user settable.  Need to detect:
BOOL	PiCAN_Present=FALSE;


struct sCAN MMsg;
byte count = 0;
int  	fd;
long  	SerialNumber_ = 0x56789CD;
#define Debug 0
pthread_t udp_tx_thread_id;
pthread_t udp_rx_thread_id;
pthread_t server_thread_id;
  
void create_threads()
{	
	// CREATE UDP - Transponder :
	const char *message1 = "every second";
	int iret1 = pthread_create( &udp_tx_thread_id, NULL, udp_transponder_function, (void*) message1);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}

	// UDP Receiver : 
	int iret2 = pthread_create( &udp_rx_thread_id, NULL, udp_receive_function, (void*) message1);
	if (iret2)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}

	// ETHERNET SERVER PROTOCOL:
	iret2 = pthread_create( &server_thread_id, NULL, server_thread, (void*) message1);
	if (iret2)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
	// CLIENT PROTOCOL... thread needed?  No.
}

#define USE_AVISUAL 1
#define USE_SWAY	1
#define USE_PICAMSCAN	0
#define USE_AUDIO	1

/* Inter Process Communications (via shared memory)		*/
void establish_ipc()
{
	if (USE_AVISUAL)
	{
		vis_allocate_memory();
		vis_attach_memory  ();
		vis_fill_memory	   ();
		//dump_line();
	} else 
		vis_deallocate_memory( 98307 );	

	if (USE_SWAY)
	{
		sway_allocate_memory();
		sway_attach_memory  (); 
		sway_fill_memory	();
	} else 
		sway_deallocate_memory();	

	if (USE_PICAMSCAN) 
	{
		vis_allocate_memory();
		vis_attach_memory  ();
		vis_fill_memory	   ();
		//dump_line();
	} else 
		vis_deallocate_memory( 98307 );	
		
	if (USE_AUDIO)
	{
		aud_allocate_memory();
		aud_attach_memory  (); 
		aud_fill_memory	   ();
	} else 
		aud_deallocate_memory();

}

void init( )
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
	button_init ( );
	init_leds   ( );
	//fuse_init ( );

	// Button boards set to 0x0E, 0x04, 0x13 which is 250 Kbps :

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
	create_threads();
	establish_ipc();

	//CAN_init( CANSPEED_250, 0 );	
	/* The CAN_isr() callbacks are not recommended.  They're only for 
	   quick routines.  The messages are queued and retrieved in the background! 
	//set_system_rx_callback( callback_board_presence );
	//set_model_rx_callback( callback_main );				*/
	//bl.RequestBoardsPresent( REQUEST_TYPE_PRESENCE );

	printf("done with board request()!\n");

	//printf("\nPiCAN Board present & Responding.\n" );
	//printf("*****************************************\n");
}
  
void help()
{
	printf("bk Instant! (Media Center) \n"								);
	printf(" 	\n"														);
	printf("This app transmits a beacon udp message. Waiting for client to connect to it.  \n");
	printf(" Once established, it sends receives telegrams.\n"			);
	printf(" Each transaction is recorded in a database.	\n"			);
	
	printf(" Other processes may conect to this via interprocess communications of linux.	\n"	);	
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

/* 	Interface between Command Central and CAN
	Process and Transmit CAN messages 
*/
void can_interface()
{
	// PUMP TRANSMIT MESSAGE QUEUE:
/*	if (ReadyToSendAnother)
	{
		SendNext();		// Transmit next can message
	}
	// HANDLE ANY RX MESSAGES:
	while (RxMessageAvailable())
	{
		struct sCAN* tmp = GetNext();
		//callback_main( tmp );
	}		*/
}

void ethernet_interface()
{	/* wifi comms */ }
void voice_interface()
{	/* NLP/Synthesis */ }

char filespath[64];
char filename[64];
/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.		*/
int main( int argc, char *argv[] )
{
	print_args( argc, argv );
	int first_param = 1;		// sudo ./pican cfg 
	int value    	= 0;
	if (argc>1)
	{
		if ((strcmp(argv[first_param], "help") == 0))
		{
			help();
			return 0;
		}
	}
	init();		
	printf("===============================================\n");		

	// FORCE CLIENT CONNECTION:
	TransportCAN = FALSE;
	if (argc>1)
	{
		int error = connect_to_robot(argv[1]);
		if (error==0)
		{
			WifiConnected = TRUE;
			TransportCAN  = TRUE;
			send_file_transmit_request( );
			strcpy(filespath, "/home/pi/abkInstant/media/");
			strcpy(filename, "test" );
			create_file_tx_thread( argv[1], filespath, filename, BASE_FILE_PORT );
			//Cmd_client_CAN_Start( );
		}
	}
	printf("================= Checking command line arguments ========================\n");	

	printf("================= Main Loop ==========================\n");
	while (1) 
	{
		can_interface();
		ethernet_interface();
		voice_interface();
	}
}

