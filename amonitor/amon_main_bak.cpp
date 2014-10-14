#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>

#include "gpio.h"
#include "mcp2515.h"
#include "bk_system_defs.h"
#include "pican_defines.h"
#include "can_eid.h"
#include "CAN_Interface.hpp"
#include "can_txbuff.hpp"
#include "can_rxbuff.hpp"
#include "pican_defines.h"

#include "gpio.h"
#include "packer_lcd.h"
#include "packer.h"
#include "packer_motor.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "chevy_codes.h" 

#include "ini_file.hpp"		// Preferences
#include "filter_file.hpp"


int can_speed = CANSPEED_500;


void tx_register_dump( byte mBuffer)
{
	byte bank = (mBuffer<<4);
	read_register( TXB0CTRL+bank );
	read_register( TXB0SIDH+bank );
	read_register( TXB0SIDL+bank );	
	read_register( TXB0EID8+bank );	
	read_register( TXB0EID0+bank );	
	read_register( TXB2DLC+bank  );
	read_register( TXB2D0+bank   );
	for (int i=0; i<8; i++)
	read_register( TXB2D0+i+bank );
}
void register_dump()
{
	// READ CAN:
	read_register( CANINTE );
	read_register( CANINTF );
	read_register( CANSTAT );
	read_register( CANCTRL );
	read_register( EFLG );
	read_register( TEC );
	read_register( REC );
	read_register( TXRTSCTRL );
	read_register( RXB0CTRL );
	read_register( RXB1CTRL );
	read_register( TXB0CTRL );
	read_register( TXB1CTRL );
	read_register( TXB2CTRL );
	if (0)
	{
		read_register( CNF1 );
		read_register( CNF2 );
		read_register( CNF3 );
	}
}

// Wiring PI pin number (gpio pin 15)
#define CAN_INT_PIN 3
#define BUTTON1_INT_PIN 4
#define BUTTON2_INT_PIN 5
#define BUTTON3_INT_PIN 6

void Button1_isr()
{
	printf("Button 1 Pressed Interrupt - reg_dump()\n");
	register_dump   ( );
	tx_register_dump(0);	
} 
void Button2_isr()
{
	printf("Button 2 Pressed Interrupt\n");	
	printf("--------------->>>>>>>\n");
	char txt[] = {"hello!!!"};
	//pack_lcd_write_text( &msg1, 0, txt);
	//AddToSendList( &msg1 );
	printf("--------------->>>>>>>\n");
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
  
short MsgCounter   = 0;
byte  MsgIDCounter = 0;
// Each module sends a message to identify itself (type of device, and 
// SerialNumber)
const byte AdrenalineEdgeID  = 0x01;
const byte AdrenalineBlueID  = 0x02;
const byte AdrenalinePWMID   = 0x03;
const byte AdrenalinePowerID = 0x04;
const byte AdrenalineRawID   = 0x05;
long  SerialNumber_      	 = 0x56789CD;


void read_cnfs()
{
	printf( "\tCNF1: " );
	int result = read_register( CNF1 );
	printf("\tCNF2: " );
	result = read_register( CNF2 );
	printf("\tCNF3: " );
	result = read_register( CNF3 );
}

int counter = 0;
char user_input[255];
char cmd[4];
byte instance;
byte mask;
byte value;

void print_args(int argc, char *argv[])
{
	printf("%d ARGS: ", argc);
	for (int i=0; i< argc; i++)
	{
		printf(" %s ", argv[i] );
	}
	printf("\n");
}

#define FILTER_ARG 1
#define SYMBOL_ARG 2
#define BAUD_ARG   3

int process_baud(char* mArgv )
{
	if (strcmp(mArgv, "500k")==0)
	{
		printf("Baud = 500k \n");
		can_speed = CANSPEED_500;
	}
	else if (strcmp(mArgv, "250k")==0)
	{
		printf("Baud = 250k \n");
		can_speed = CANSPEED_250;
	}
	else if (strcmp(mArgv, "95k")==0)
	{
		printf("Baud = 250k \n");
		can_speed = CANSPEED_95;
	}
	else if (strcmp(mArgv, "33k")==0)
	{
		printf("Baud = 33.33k \n");
		can_speed = CANSPEED_250;
	}
	return can_speed;
}

void filter( char* mFilename)
{
	printf("Reading Filter file: %s \n", mFilename );			
	FilterFile filt( mFilename  );
	filt.Read_n_Add(         );
}
void symbol_file(char* mFilename)
{
	printf("Reading Filter file: %s \n", mFilename );			
	//SymbolFile symbol( mArgv   );
	//symbol.Read_n_Add(         );
}

int process_argument( char* mArgv, byte Argument_type )
{
	switch( Argument_type )
	{
	case FILTER_ARG:
			filter( mArgv );
		break;
			
	case SYMBOL_ARG:
			symbol_file( mArgv );
		break;	
		
	case BAUD_ARG:
			process_baud( mArgv );
		break;
	
	default: break;
	}
}

int get_argument_type( char* mArgv )
{
	switch (mArgv[0])
	{
	case 'f':		// Filter File.
	case 'F':
			return FILTER_ARG;
			break;
				
	case 's':		// Symbol File.
	case 'S':
			return SYMBOL_ARG;
			break;

	case 'b':		// Baud Rate
	case 'B':
			return BAUD_ARG;	
			break;
	
	default: return 0; break;	
	}
}

/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.
*/
int main( int argc, char *argv[] )
{
	print_args( argc, argv );
	

	for (int i=1; i<argc; i++)
	{
		byte type = get_argument_type(argv[i]);
		process_argument( argv[i], type );			
	}
		
	init(can_speed);
	printf("===============================================\n");
	//register_dump();
	printf("===============================================\n");	
	while (1) { 
		if (isRxMessageAvailable())
			print_message( GetNextRxMsg() );
		delay(5);
	};	
}

