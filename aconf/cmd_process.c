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
//#include "gpio.h"
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
#include "help.h"

const char VectorFileName[] = "vectors.ini";
#define MAX_VECTOR_SIZE 20

void send_screen1( byte minstance )
{
	pack_lcd_clear_screen( &msg1, minstance );
	AddToSendList( &msg1 );
	delay (LCD_CMD_PAUSE);

	pack_lcd_cursor_to( &msg1, minstance, 2, 2, 2);
	AddToSendList( &msg1 );
	delay (LCD_CMD_PAUSE);

	char txt[50];
	memcpy( txt,"Nancy C ", 8 );
	pack_lcd_write_text( &msg1, minstance, txt);
	AddToSendList( &msg1 );
	delay (LCD_CMD_PAUSE);
	
	pack_lcd_cursor_to( &msg1, minstance, 5, 4, 0 );
	AddToSendList( &msg1 );
	delay (LCD_CMD_PAUSE);
	
	memcpy( txt,"Briscoe", 8 );
	pack_lcd_write_text( &msg1, minstance, txt);
	AddToSendList( &msg1 );
	delay (LCD_CMD_PAUSE);
}

int proc_dev( int argc, char *argv[] )
{
	printf("DEVICE LIST...\n");		

//				printf("Requesting Device List\n");
//				FreeBoardList();
	pack_board_presence_request( &msg1, ASK_PRESENCE );
	print_message( &msg1 );	
	AddToSendList( &msg1 );
	delay(2000);
	if (argc > 2)
	{
		if (strcmp(argv[2], "model")==0)
		{
			printf("Printing Device List - Model ordered\n");
			sort_board_list( compare_boardtype );				
			print_all_boards();
		}
	} else {	
		printf("Printing Device List - Instance ordered\n");		
		sort_board_list( compare_instance );
		print_all_boards();			
	}
	return 1;
}

int proc_led( int argc, char *argv[], byte first_param )
{
	if (argc <= (first_param+1)) {
			help_led();
			return 0;
	}
	byte instance = atoi(argv[first_param+1]);

	printf("SYSTEM LEDS... instance=%d\n", instance );
	if (argc < (first_param+2)) {
			printf( "No LED mode parameter.\n"); 
			return 0;
	}
	if (strcmp(argv[first_param+2], "device") == 0) {
		printf("Device\n");
		pack_system_led( &msg1, instance, SYSTEM_LED_MODE_DEVICE, 0 );
	}
	if (strcmp(argv[first_param+2], "myInstance") == 0) {
		printf("Strobing\n");
		pack_system_led( &msg1, instance, SYSTEM_LED_MODE_MYINSTANCE, 0 );				
	} 
	else if (strcmp(argv[first_param+2], "strobe") == 0)
	{ 
		if (argc < (first_param+3)) {
				printf( "No LED on/off parameter.\n"); 
				return 0;
		}
		byte value;
		if (strcmp(argv[first_param+3], "on") == 0)
			value = 1;
		else if (strcmp(argv[first_param+3], "off") == 0)
			value = 0;
		else 
			value = atoi( argv[first_param+3] );
		pack_system_led( &msg1, instance, SYSTEM_LED_MODE_STROBE, value );
	}
	else if (strcmp(argv[first_param+2], "pattern") == 0)
	{
		if (argc < (first_param+3)) {
				printf( "No LED pattern parameter.\n"); 
				return 0;
		}
		printf("pattern: id=%d", ID_SYSTEM_LED_REQUEST );
		byte pattern = atoi( argv[first_param+3] );
		pack_system_led( &msg1, instance, SYSTEM_LED_MODE_PATTERN, pattern );
		print_message(&msg1);
	}
	AddToSendList( &msg1 );
	return 1;
}

