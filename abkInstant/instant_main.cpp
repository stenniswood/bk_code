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
#include <pthread.h>
#include "pican_defines.h"
#include "CAN_Interface.hpp"
#include "can_txbuff.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "catagorization.h"
#include "serverthread.h"
#include "client.h"
#include "visual_memory.h"
#include "sway_memory.h"
#include "audio_memory.h"
#include "picamscan_memory.h"
#include "client_memory.hpp"
#include "CAN_memory.h"
#include "udp_transponder.hpp"
#include "thread_control.h"
#include "client_to_socket.h"


// Right now this is user settable.  Need to detect:
BOOL	PiCAN_Present=FALSE;

struct sCAN MMsg;
byte 		count = 0;
int  			fd;
long  		SerialNumber_ = 0x56789CD;
#define 	Debug 0
pthread_t 	udp_tx_thread_id;
pthread_t 	udp_rx_thread_id;
pthread_t 	server_thread_id;

// SET THIS IN serverThread Parse whatever.
int create_client_thread_requested = 0;		// from a serverthread which received a request to connect to another RPI.


void create_threads()
{
	/* These threads are located in :   	*/

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
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
		exit(EXIT_FAILURE);
	}
	// CLIENT PROTOCOL... thread needed?  No.
}



void create_another_client_thread()		// really serverthread which initiates connection (ie. client thread)
{
	// TBD!!!	
	const char *message1 = "client blah";
	// ETHERNET SERVER PROTOCOL:
	int iret2 = pthread_create( &server_thread_id, NULL, server_thread, (void*) message1);
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
#define USE_AUDIO	0

/* Inter Process Communications (via shared memory)		
*/
void establish_ipc()
{
	printf("************************* SHARED MEMORY *****************************\n");
	// Allocate a shared memory so we can accept user commands (ie. connect, send audio, etc)
	int result = connect_shared_client_memory(TRUE);

	if (USE_AVISUAL)
	{
		printf("************************* AVISUAL SHARED MEMORY *****************************\n");
		vis_allocate_memory();
		vis_attach_memory  ();
		vis_fill_memory	   ();
	}
	//else vis_deallocate_memory( 98307 );

	if (USE_SWAY)
	{
		printf("************************* SWAY SHARED MEMORY *****************************\n");	
		sway_allocate_memory();
		sway_attach_memory  ();
		sway_fill_memory	();
	}
	//	else sway_deallocate_memory();

	if (USE_PICAMSCAN) 
	{
		printf("************************* PICAMSCAN SHARED MEMORY *****************************\n");	
		picam_allocate_memory();
		picam_attach_memory  ();
		picam_fill_memory	 ();
	} 
//	else picam_deallocate_memory(  );	// 98307
	
	if (USE_AUDIO)
	{
		printf("************************* AUDIO SHARED MEMORY *****************************\n");	
	    result =  audio_connect_shared_memory(FALSE);
	}  

	if (USE_CAN)
	{
		printf("************************* CAN SHARED MEMORY *****************************\n");	
	    result =  can_connect_shared_memory(FALSE);
	} 
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

	printf("done with board request()!\n");
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


#if (PLATFORM==Mac)
char amon_command[] = "~/bk_code/amonitor/amon";
#elif (PLATFORM==RPI)
char amon_command[] = "sudo /home/pi/bk_code/amonitor/amon > /dev/null";
#elif (PLATFORM==linux_desktop)
char amon_command[] = "sudo /home/steve/bk_code/amonitor/amon";
#endif

/*int start_amon() 
{
    int pid;
    switch (pid=fork()) {
        case -1:
			printf("fork() = -1\n");
            return 0;
        case 0:
            execvp(amon_command, NULL);
            printf("returned from ececvp\n");
        default: printf("unknown start_amon pid!\n");
            return 0;
    }
    return 1;
}*/


/* Client request shared memory.  */
void scan_client()
{
	// CHECK CLIENT MEMORY FOR REQUEST:
	if (cli_is_new_update())
	{
		handle_client_request();
		cli_ack_update_status();
		printf(" acknowledged to client memory. \n");
	}	
}
		
void update_outputs()
{
	// Update Client List:
/*	int size = ipc_memory_client->NumberClients;
	for (int i=0; i<size; i++)
		cli_ipc_add_new_client( mClientText );		
*/	
}

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

	printf("================= Main Loop ==========================\n");
	int count = 0;
	sleep(1);
	while (1) 
	{
		//voice_interface();		// empty
		scan_client();
		usleep(50000);
	}
}

