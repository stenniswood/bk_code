#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <signal.h>
#include <time.h>

#include "gpio.h"
#include "mcp2515.h"
#include "pican_defines.h"
#include "gpio.h"
#include "CAN_Interface.h"
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
#include "timer.h"
#include "config_file.h"

// Wiring PI pin number (gpio pin 15)
#define CAN_INT_PIN 3
#define BUTTON1_INT_PIN 4
#define BUTTON2_INT_PIN 5
#define BUTTON3_INT_PIN 6

#define fifty_ms    50000000

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

int counter = 0;
char user_input[255];
char cmd[4];
byte instance;
byte value;


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

/*void Button1_isr()
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
	pack_lcd_write_text( &msg1, 0, txt);
	AddToSendList( &msg1 );
	printf("--------------->>>>>>>\n");
} 
void Button3_isr()
{
	printf("Button 3 Pressed Interrupt\n");
} */


/* Set Callback rate at 20Hz */
void next_sequence_handler(int sig, siginfo_t *si, void *uc)
{
	counter++;
	if ((counter%20)==0)  // Every Second, update Thrust Reqested
	{
		// printf( "SEQ: timeslice Sending Motor Sequence. c=%d\n", counter);
		for (int i=0; i<ConfigInfo.Number_of_appendages; i++)
			send_speed_vector( i );		
		// or send_angle_vector()
	}
}

void init_hardware()
{
	int result = wiringPiSetup();
	// Ready-to-send, Receive buffer full
	pinMode		( CAN_INT_PIN, INPUT );
	pinMode		( RX0BF,  INPUT 	 );
	pinMode		( RX1BF,  INPUT  );
	pinMode		( TX0RTS, OUTPUT );
	pinMode		( TX1RTS, OUTPUT );
	pinMode		( TX2RTS, OUTPUT );	
	digitalWrite( TX0RTS, 1 );
	digitalWrite( TX1RTS, 1 );
	digitalWrite( TX2RTS, 1 );
}
void init_interrupts()
{
	set_system_rx_callback( callback_board_presence 	 );
	set_model_rx_callback ( can_motor_position_responder );
	set_rx_vector_refreshed_callback( can_vector_read_complete_responder );
	
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

	//printf("\nPiCAN Board present & Responding.\n" );
	//printf("*****************************************\n");
	write_register( CANINTE,  0x1F );	// enable 2 TXs & 1 RX	
	write_register( CANCTRL,  0x10 );	// Abort any transmitting msgs...
	write_register( CANCTRL,  0x00 );	// 
}