int proc_instance( int argc, char *argv[], byte first_param )
{
	
	// GET SPECIFIC INSTANCE NUMBER (255 for all):
	if (argc <= (first_param+1)) {
			help_instance();
			printf( "No instance parameter.\n"); 
			return 0;
	}
	byte instance = atoi(argv[first_param+1]);

	// GET OPERATION TO BE DONE ON INSTANCE:
	if (argc < (first_param+2)) {
			printf( "No  operation specified.\n");
			return 0;
	}
	if ((strcmp(argv[first_param+2], "reset") == 0) || 
	    (strcmp(argv[first_param+2], "restart") == 0))
	{
		pack_instance_restart( &msg1, instance );
		AddToSendList( &msg1 );			
	} 
	else if (strcmp(argv[first_param+2], "reassign") == 0)
	{
		byte newinstance=0;
		if (argc < (first_param+3)) {
				printf( "No instance parameter.\n"); 
				return 0;
		}
		newinstance = atoi(argv[first_param+3]);

		pack_instance_reassign( &msg1, instance, newinstance );
		AddToSendList( &msg1 );			
	}
	return 1;
}

int proc_lcd( int argc, char *argv[], byte first_param )
{
	byte row; byte Start; byte End;
	
	// GET SPECIFIC INSTANCE NUMBER (255 for all):
	if (argc <= (first_param+1)) {
			help_lcd();
			return 0;
	}
	byte instance = atoi(argv[first_param+1]);

	// GET OPERATION TO BE DONE ON INSTANCE:
	if (argc < (first_param+2)) {
			printf( "No instance operation specified.\n");
			return 0;
	}
	if (strcmp(argv[first_param+2], "txt") == 0)
	{
		byte line  = atoi(argv[first_param+3]);	
		byte column= atoi(argv[first_param+4]);	
		byte font  = atoi(argv[first_param+5]);
		pack_lcd_cursor_to ( &msg1, instance, line, column, font );
		AddToSendList( &msg1 );
		delay (LCD_CMD_PAUSE);		
		pack_lcd_write_text( &msg1, instance, argv[first_param+6]);
		AddToSendList( &msg1 );
		delay (LCD_CMD_PAUSE);
	}
	else if (strcmp(argv[first_param+2], "hl") == 0)
	{
		row    = atoi(argv[first_param+3]);
		Start  = atoi(argv[first_param+4]);	
		End    = atoi(argv[first_param+5]);
		pack_lcd_draw_horizontal(&msg1, instance, row, Start, End);
		AddToSendList( &msg1 );
	}
	else if (strcmp(argv[first_param+2], "vl") == 0)
	{
		row    = atoi(argv[first_param+3]);
		Start  = atoi(argv[first_param+4]);	
		End    = atoi(argv[first_param+5]);
		pack_lcd_draw_vertical(&msg1, instance, row, Start, End);
		AddToSendList( &msg1 );		
	}
	else if (strcmp(argv[first_param+2], "clear") == 0)
	{
		pack_lcd_clear_screen	( &msg1, instance );
		AddToSendList( &msg1 );		
	}
	else if (strcmp(argv[first_param+2], "allon") == 0)
	{
		pack_lcd_all_on_screen	( &msg1, instance );
		AddToSendList( &msg1 );		
	}
	else if (strcmp(argv[first_param+2], "box") == 0)
	{
		pack_lcd_draw_vertical	(&msg1, instance, row, Start, End);
		AddToSendList( &msg1 );		
	}
	else if (strcmp(argv[first_param+2], "invert") == 0)
	{
		pack_lcd_draw_vertical	(&msg1, instance, row, Start, End);
		AddToSendList( &msg1 );		
	}
	else if (strcmp(argv[first_param+2], "screen") == 0)
	{
		send_screen1(instance);
	}
	else if (strcmp(argv[first_param+2], "back") == 0)
	{
		float percent 		 = atof( argv[first_param+3] );
		word PercentinTenths = ceil(percent * 10);
		pack_lcd_backlight(&msg1, instance, PercentinTenths );
		AddToSendList( &msg1 );
	}
	else if (strcmp(argv[first_param+2], "contrast") == 0)
	{
		float percent = atof( argv[first_param+3] );
		word PercentinTenths = ceil(percent * 10);
		pack_lcd_contrast(&msg1, instance, PercentinTenths );
		AddToSendList( &msg1 );
	}
	else if (strcmp(argv[first_param+2], "beep") == 0)
	{
		int On_ms    = atoi(argv[first_param+3] );
		int Off_ms   = atoi(argv[first_param+4] );
		int NumBeeps = atoi(argv[first_param+5] );
		pack_lcd_beep( &msg1, instance, On_ms, Off_ms, NumBeeps);
		AddToSendList( &msg1 );	
	}
}

