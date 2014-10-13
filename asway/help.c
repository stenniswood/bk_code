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
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "help.h"

void help()
{
	printf("Commands List : \n\n");
	help_main();
	help_config();
}

void help_main()
{
	printf("Two Wheel Balancing Robot (ie Segway style) \n"				 );
	printf(" 	\n"														 );
	printf("This app Runs the segway balancing algorithm and basic move operations.   \n");
	printf(" For wireless Operation also startup bk_remote app.\n"		 );
	printf(" 	\n"														 );
	printf("Option List : \n\n"											 );
	printf("	X  [instance] Tilt sensor X axis goes with which board.\n");
	printf("	Y  [instance] Tilt sensor Y axis goes with which board.\n");
}


void help_config()
{
	printf("config [command] [instance] \n");
	printf("\t[command] => [Left,Right,Tilt,Angle] \n");
	printf("\t\t For left wheel, right wheel, Tilt angle offset, etc \n");
}