void init()
{
	init_appendages();
	init_hardware();
	read_config();
	read_vector_file("vectors.ini");

	// Button boards set to 0x0E, 0x04, 0x13 which is 250 Kbps :
	Init( CANSPEED_250, 0 );
	read_register( CANSTAT );
	init_interrupts();

	// SETUP OS timer:
	set_handler ( &sa,  next_sequence_handler );
	create_timer( &sev, &timerid 			  );
	start_timer ( &its, fifty_ms, timerid     );

	printf("Finished Init!\n");
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


void help()
{
	printf("\nAdrenaline Motion Sequencer!   \n\n"				   );
	printf(" The configure file specifies number of vectors, instance numbers, and calibrated stops.\n"	);
	printf(" (config file is \"sequencer.ini\")\n"				   );

	printf("\"help\"\t\tShow this text.\n"	 						  );	
	printf("\"report\"\tConfigure BigMotor boards to report their:\n" );
	printf("\t[rate:1,2,4,8]\tTime period between reports:  10, 20, 50, 100 ms.\n"	 );
	printf("\t[report type: none,value,angle]\n"	 		);
	printf("\t\tvalue\t: Potentiometer Values\n"	 		);
	printf("\t\tangle\t: Calibrated angles\n"	 			);
	printf("\t\tnone\t: Turn off BigMotor reports\n"		);

	printf("\"calibrate\"||\"set_stop\"\t\tConfigure a BigMotor stop angle:.\n"	 				 		);
	printf("\t[instance]\t\tInstance number of the board to calibrate.\n"	 			);
	printf("\t[stop number:0,1]\tStop number to calibrate.\n"	 					);
	printf("\t[float:Angle in Degrees] Angle that the stop corresponds to (degrees).\n"	 			);
	printf("\t[word: Pot Value]\tPotentiometer Value for the angle (-1 indicates current Pot value)\n");
	
	printf("\"run\"\tSpecifies instance numbers & number of vectors.\n"	 			);	
	printf("\t[sequence file]\t\tSpecifies .csv file with vector samples.\n"			);
	printf("	\t(default=\"sequence.csv\") \n"	 								);		
	printf("\t[resample iterations]\tSpecifies n number of resampling iterations.\n"	);
	printf("					  Actual rate will be 2^n number of samples.\n"  	);
	printf("\t[repeat times]\t\tSpecifies how many times the sequence should be repeated.\n");
	printf("			  \t use -1 for infinite.\n"									  );
	printf("\n"	 );
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

// Taken from "AdrenalineFirmware\BigMotorEn\motor.h"
#define MODE_SEND_UPDATES_NONE	0x00	// 
#define MODE_SEND_UPDATES_10ms	0x10	// 
#define MODE_SEND_UPDATES_20ms	0x20	// 
#define MODE_SEND_UPDATES_50ms	0x40	// 
#define MODE_SEND_UPDATES_100ms	0x80	// 
#define MODE_SEND_POSITION_RAW	0x01 	// Measured Pot/Encoder, CurrentLeft, CurrentRight, Speed
#define MODE_SEND_POSITION_CALC 0x02 	// Calculated quantities: Angle (deg*100), Current (Amps*100)
#define MODE_SEND_STATUS		0x04 	// 


/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.
*/
int main( int argc, char *argv[] )
{
	word ms=0;

	char* SequenceFileName = "sequence.csv";

	print_args( argc, argv );
	int first_param = 1;		// sudo ./pican cfg
	int value    	= 0;
	int resample_iterations = 0;
	int repeat_iterations   = -1;
	init();

	if (argc > 1)
	{
		if ((strcmp(argv[1], "help") == 0) )
		{
				help();
				return 0;
		}		
		printf("READING CONFIGURE...\n");
		//read_config( ConfigFileName );

		if ( (strcmp(argv[1], "report") == 0) )
		{
			if (argc>2) 					// Get report rate:
			{				
				ms = atoi(argv[2]) << 4;		// number of milliseconds
				if ((ms>>4)>8) { printf ("Rate is invalid!"); return 0; }
				/* must be one 1 4 predefined:  */
			}
			if (argc>3)
			{
				printf("Configuring reporting : %s rate=%x;\n", argv[3], ms);
				if (strcmp(argv[3], "value") == 0) 
				{ 
					configure_motor_reports( 0, ms, MODE_SEND_POSITION_RAW );				
				}
				if (strcmp(argv[3], "angle") == 0) 
				{
					configure_motor_reports( 0, ms, MODE_SEND_POSITION_CALC );
				}
				if (strcmp(argv[3], "status") == 0) 
				{
					configure_motor_reports( 0, ms, MODE_SEND_STATUS );
				}
				if (strcmp(argv[3], "none") == 0) 
				{
					configure_motor_reports( 0, ms, MODE_SEND_UPDATES_NONE );
				}
				return 1;	
			}
		}
		if ( (strcmp(argv[1], "set_stop") == 0) ||
		     (strcmp(argv[1], "calibrate") == 0) )		
		{
			if (argc>2)
			{
				byte instance = atoi( argv[2] );
				if (argc>3)
				{
					byte stop_number = atoi( argv[3] );
					if (argc>4)
					{
						float angleDegrees = atof( argv[4] );
						if (argc>5)
						{
							word value = atoi( argv[5] );					
				    		pack_mark_motor_stop( &msg1, instance, stop_number, angleDegrees, value );						 		  
						 	print_message( &msg1 );
							AddToSendList( &msg1 );	  
						}
					}						 	
				}
			}
		}	// end of "set_stop"	

		if ( (strcmp(argv[1], "read_stop") == 0) ||
		     (strcmp(argv[1], "read") == 0) )		
		{
			byte stop  = atoi( argv[2] );							
			word value = atoi( argv[3] );					
			//pack_mark_motor_stop( &msg1, instance, stop, angleDegrees, value );		
			print_message( &msg1 );
			AddToSendList( &msg1 );	  		
		}

		if ( strcmp(argv[1], "run") == 0 )
		{
			if (argc > 2)
			{
				printf("Reading Sequence File...\n");
				//	read_sequence( argv[2] );			
				if (argc > 3)
				{
					resample_iterations = atoi(argv[3]);
					printf("Resampling %d iterations\n", resample_iterations );
					//	read_sequence( argv[3] );
					if (argc > 4)
					{
						repeat_iterations = atoi(argv[4]);
						printf("Repeating %d iterations\n", repeat_iterations );
						//	resample( argv[4] );
						printf("====================looping===========================\n");
						while (repeat_iterations>=-1) 
						{ 
							if (repeat_iterations>0) repeat_iterations--; 	// if -1 don't change!
						}
					}
				}
			}
		}	// end of run

	}

}