int proc_mot( int argc, char *argv[], byte first_param )
{
	float angle;
	word  pot_value;
	float speed, speed2;
	printf("MOTOR CMD...\n");

	// GET SPECIFIC INSTANCE NUMBER (255 for all):
	if (argc <= (first_param+1)) {
			help_mot();
			return 0;
	}
	byte instance = atoi(argv[first_param+1]);

	// GET OPERATION TO BE DONE ON INSTANCE:
	if (argc < (first_param+2)) {
			printf( "No instance operation specified.\n");
			return 0;
	}
	if (strcmp(argv[first_param+2], "getmark") == 0)
	{
		byte stop_num  = atoi(argv[first_param+3]);
		pack_mark_motor_stop( &msg1, instance, 0x10|stop_num, 0, 0 );
		AddToSendList( &msg1 );
	} else if (strcmp(argv[first_param+2], "mark") == 0)
	{
		byte stop_num  = atoi(argv[first_param+3]);
		angle     = atof(argv[first_param+4]);
		pot_value = strtol(argv[first_param+5], NULL, 0);
		pack_mark_motor_stop( &msg1, instance, stop_num, angle, pot_value );
		AddToSendList( &msg1 );
	} else if (strcmp(argv[first_param+2], "moveto") == 0)
	{
		angle     = atof(argv[first_param+3]);
		short spd = (short)(atof(argv[first_param+4])*100);
		printf ("angle=%10.3f; speed=%x\n",  angle, spd );
		pack_move_to_angle( &msg1, instance, angle, spd );
		AddToSendList( &msg1 );
	} else if (strcmp(argv[first_param+2], "speed") == 0)
	{
		speed    = atof(argv[first_param+3]);
		printf("speed=%10.3f\n", speed);
		pack_move_speed( &msg1, instance, speed );
		//print_message( &msg1 );
		AddToSendList( &msg1 );
	}  else if (strcmp(argv[first_param+2], "coast") == 0)
	{
		speed = atof(argv[first_param+3]);
		printf    ( "brake=%10.3f \%\n", speed );
		pack_coast( &msg1, instance, speed     );
		//print_message( &msg1 );
		AddToSendList( &msg1 );
	} else if (strcmp(argv[first_param+2], "stop") == 0)
	{
		//speed = atof(argv[first_param+3]);
		pack_stop( &msg1, instance );
		AddToSendList( &msg1 );
	} else if (strcmp(argv[first_param+2], "dspeed") == 0)
	{
		speed    = atof(argv[first_param+3]);
		speed2   = atof(argv[first_param+4]);
		printf("speed=%10.3f\n", speed);
		printf("speed2=%10.3f\n", speed2);
		pack_move_dual_speed( &msg1, instance, speed, speed2 );
		AddToSendList( &msg1 );
	}	
	
}

void getLine( FILE* f, char* mLine )
{
	byte i=0;
	while (!feof(f))
	{
		mLine[i] = fgetc(f);
		if (mLine[i] == '\n') {
			mLine[i] = 0;
			return ;
		}
		i++;
	}
}

char* getWord( char* mLine )
{
	static char tmp[50];
	char* firstspace = strchr(mLine, ' ');
	strncpy(tmp, mLine, (firstspace-mLine));
	tmp[(firstspace-mLine)] = 0;
	return tmp;
}

/*
	Input must be all integers
			mInts should be allocated large enough to hold the expected size.
	Return : Size of the array.
*/
byte getIntArray( char* mLine, byte* mInts )
{
	char* tPtr = mLine;
	static char tmp[50];
	int i=0;

	do {
		char* firstspace = strchr(tPtr, ' ');
		if (firstspace==NULL) return i;
		
		strncpy(tmp, tPtr, (firstspace-tPtr));
		tmp[(firstspace-tPtr)]=0;
		mInts[i++] = atoi(tmp);

		// leap frog to next word:
		tPtr = firstspace;
		while (*tPtr == ' ') { tPtr++; };		
	} while( 1 );
}

