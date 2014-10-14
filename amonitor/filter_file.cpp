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


char line[255];


FilterFile::FilterFile( char* mFileName )
:Preferences( mFileName )
{
}

tID FilterFile::convert_to_id( char* line )
{
	static tID id;
	int num = 0;
	char** array = split(line, ':', &num );
	if (num>2)
		//sscanf_s( array[1], "%x", &(id.group.id) );
		id.group.id = strtol (array[1],NULL,16);
		//id.group.id = atoi(array[1], 16);
	return id;
}

void FilterFile::Read_n_Add(  )
{
	open( TRUE );	
	
	// SETUP PASS/REJECT/NO FILTERING:	
	readln_nb( line );	
	printf("Filter Type: %s\n", line );
	if (strcmp(line, "Reject")==0)
		set_filter_mode( REJECT_FILTER );
	else if (strcmp(line, "Pass")==0)
		set_filter_mode( PASS_FILTER );
	else if (strcmp(line, "Off")==0)
		set_filter_mode( NO_FILTER );

	tID id;
	while (!feof(fd))
	{
		readln_nb( line );
		printf( "Line is %s \n", line );
		id = convert_to_id( line );
		print_eid         ( &id );
		add_id_to_filter  ( &id );
	}
	fclose(fd);
}

