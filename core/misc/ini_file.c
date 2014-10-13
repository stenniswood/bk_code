/****************************************************************
INI FILE 
CONTENTS:
	M1		M2		instance1
	M3		M4		instance2

instance1, instance2, alpha, swap_xy[0,1]
 
To Fly!
	Stand alone solution
		Thrust following a sequence
			
	String solution	

	Socket connection to cell phone (involves Wifi though)
		SocketServer - simple app for GPIO
		
		Simple A) Thumb slider for thrust ( auto balance from tilt )
		Stage  B) Tilt matches tilt
		
****************************************************************/
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "gpio.h"
#include "mcp2515.h"
#include "pican_defines.h"
#include "gpio.h"
#include "CAN_Interface_.h"
#include "packer_lcd.h"
#include "can_txbuff.h"
#include "Packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
//#include "system_msgs_callback.h"
//#include "board_list.h"
#include "motor_vector.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "ini_file.h"


char line[255];
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


void show_file( char* mFileName )
{
	FILE* f = fopen(mFileName,"r");

	// PRINT .ini file
	char c = fgetc(f);
	while (!feof(f))
	{	
		printf("%c", c);
		c = fgetc(f);
	}			
	fclose(f);
}

void save( int argc, char* argv[], char* mVectName, byte vector_size )
{
	byte  Instances[MAX_VECTOR_SIZE];
	FILE* f = fopen(VectorFileName,"a+");
	fprintf(f, "%s %d ", argv[first_param+2], vector_size );
	byte instances[50];
	
	for (int i=0; i<vector_size; i++)
	{
		if ((i+3) > argc)
		{
			printf("Not enough instances!");
			return 0;
		} else {
			instances[i] = atoi( argv[first_param+4+i] );
			fprintf( f, "%d ", instances[i] );
		}
	}
	fprintf( f, "\n");
	fclose(f);
}

