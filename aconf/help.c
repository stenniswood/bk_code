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
	help_dev();
	help_config();
	help_led();
	help_instance();
	help_vec();
	help_mot();
	help_lcd();
}

void help_dev()
{
	printf("dev [model:opt] \n");
	printf("\tmodel => sort by board model numbers\n");
}

void help_config()
{
	printf("config [instance] \n");
	printf("\tread => read all 4 config bytes \n");
	printf("\t[1..4] => config byte index \n");
	printf("\t[mask]  => indicates which bits can be changed. \n");
	printf("\t[value] => indicates which bits to change to. \n");
}

void help_led()
{
	printf("led [instance] [mode] [value:opt] : \n");
	printf("\tmode: device = restore the LED control to the device\n");
	printf("\t    : myInstance = each boards flashes it's instance number\n");
	printf("\t    : strobe = leds strobe on or off sequentially\n");		
	printf("\t    : pattern = each board(s) shows the pattern number\n");
}

void help_instance()
{
	printf("instance [instance] [operation] [new instance:opt]\n");
	printf("\t: restart - clear the boards instance & confirmed status\n");
	printf("\t: reassign - Directly change the boards instance number\n");
}
void help_vec()
{
	printf("vec : \n");
	printf("\t: clearall - erase all previously defined vectors (.ini)\n");
	printf("\t: showall  - display all previously defined vectors (.ini)\n");
	printf("\t: define   - [new vec name] [size] [instance0] [instance1]...[instance n]\n");
	printf("\t: moveto   - [vec name] [angle 0] [angle 1]...[angle n] \n");
}
void help_mot()
{
	printf("mot [instance] [cmd] :  Motor board\n");
	//printf("\t: config = change mode/configuration \n" );
	printf("\t: getmark [stop number:1,2] = read stop angle & value\n");
	printf("\t: mark   [stop number:1,2] [angle] [value] = set stops\n");
	printf("\t: moveto [angle:real] [speed(0..100.99)] = move motor to an angle.\n"   );
	printf("\t: speed  [speed percent (0..100)] = run motor at speed\n" );
	printf("\t: coast  [brake percent (0..100)] = 0 most coast.  100 most braking.\n" );
	// this is counter intuitive.  Seems like 100 mosfets are on - would allow recirculating current
	// Yet this is not what is experienced.
	printf("\t: dspeed [speed1 percent (0..100)] [speed1 percent (0..100)] = dual speed (BigMotor configured as 2 unidirectional motors)\n" );	
	printf("\t: stop   .\n" );	
}

void help_lcd()
{
	printf("lcd [instance] [operation] [new instance:opt]\n");
	printf("\t: clear- erase entire screen\n");
	printf("\t: allon- fill entire screen\n");	
	printf("\t: txt  - [x:opt] [y:opt] [font] [text]\n"	);
	printf("\t: hl 	 - horizontal line [row] [StartColumn] [EndColumn] \n");
	printf("\t: vl 	 - vertical line   [Column] [StartRow] [EndRow] \n");
	printf("\t: box  - draw a box      [Column] [StartRow] [EndRow] \n");
	printf("\t: invert- vertical line   [Column] [StartRow] [EndRow] \n");	
	printf("\t: back - backlight [percent on (0.0..100.0)]\n" );
	printf("\t: contrast - change the contrast [percent on (0.0..100.0)]\n");
	printf("\t: beep - [on ms] [off ms] [number] \n");
	printf("\t: screen - Show test screen\n");
}

