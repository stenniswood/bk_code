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
#include "mcp2515.h"
#include "pican_defines.h"
#include "CAN_Interface.h"
#include "packer_lcd.h"
#include "can_txbuff.h"
#include "packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "system_msgs_callback.h"
#include "board_list.h"
//#include "motor_vector.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "ini_file.h"
#include "power.h"
#include "quad_ini.h"


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

/*
	Gets all text up to the first space.
*/
char* getWord( char* mLine )
{
	static char tmp[50];
	char* firstspace = strchr(mLine, ' ');
	strncpy(tmp, mLine, (firstspace-mLine));
	tmp[(firstspace-mLine)] = 0;
	return tmp;
}


void read_ini(char* mFileName )
{
	//printf("opening %s\n", mFileName);
	FILE* f = fopen(mFileName,"r");
	getLine( f, line );
	
	char* p   = strtok (line, " ");
	instance1 = atoi( p );

	p = strtok (NULL, " ");
	instance2 = atoi( p );

	p = strtok (NULL, " ");
	x_alpha = atof( p );

	p = strtok (NULL, " ");
	y_alpha = atof( p );

	char* thrust_map_Filename = strtok (NULL, " ");
	printf("INI: Instance <1,2> = <%d, %d> \n", instance1, instance2 );	
	printf("INI: alpha <x,y> = <%8.6f %8.6f> \n", x_alpha, y_alpha 	);
	printf("INI: Thrust Filename:  %s\n", thrust_map_Filename );	
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

void save_ini( char* mFileName, byte instance1, byte instance2, 
			   float x_alpha, float y_alpha )
{
	FILE* f = fopen(mFileName,"a+");
	fprintf(f, "%d %d %10.4f %10.4f\n", instance1, instance2, x_alpha, y_alpha );	
	fclose(f);
}

