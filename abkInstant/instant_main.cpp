/* BK Instant main file. 

Install MySQL (on RPI):
	sudo apt-get install mysqlserver mysqlclient
	sudo apt-get install mysql-server
	sudo apt-get install libmysqlclient-dev

Starting Mysql on Mac:
	sudo /usr/local/mysql/support-files/mysql.server start
	

 */
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string>
#include <vector>
#include <pthread.h>
#include <stdint.h>
#include <signal.h>

#include "ordering_protocol.hpp"	// this has to go before something which redefines max()
#include "serverthread.hpp"

#if (PLATFORM==RPI)
#include "bcm_host.h"
#include "ilclient.h"
#include "pican_defines.h"
#endif

#include "udp_transponder.hpp"
#include "listen_thread.hpp"
#include "serverthread.hpp"
#include "CAN_Interface.hpp"
#include "can_id_list.h"
#include "cmd_process.h"

/*** IPC Includes ***/
#include "visual_memory.hpp"
#include "sway_memory.h"
#include "audio_memory.h"
#include "picamscan_memory.h"
#include "client_memory.hpp"
#include "simulator_memory.h"
#include "CAN_memory.h"
/*** END IPC Includes ***/

#include "GENERAL_protocol.hpp"
#include "client_to_socket.hpp"
#include "tone_generator.h"
#include "sql_users.hpp"
#include "viki_logger.hpp"

#include "protocol_unit_tests.hpp"
#include "serverthread.hpp"
#include "sequencer_memory.hpp"
#include "sql_device.hpp"


aVisualMemory avisual_mem;
SequencerIPC  sequencer_mem;

#define  uint32_t long int

//#include "AUDIO_device.hpp"


// Right now this is user settable.  Need to detect:
BOOL		PiCAN_Present=FALSE;

struct sCAN MMsg;
byte 		count = 0;
int  		fd;
long  		SerialNumber_ = 0x56789CD;
#define 	Debug 0
pthread_t 	udp_tx_thread_id;
pthread_t 	udp_rx_thread_id;
pthread_t 	server_thread_id;

// SET THIS IN serverThread Parse whatever.
int create_client_thread_requested = 0;		// from a serverthread which received a request to connect to another RPI.


void create_threads()
{
	const char *message1 = "every second";
	/* These threads are located in :   	

	// CREATE UDP - Transponder : 
	
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
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
		exit(EXIT_FAILURE);
	}
*/
	// ETHERNET SERVER PROTOCOL:
	int iret3 = pthread_create( &server_thread_id, NULL, listen_thread, (void*) message1);
	if (iret3)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret3);
		exit(EXIT_FAILURE);
	}
	// CLIENT PROTOCOL... thread needed?  No.
}



void create_another_client_thread()		// really serverthread which initiates connection (ie. client thread)
{
	// TBD!!!	
	const char *message1 = "client blah";
	// ETHERNET SERVER PROTOCOL:
	int iret2 = pthread_create( &server_thread_id, NULL, listen_thread, (void*) message1);
	if (iret2)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
		exit(EXIT_FAILURE);
	}
}

// WHICH IPC MEMORY TO SETUP ON STARTUP?
#define USE_AVISUAL 0
#define USE_CAN		1
#define USE_SWAY	0
#define USE_PICAMSCAN	0
#define USE_AUDIO	1

int bkInstant_connected = FALSE;
int can_connected = FALSE;
int audio_mem_connected = FALSE;			// audio memory is not necessary to hear audio.

/* Inter Process Communications (via shared memory)		
*/
void establish_ipc()
{
	printf("************************* SHARED MEMORY *****************************\n");
	// Always have a Client connection, so we can accept user commands (ie. connect, send audio, etc)
	bkInstant_connected = connect_shared_client_memory(TRUE);
	if (bkInstant_connected)
		printf("IPC Client - attached to memory.\n");
	else
		printf("IPC Client - unavailable.\n");
	
	//printf("Update/Ack Counters = %d/%d\n", ipc_memory_client->UpdateCounter, ipc_memory_client->AcknowledgedCounter);	

	if (USE_AVISUAL)
	{
		//printf("************************* AVISUAL SHARED MEMORY *****************************\n");

	}
	//else vis_deallocate_memory( 98307 );

	if (USE_SWAY)
	{
		//printf("************************* SWAY SHARED MEMORY *****************************\n");	
		sway_allocate_memory();
		sway_attach_memory  ();
		sway_fill_memory	();
	}
	//	else sway_deallocate_memory();

	if (USE_PICAMSCAN) 
	{
		//printf("************************* PICAMSCAN SHARED MEMORY *****************************\n");	
		picam_allocate_memory();
		picam_attach_memory  ();
		picam_fill_memory	 ();
	}
//	else picam_deallocate_memory(  );	// 98307
	
	if (USE_AUDIO)
	{
		//printf("************************* AUDIO SHARED MEMORY *****************************\n");	
	    audio_mem_connected =  audio_connect_shared_memory(TRUE);
	    if (audio_mem_connected)
			printf("IPC Audio - attached to memory.\n");
		else
			printf("IPC Audio - unavailable.\n");

	}

	if (USE_CAN)
	{
		//printf("************************* CAN SHARED MEMORY *****************************\n");	
	    can_connected =  can_connect_shared_memory(FALSE);
	    if (can_connected)
			printf("IPC CAN - attached to memory.\n");
		else
			printf("IPC CAN - unavailable.\n");
	}
	
    
    int sim_available = connect_shared_simulator_memory( FALSE );
    if (sim_available)
		printf("IPC Simulator - attached to memory.\n");
	else
		printf("IPC Simulator - unavailable.\n");    
    //printf("************************* SIMULATOR SHARED MEMORY *****************************\n");
    
	printf("****************** END SHARED MEMORY SETUP *******************");	
}

