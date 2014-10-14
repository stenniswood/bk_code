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

void help()
{
	printf("Commands List : \n\n");
	help_led();
}

void print_commands()
{
	printf("all_on		-	Turn all pixels on \n");
	printf("clear_screen	-	Turn all pixels off\n");
	printf("draw_line	-	x1 y1   x2 y2 \n");
	
	printf("clear_eol	-	clear all pixels to end of line\n");
	printf("horiz_line	-	row, start, stop column\n");
	printf("vert_line	-	column, start, stop row\n");
			
	printf("arc		-	cx cy  Radius StartAngle StopAngle\n");
	printf("box		-	x1 y1  x2 y2\n");
	printf("circle		-	cx cy Radius \n");
	
	printf("beep		-	on_ms off_ms  repeat\n");	
	printf("backlight	-	fraction\n");	
	printf("contrast	-	fraction\n");	
			
	printf("invert		-	\n");
	printf("cursor_to	-	line column Font \n");
	printf("graph		-	\n");
	printf("polar		-	\n");

	printf("set_window	-	x1 y1  x2 y2 \n");	
	printf("text		-	\" your text here!  \" \n");	
}

void help_led()
{
	printf("LCD Drawing utility\n\n"								  );
	printf("alcd [instance] [command] [param1] .. [param n] \n"		  );
	printf("\t: [instance] 	 - board id on the network\n"			  );
	printf("\t: [command] 	 - type of drawing to be done:\n\n"		  );	
	print_commands();
//	printf("\t: [param1] 	 - first parameter - depends on the command.\n"	);	
//	printf("\t: [param n] 	 - the last parameter for the cmd.\n"	  );
}

