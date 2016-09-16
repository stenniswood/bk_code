/* 
	BK Instant - client interface.  
			A very thin client for giving commands to the Instant server.

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
#include "pican_defines.h"
//#include "CAN_Interface.hpp"
//#include "packer_lcd.h"
//#include "packer_motor.h"
//#include "can_txbuff.h"
//#include "packer.h"
//#include "can_id_list.h"

#include "buttons.h"
#include "client_memory.hpp"
#include "vision_memory.h"

// Right now this is user settable.  Need to detect:
bool	server_is_vision = false;
unsigned int 	count = 0;
int  	fd;
long  	SerialNumber_ = 0x56789CD;
#define Debug 0
pthread_t udp_tx_thread_id;
pthread_t udp_rx_thread_id;
pthread_t server_thread_id;

#define USE_AVISUAL 1
#define USE_SWAY	1
#define USE_PICAMSCAN	0
#define USE_AUDIO	1

/* 
	Inter Process Communications (via shared memory)		
*/
void establish_ipc()
{
	int result=0;
	if (server_is_vision==false)
	{
		result = connect_shared_client_memory( 0 );
		if (result<=0) {
			printf("\n    You must start the Instant server first!\n\n");
			exit(1);
		}	 
		cli_print_clients();
	} else {	
		if (server_is_vision)
		{
			if (is_eyes_ipc_memory_available())
			{
				//eyes_fill_memory();
				result = eyes_connect_shared_memory( 0 );
				if (result<=0)  {
					printf("\n    You must start the xeyes server first!\n\n");
					exit(1);
				}			
			}	
		}
	}
}
  
void help()
{
	printf("bk Instant Client! (Media Center) \n"			);
	printf(" 	\n"											);
	printf(" whoami\t 	\n"									);	
	printf(" list\t\t(clients) optional	\n"					);		
	printf(" connect\t[ip address]	\n"						);	
	printf(" connect\t[beacon name]	\n"						);		
	printf(" send\t\t[data_type]	\n"						);	
	printf(" 		CAN,AUDIO,VIDEO,file,Mouse/keyboard,\n"	);
	printf(" 		robot coords,...\n"	);
	printf(" 	\n"											);	
	printf("This app transmits a message to the server.   \n");	
	printf(" Other processes may conect to this via interprocess communications of linux.	\n"	);	
}

void print_args(int argc, char *argv[])
{
	printf("ARGS: %d\n", argc);
	for (int i=0; i< argc; i++)
	{
		printf("%d: %s \n", i, argv[i] );
	}
	printf("\n");
}

/* A connection to another client must exist already.  Doesn't matter how the connection
	was established.  If Instant received it or initiaited it based on this client.
*/
void parse_send_cmds( char* mArgv[], int mArgc )
{
	// CAN,AUDIO,VIDEO,file,Mouse/keyboard:
	cli_ipc_write_sentence( mArgv[1] );
}

char working_buffer[127];

void cli_write_sentence(char* mText )
{
	cli_ipc_write_sentence( mText );
	printf("Client Request:|%s|\n", ipc_memory_client->Sentence );	
	printf("waiting for acknowledge; count=%d, ack=%d\n",ipc_memory_client->UpdateCounter, ipc_memory_client->AcknowledgedCounter  );		
	cli_wait_for_ack_update();
	printf("waiting for response...\n");
	cli_wait_for_response();		
	cli_ack_response();		
	printf("RESPONSE:  %s\n", cli_get_sentence() );
}

void vision_write_sentence(char* mText)
{
	eyes_write_client_command( mText );
	printf("Client Request:|%s|\n", ipc_memory_eyes->client_command );	
	//printf("waiting for acknowledge; count=%d, ack=%d\n",ipc_memory_eyes->CommandCounter, ipc_memory_eyes->AcknowledgeCounter );
	eyes_wait_for_acknowledgement();
	//printf("Acknowledged.  waiting for response...\n");
	eyes_wait_for_server();		
	eyes_acknowledge_server();		
	printf("RESPONSE:  %s\n", ipc_memory_eyes->ServerEvent  );
}

/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.		*/
int main( int argc, char *argv[] )
{
	print_args( argc, argv );
	printf("======================= Beyond Kinetics ========================\n");		
	int first_param = 1;
	int value    	= 0;
	if (argc>1)
	{
		if ((strcmp(argv[first_param], "help") == 0))
		{
			help();
			return 0;
		}
		if ((strcmp(argv[first_param], "avisual") == 0))
		{
			server_is_vision = false;
		}
		if ((strcmp(argv[first_param], "xeyes") == 0))
		{
			server_is_vision = true;
		}
	}
	establish_ipc();

	// PARSE ARGUMENTS : 
	if (argc>1)
	{
		if (server_is_vision)
			vision_write_sentence( argv[first_param+1] );	// xeyes
		else
			cli_write_sentence   ( argv[first_param+1] );	// avisual
	}
	
	printf("================= DONE ========================\n");
}


/*			Done in Instant.
			int error = connect_to_robot(argv[2]);
			if (error==0)
			{ } */

/*		if (strcmp(argv[1], "whoami")==0)
		{
			printf("\n\nYour beacon name is %s.\n", ipc_memory_client->OurBeaconName );
			printf("Your IP address is %s.\n", ipc_memory_client->ipAddress );
		} 
		else if (strcmp(argv[1], "list")==0)
		{
			printf("\n\n%d Instant clients available.\n", ipc_memory_client->NumberClients );
			cli_print_clients();
		}
		else if (strcmp(argv[1], "connect")==0)
		{
			strcpy( working_buffer, "connect " );
			strcat( working_buffer, argv[2]   );
			printf("\n\nConnect command:  %s\n", working_buffer );			
			cli_ipc_write_sentence( working_buffer );			
		}
		else if (strcmp(argv[1], "send")==0)
		{
			strcpy( working_buffer, "send " );
			strcat( working_buffer, argv[2] );
			printf("\n\nConnect command:  %s\n", working_buffer );			
			cli_ipc_write_sentence( working_buffer );			

			//CAN,AUDIO,VIDEO,file,Mouse/keyboard
			//parse_send_cmds(argv, argc, 2);						
		}
		else if (strcmp(argv[1], "stop")==0)
		{
			strcpy( working_buffer, "stop " );
			strcat( working_buffer, argv[2] );
			printf("\n\nConnect command:  %s\n", working_buffer );			
			cli_ipc_write_sentence( working_buffer );			

			//CAN,AUDIO,VIDEO,file,Mouse/keyboard
			//parse_send_cmds(argv, argc, 2);						
 		}  */