// Get a Pointer to the start of the next word!
char* skipNext( char* mStr)
{
	char* tPtr = strchr( mStr, ' ' );
	if (tPtr==NULL) return NULL;
	while (*tPtr == ' ') { tPtr++; };		
	return tPtr;
}

char line[255];
char* readvec( char* mName )
{
	FILE* f = fopen(VectorFileName,"r");
	getLine( f, line );
	do 
	{	
		char* name = getWord( line );
		if (strcmp( name, mName ) == 0)			
			return line;
		getLine( f, line );		
	} while (!feof(f));	
	fclose(f);
}

int proc_vec( int argc, char *argv[], byte first_param )
{
	float Angles[MAX_VECTOR_SIZE];
	float Speeds[MAX_VECTOR_SIZE];			
	byte  Instances[MAX_VECTOR_SIZE];

	// GET SPECIFIC INSTANCE NUMBER (255 for all):
	if (argc <= (first_param+1)) {
			help_vec();
			printf( "No vector command.\n"); 
			return 0;
	}
	
	// GET OPERATION TO BE DONE ON INSTANCE:
	if (strcmp(argv[first_param+1], "clearall") == 0)
	{
			// Erase .ini file
			FILE* f = fopen(VectorFileName,"w");
			fprintf(f, " ");
			fclose(f);
			printf("Motor Vectors Erased!...\n");
	}
	if (strcmp(argv[first_param+1], "define") == 0)
	{
			// Create a Vector (mapping of board instances with indexes)
			printf("Create Motor Vector...\n");
			if (argc < (first_param+3)) {
					printf( "No vector command.\n"); 
					return 0;
			}

			// CONSTRUCT vector:			
			byte size = atoi(argv[first_param+3]);
			if (size > 50) {
				printf("Current Max vector size = 50\n");
				return 0;
			}
			//struct stVector* vector = NewVector( argv[first_param+2], size );
	//		AssignBoards	 ( vector, instances );
		
			FILE* f = fopen(VectorFileName,"a+");
			fprintf(f, "%s %d ", argv[first_param+2], size);

			byte instances[50];
			for (int i=0; i<size; i++)
			{
				if ((i+3) > argc)
				{
					printf("Not enough instances!");
					return 0; 
				} else {
					instances[i] = atoi(argv[first_param+4+i]);
					fprintf( f, "%d ", instances[i] );
				}
			}
			fprintf( f, "\n");
			fclose(f);
	}
	if (strcmp(argv[first_param+1], "showall") == 0)
	{
			FILE* f = fopen(VectorFileName,"r");
			
			// PRINT .ini file
			char c = fgetc(f);
			while (!feof(f))
			{	
				printf("%c", c);
				c = fgetc(f);
			}			
			fclose(f);
	}
	else if (strcmp(argv[first_param+1], "set") == 0)
	{
			// GET VECTOR FROM INI File:
			char* vect_text = readvec( argv[first_param+2] );
			if (vect_text == NULL)
				return 0;
			//printf("%s\n", vect_text);

			// Create a Vector (mapping of board instances with indexes)
			// PARSE SIZE:
			char* sLine = skipNext( vect_text );		// skip Name
			byte vsize  = atoi( getWord(sLine) );
			char* rLine = skipNext( sLine );			// skip size

			// PARSE INSTANCES:
			byte size   = getIntArray( rLine, Instances );

			// The send actually does the AddToSendList() calls for each element
			if (argc < (vsize*2 + first_param+2))
			{	printf ("Not enough vectors!");  return 0;  }

			//int argi = 0;
			// EXTRACT ALL ANGLES & SPEEDS:
			int i;
			for (i=0; i<vsize; i++) {
				Angles[i] = atof(argv[first_param+4+i]);
				//printf("%9.3f, ", Angles[i]);
			}
			//printf("\nSpeeds=");
			byte s = first_param+4+i;
			for (i=0; i<vsize; i++) {
				Speeds[i] = atof(argv[s+i]);
				//printf("%9.3f ", Speeds[i]);
			}
			//printf("\n");
			send_angle_vector( vsize, Instances, Angles, Speeds ); 			
	}
}

