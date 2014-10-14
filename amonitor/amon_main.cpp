#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>

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

#include "ini_file.hpp"		// Preferences
#include "filter_file.hpp"



int can_speed = CANSPEED_500;

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
	gpio_init();

	// Button boards set to 0x0E, 0x04, 0x13 which is 250 Kbps :
	CAN_init( CAN_Speed, 0 );
	read_register( CANSTAT );

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
	set_model_rx_callback( parse_chevy_id );
	
	//printf("\nPiCAN Board present & Responding.\n" );
	//printf("*****************************************\n");
	write_register( CANINTE,  0x1F );	// enable 2 TXs & 1 RX	
	write_register( CANCTRL,  0x10 );	// Abort any transmitting msgs...
	write_register( CANCTRL,  0x00 );	// 
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


int main( int argc, char *argv[] )
{
	print_args( argc, argv );
	

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
			FilterFile filt( argv[2] );
			filt.Read_n_Add(         );
		}
	}

	init(can_speed);
	printf("===============================================\n");
	//register_dump();
	//printf("===============================================\n");	
	while (1) { 
		if (isRxMessageAvailable())
			print_message( GetNextRxMsg() );
	};	
}

