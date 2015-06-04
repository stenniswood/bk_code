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

#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "catagorization.h"
#include "can_rxbuff.h"
#include "sway_memory.h"
#include "picamscan_memory.h"
#include "client_memory.h"
#include "client.h"

//#include "udp_transponder.hpp"
//#include "thread_control.h"


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
  

#define USE_AVISUAL 1
#define USE_SWAY	1
#define USE_PICAMSCAN	0
#define USE_AUDIO	1

/* 
	Inter Process Communications (via shared memory)		
*/
void establish_ipc()
{
	int result = cli_allocate_memory();
	printf("Connect allocate CLIENT MEMORY: %d \n", result );
	//client_segment_id = 98307;
	
	cli_attach_memory  (); 	
	
	print_clients();
	//cli_dump_ipc();
}

void init( )
{	
	establish_ipc();
}
  
void help()
{
	printf("bk Instant Client! (Media Center) \n"								);
	printf(" 	\n"														);
	printf("This app transmits a message to the server.   \n");	
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
	
	if (argc>1)
	{
		// FORCE CLIENT CONNECTION:
		//TransportCAN = FALSE;
		int error = connect_to_robot(argv[1]);
		if (error==0)
		{
		}
	}
	printf("================= Checking command line arguments ========================\n");	

	printf("================= Main Loop ==========================\n");


}

