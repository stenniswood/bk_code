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
#include "motor_vector.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "help.h"

void help()
{
	printf("Commands List : \n\n");
	help_led();
}

void help_led()
{
	printf("aled [mode] [start] [end] [delay ms] [led pattern] \n");
	printf("\t: [mode] 	 - [strobe, blink]\n");
	printf("\t: 	strobe   - quickly strobe led 1,2,3,4 in order.\n");
	printf("\t: 	blink    - flash the pattern, then off before going to next board.\n");	
	printf("\t: [start] is the starting board instance.\n");
	printf("\t: [end] is the ending board instance.\n");	
	printf("\t: [delay ms] is the pause between on and off.\n");	
	printf("\t: [led pattern] is the led pattern to send.\n");	
}

