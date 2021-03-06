#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <unistd.h>
#include <pthread.h>


#include "gpio.h"
#include "mcp2515_defs.h"
#include "bk_system_defs.h"
#include "pican_defines.h"
#include "can_eid.h"
#include "CAN_Interface.hpp"
#include "can_txbuff.hpp"
#include "can_rxbuff.hpp"
#include "pican_defines.h"
#include "packer.h"
#include "system_msgs_callback.h"
#include "chevy_codes.h" 
#include "ini_file.hpp"			// Preferences
#include "CAN_memory.h"
#include "OS_Dispatch.h"
#include "OS_timers.h"
#include "can_id_list.h"


// Tail for this amon app.
int RxTail_cmdline=0;
int RxTail_cmdline_laps=0;
unsigned int TxTail_cmdline=0;
unsigned int TxTail_cmdline_laps=0;

int can_speed = CANSPEED_250;

// Wiring PI pin number (gpio pin 15)
#define CAN_INT_PIN 	3
#define BUTTON1_INT_PIN 4
#define BUTTON2_INT_PIN 5
#define BUTTON3_INT_PIN 6

void Button1_isr()
{
	printf("Button 1 Pressed Interrupt - reg_dump()\n");
	register_dump   ( );
	//tx_register_dump(1);	
} 
void Button2_isr()
{
	Reset();		// CAN 
	CAN_init( can_speed, 0 );
	printf("Button 2 Pressed - Reset CAN \n");	
} 
void Button3_isr()
{
	printf("Button 3 Pressed Interrupt\n");
	printf("RxTail_cmdline,laps= %d, %d\n", RxTail_cmdline, RxTail_cmdline_laps );
	printf("TxTail_cmdline,laps= %d, %d\n", TxTail_cmdline, TxTail_cmdline_laps );	
	printf("TxHead,laps = %d, %d\n", ipc_memory_can->TxHead, ipc_memory_can->TxHeadLap );
}

void init(int CAN_Speed )
{
	int result = wiringPiSetup();
	// Ready-to-send, Receive buffer full
	pinMode		( CAN_INT_PIN, INPUT );
	pinMode		( RX0BF,  INPUT 	 );
	pinMode		( RX1BF,  INPUT 	 );
	pinMode		( TX0RTS, OUTPUT 	);
	pinMode		( TX1RTS, OUTPUT 	);
	pinMode		( TX2RTS, OUTPUT 	);	
	digitalWrite( TX0RTS, 1  	 	);
	digitalWrite( TX1RTS, 1 	 	);
	digitalWrite( TX2RTS, 1 	 	);
	gpio_init   ( );

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
	//set_model_rx_callback( parse_chevy_id );
	
	// Button boards set to 0x0E, 0x04, 0x13 which is 250 Kbps :
	CAN_init( CAN_Speed, 0 );
	read_register( CANSTAT );
	
	//printf("\nPiCAN Board present & Responding.\n" );
	//printf("*****************************************\n");
	write_register( CANINTE,  0x1F );	// enable 2 TXs & 1 RX	
	write_register( CANCTRL,  0x10 );	// Abort any transmitting msgs...
	write_register( CANCTRL,  0x00 );	// 
}

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

bool message_reject_filter( struct sCAN* message )
{
	if ( message->id.group.id == ID_ACCEL_XYZ )		return true;
	if ( message->id.group.id == ID_GYRO_XYZ  )		return true;
	if ( message->id.group.id == ID_MAGNET_XYZ )	return true;		
	return false;
}

bool instance_reject_filter( struct sCAN* message )
{
	if ( message->id.group.instance == 0 )
		return true;
	return false;
}

int main( int argc, char *argv[] )
{
	print_args( argc, argv );
	bool use_reject_filter=false;
	if (argc>1) {
		if (strcmp(argv[1], "500k")==0)
		{
			printf("Baud = 500k \n");
			can_speed = CANSPEED_500;
		}
		else if (strcmp(argv[1], "250k")==0)
		{
			printf("Baud = 250k \n");
			can_speed = CANSPEED_250;
		}
		else if (strcmp(argv[1], "95k")==0)
		{
			printf("Baud = 250k \n");
			can_speed = CANSPEED_95;
		}
		else if (strcmp(argv[1], "33k")==0)
		{
			printf("Baud = 33.33k \n");
			can_speed = CANSPEED_250;
		}
		if (argc>2)
		{
			printf("Reading Filter file: %s \n", argv[2] );			
//			FilterFile filt( argv[2] );
//			filt.Read_n_Add(         );
		}
		if (strcmp(argv[1], "reject")==0)
			use_reject_filter = true;
	}
	printf("===============================================\n");
	int result = can_connect_shared_memory(TRUE);
	init_can_memory();
	printf("===============================================\n");		
	init(can_speed);
	create_threads();
	register_dump();
	struct sCAN* message=NULL;

	int dump_count=0;	
	while (1) {
		if (shm_isRxMessageAvailable(&RxTail_cmdline, &RxTail_cmdline_laps)) 
		{
			message = shm_GetNextRxMsg(&RxTail_cmdline, &RxTail_cmdline_laps);			
			if (use_reject_filter==false)
				print_message( message );			
			else if (( instance_reject_filter(message) == false ) &&
				( message_reject_filter(message) == false ))
				print_message( message );
			dump_count++;
			if (dump_count>40096)
			{
				dump_count = 0;
				/*printf("Head: %d, %d\n", ipc_memory_can->RxHead, 
											ipc_memory_can->RxHeadLap);	
				printf("Tail: %d, %d\n", RxTail_cmdline, RxTail_cmdline_laps); */
			}
		}

		// Now Tx Messages (perhaps better as interrupt driven)
		if (shm_isTxMessageAvailable(&TxTail_cmdline, &TxTail_cmdline_laps))  {
			// pointer to 1 allocation. overwritten on next call!			
			message = shm_GetNextTxMsg  (&TxTail_cmdline, &TxTail_cmdline_laps );
			printf("TX: \t"); print_message( message );
			// wait until all messages are transmitted.  
			AddToSendList(message);
			//send_message(message);
			//can_send_msg_no_wait(0, message);	// use no wait b/c want to keep Rx msgs flowing.
		}
	};	
}