void init( )
{	
	//fuse_init ( );
	establish_ipc();

	// Button boards set to 0x0E, 0x04, 0x13 which is 250 Kbps :

	// set Pin 17/0 generate an interrupt on high-to-low transitions
	// and attach myInterrupt() to the interrupt
	create_threads();

	init_client_request();
	printf("done with init()!\n");
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

ServerHandler cli_app_sh;

/* Client request shared memory.  

	Two Different ways of connecting to Instant:
		a) Client app (uses cli shared member)
		b) TCP/IP client connect 
*/
void scan_client()
{
	// CHECK CLIENT MEMORY FOR REQUEST:
	if (is_client_ipc_memory_available())
	if (cli_is_new_update())
	{
		printf("is new update.\n");
		handle_client_request(&cli_app_sh);
		cli_ipc_write_response( cli_app_sh.NLP_Response.c_str(), "Instant Server" );

		// Now wait until our response is taken.  Don't want to reprocess it on 
		// next pass thru here!
		//cli_wait_for_ack_update();		// should be a timeout on this though
	}	
}

//#include "calendar_entry.hpp"
//#include "menu_items.hpp"

void close_all_dbases()
{
	printf("\tClosing mysql connections!\n");
	sql_users.close_connection();	
//	close_menus_db();
//	close_menu_items_db();
//	close_calendar_db();
}
void close_all_sockets( )
{
	printf("\tClosing sockets!\n");
	//close_client_sockets();
	close_listen_socket( );
}
void shutdown( int sig )
{
	ShutDownConnections = true;
	printf("Program Terminating :\n");
	close_all_sockets();
	close_all_dbases();
	exit(0);	
}

/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.		*/


int main( int argc, char *argv[] )
{
	signal(SIGINT, shutdown); 	// in Ctrl-C event.
	avisual_mem.connect_shared_memory(FALSE); 
	sequencer_mem.connect_shared_memory(FALSE); 	

	print_args( argc, argv );
	int first_param = 1;		// sudo ./pican cfg 
	if (argc>1)
	{
		if ((strcmp(argv[first_param], "help") == 0))
		{
			help();
			return 0;
		}
		else if ((strcmp(argv[first_param], "viki") == 0))
		{
			char ip[26];
			int result = hostname_to_ip  ( "www.beyond-kinetics.com", ip );
			result= connect_to_robot( ip );
		} else {
			int result= connect_to_robot( argv[first_param] );
		}		
	}
	
//	viki_logger.connect_to_logger_db();
//	test_devices_db();
//	exit(1);
	
	//protocol_unit_tests();		// will exit the app when done testing!
		
    Init_General_Protocol();
	init();	// <-- Serverthread is established
	
	printf("===============================================\n");
	if (argc>1)
	{
		// TEST CLIENT CONNECTION :
		/*
			WifiConnected = TRUE;
			TransportCAN  = TRUE;
			send_file_transmit_request( );
			strcpy(filespath, "/home/pi/abkInstant/media/");
			strcpy(filename, "test" );
			create_file_tx_thread( argv[1], filespath, filename, BASE_FILE_PORT );
		*/
	}
	printf("================= Checking command line arguments ========================\n");		


#define N_WAVES  1024*32    // dimension of Sinewave[]

/*	static short Sinewaves[N_WAVES];			// 8k * 2 = 16k of short = 32k bytes buffer size. correct.
    
    int   period = get_note_period( 5 , 4, 22050 );
    saw_tooth_left ( Sinewaves, period, N_WAVES );
    period = get_note_period( 3 , 4, 22050 );
    saw_tooth_right( Sinewaves, period, N_WAVES );
    //silence_channel(Sinewaves, N_WAVES, 1);
    
	int32_t result = audio_setup( 0, 22050, 2, N_WAVES );
    for (int j=0; j<4; j++)
	{
		uint8_t* result = audio_add_play_buffer( Sinewaves, N_WAVES, 22050 );
	}
    usleep(2000000);
    audio_close();  */
	
	printf("================= Main Loop ==========================\n");	
	while (1) 
	{
		scan_client();
		usleep(50000);
	}
}
