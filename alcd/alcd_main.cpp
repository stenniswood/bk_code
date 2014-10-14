#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "mcp2515.h"
#include "pican_defines.h"
#include "CAN_Interface.hpp"
#include "packer_lcd.h"
#include "can_txbuff.hpp"
#include "packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "system_msgs_callback.h"
#include "board_list.h"
//#include "motor_vector.h"
#include "can_id_list.h"
//#include "cmd_process.h"
#include "buttons.h"
#include "help.h"

BOOL can_msg_callback( struct sCAN* mMsg )
{
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
	default: break;
	}
}

void tx_register_dump(byte mBuffer)
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
	digitalWrite( TX0RTS, 1 );
	digitalWrite( TX1RTS, 1 );
	digitalWrite( TX2RTS, 1 );
	//gpio_init();

	// Button boards set to 0x0E, 0x04, 0x13 which is 250 Kbps :
	CAN_init( CANSPEED_250, 0 );
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
	//set_system_rx_callback( callback_board_presence );
	//set_model_rx_callback( can_msg_callback );	

	//printf("\nPiCAN Board present & Responding.\n" );
	//printf("*****************************************\n");
	write_register( CANINTE,  0x1F );	// enable 2 TXs & 1 RX	
	write_register( CANCTRL,  0x10 );	// Abort any transmitting msgs...
	write_register( CANCTRL,  0x00 );	// 
}

void BeforeExit()
{
	wiringPiISR( CAN_INT_PIN, 	  INT_EDGE_FALLING, NULL );
	wiringPiISR( BUTTON1_INT_PIN, INT_EDGE_FALLING, NULL );
	wiringPiISR( BUTTON2_INT_PIN, INT_EDGE_FALLING, NULL );
	wiringPiISR( BUTTON3_INT_PIN, INT_EDGE_FALLING, NULL );
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
BOOL Strobe = FALSE;
int delay_ms = 250;
int pattern = 0x04;

void first_led_operation(int index)
{
	if (Strobe)
		pack_system_led( &msg1, index, SYSTEM_LED_MODE_STROBE, 1);	
	else 
		pack_system_led( &msg1, index, SYSTEM_LED_MODE_PATTERN, pattern);	
	AddToSendList( &msg1 );
}
void last_led_operation(int index)
{
//	printf("led on %d  pattern %d\n", index, pattern);
	if (Strobe)
		pack_system_led( &msg1, index, SYSTEM_LED_MODE_STROBE, 0 );	
	else 
		pack_system_led( &msg1, index, SYSTEM_LED_MODE_PATTERN, 0);
	AddToSendList( &msg1 );
}


/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.
*/
int main( int argc, char *argv[] )
{
	print_args(argc, argv);

	tID mid;
	mid = create_CAN_eid_b( 0x04, 0x3214, 0x27 );
	//printf( "full_id=%6x\n", mid.full_id );
	printf( "id_array=%2x %2x %2x %2x\n", mid.id_array[0], mid.id_array[1], mid.id_array[2], mid.id_array[3] );	
	print_eid( &mid );
	
	int sindex = 0;
	int eindex = 255;
	print_args( argc, argv );
	int first_param = 1;		// sudo ./pican cfg
	int value    	= 0;
	byte Instance = 0;
	char* Text =0;
	byte param1=0;
	byte param2=0;
	byte param3=0;
	byte param4=0;
	byte param5=0;
	
	if ((strcmp(argv[first_param], "help") == 0) || (argc<1))
	{
			help();
			return 0;
	}
	init();
	printf("===============================================\n");

	if (argc>2) {
		Instance = atoi( argv[first_param++] );
		printf("Instance=%d\n", Instance); 
	}
	if (argc>2) {
		if (argc>3)	param1 = atoi(argv[first_param+1]);
		if (argc>4)	param2 = atoi(argv[first_param+2]);
		if (argc>5)	param3 = atoi(argv[first_param+3]);
		if (argc>6)	param4 = atoi(argv[first_param+4]);
		if (argc>7)	param5 = atoi(argv[first_param+5]);
		
		printf("Command=%s\n", argv[first_param] );
		if (strcmp(argv[first_param], "mode") == 0)	
		{		
			pack_lcd_monitor_can	( &msg1, Instance, param1 );
		}
		else if (strcmp(argv[first_param], "all_on") == 0)	
		{		
			pack_lcd_all_on_screen	( &msg1, Instance );
		}
		else if (strcmp(argv[first_param], "clear_screen") == 0)	
		{
			pack_lcd_clear_screen	( &msg1, Instance );
		}
		else if (strcmp(argv[first_param], "select_font") == 0)	
		{
			pack_lcd_select_font( &msg1, Instance, param1 );
		}
		else if (strcmp(argv[first_param], "draw_line") == 0)	
		{
			pack_lcd_draw_line( &msg1, Instance, param1, param2, param3, param4 );
		}
		else if (strcmp(argv[first_param], "clear_eol") == 0)
		{
			/*byte Line;  byte StartCol;	byte EndCol; */
			pack_lcd_clear_eol( &msg1, Instance, param1, param2, param3 );							
		}
		else if (strcmp(argv[first_param], "horiz_line") == 0)
		{
			/*byte x_row;	byte StartCol;	byte EndCol; */
			pack_lcd_draw_horizontal(&msg1, Instance, param1, param2, param3 );
		}
		else if (strcmp(argv[first_param], "vert_line") == 0)
		{
			/* byte y_col;	byte StartRow;	byte EndRow;	*/
			pack_lcd_draw_vertical( &msg1, Instance, param1, param2, param3 );
		}
		else if (strcmp(argv[first_param], "arc") == 0)
		{
			/*byte cx;  byte cy;  byte radius;   byte start;    byte end;  */
			pack_lcd_draw_arc( &msg1, Instance, param1, param2, param3, param4, param5 );
		}
		else if (strcmp(argv[first_param], "box") == 0)
		{
			/* byte RowStart; byte RowEnd, byte ColumnStart,  byte ColumnEnd; */
			pack_lcd_draw_box( &msg1, Instance, param1, param2, param3, param4 );
		}
		else if (strcmp(argv[first_param], "circle") == 0)
		{
			/*byte cx;  byte cy;   byte radius	*/
			pack_lcd_draw_circle(&msg1, Instance, param1, param2, param3 );
		}
		else if (strcmp(argv[first_param], "beep") == 0)
		{
			/* byte On_ms;	byte Off_ms;	byte NumBeeps */
			pack_lcd_beep( &msg1, Instance,  param1,  param2, param3 );
		}
		else if (strcmp(argv[first_param], "backlight") == 0)
		{

			float percent         = atof(argv[first_param+1]);
			word mPercentinTenths = round(percent * 10.);
			pack_lcd_backlight    ( &msg1, Instance,  mPercentinTenths );
		}
		else if (strcmp(argv[first_param], "contrast") == 0)
		{
			float percent = atof(argv[first_param+1]);
			word mPercentinTenths = round(percent * 10.);
			pack_lcd_contrast( &msg1, Instance, mPercentinTenths );
		}
		else if (strcmp(argv[first_param], "invert") == 0)
		{
			pack_lcd_invert_region(&msg1, Instance );
		}
		else if (strcmp(argv[first_param], "cursor_to") == 0)
		{
			/* byte mLine   byte mColumn 	byte mFont   */
			pack_lcd_cursor_to(&msg1, Instance, param1, param2, param3 );
		}
		else if (strcmp(argv[first_param], "graph") == 0)
		{
			pack_lcd_draw_graph	( &msg1, Instance );
		}
		else if (strcmp(argv[first_param], "polar") == 0)
		{
			pack_lcd_draw_polar( &msg1, Instance );		
		}
		else if (strcmp(argv[first_param], "set_window") == 0)
		{		
			pack_lcd_set_window( &msg1, Instance, param1, param2, param3, param4 );
		}
		else if (strcmp(argv[first_param], "text") == 0)
		{		
			Text = argv[3];
			int index=0;
			int length   = strlen(Text);
			int num_msgs = ceil(length / 8 );
			printf("Length=%d, num_msgs=%d\n", length, num_msgs);
			for (int i=0; i<num_msgs; i++)
			{ 
				printf("Sending: %c%c%c%c%c%c%c%c \n", 
						Text[index+0], Text[index+1], Text[index+2], Text[index+3],
						Text[index+4], Text[index+5], Text[index+6], Text[index+7]
				 	);
				pack_lcd_write_text( &msg1, Instance, &(Text[index]) );
				index += 8;
			}				
		} else 
		{
			printf("Unknown Command: %s!\n", argv[first_param]);
		}
		//printf("PACKED!!!\n");
		print_message ( &msg1 );
		AddToSendList ( &msg1 );
		SendNext();
	}

/*
		if ((strcmp(argv[first_param], "config") == 0) ||
			(strcmp(argv[first_param], "configure") == 0))
		{ }
	sindex = atoi(argv[first_param]);
	eindex = atoi(argv[first_param+1]);
	if (argc>(first_param+1))
		delay_ms = atoi(argv[first_param+2]);
	if (argc>(first_param+2))
		pattern = atoi(argv[first_param+3]); */
}



