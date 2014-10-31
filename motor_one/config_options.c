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
#include "help_configs.h"


/********************************
RETURN:		Config Index [1..4]
*********************************/
byte get_config_option_bigmotor( char* mOption, byte* mValue)
{
	if (strcmp(mOption, "MODE_USE_ENCODER")==0)
	{
		*mValue = 0x01;	
		return 1;
	} else if (strcmp(mOption, "MODE_2_MOTORS")==0)
	{
		*mValue = 0x02;	
		return 1;
	} else if (strcmp(mOption, "MODE_RESERVED_1")==0)
	{
		*mValue = 0x04;	
		return 1;
	} else if (strcmp(mOption, "MODE_FREE_SPIN")==0)
	{
		*mValue = 0x08;	
		return 1;
	} else if (strcmp(mOption, "MODE_TILT_RESPONDER")==0)
	{
		*mValue = 0x10;	
		return 1;
	} else if (strcmp(mOption, "MODE_PID_CONTROL")==0)
	{
		*mValue = 0x20;	
		return 1;
	} else if (strcmp(mOption, "MODE_BASE_FREQ_1")==0)
	{
		*mValue = 0x40;	
		return 1;
	} else if (strcmp(mOption, "MODE_BASE_FREQ_2")==0)
	{
		*mValue = 0x80;	
		return 1;
	}
	
	/********* CONFIG BYTE #2 ******************/
	if (strcmp(mOption, "MODE_SEND_POSITION_RAW")==0)
	{
		*mValue = 0x01;	
		return 2;
	} else if (strcmp(mOption, "MODE_SEND_POSITION_CALC")==0)
	{
		*mValue = 0x02;	
		return 2;
	} else if (strcmp(mOption, "MODE_SEND_STATUS")==0)
	{
		*mValue = 0x04;	
		return 2;
	} else if (strcmp(mOption, "MODE_SEND_ACCEL")==0)
	{
		*mValue = 0x08;	
		return 2;
	}	
}
/********************************
RETURN:		Config Index [1..4]
*********************************/
byte get_config_option_tilt( char* mOption, byte* mValue)
{
	if (strcmp(mOption, "MODE_SEND_COMBINED_ANGLES_VELOCITY")==0)
	{
		*mValue = 0x01;	
		return 1;
	} else if (strcmp(mOption, "MODE_SEND_COMBINED_LINEAR")==0)
	{
		*mValue = 0x02;	
		return 1;
	} else if (strcmp(mOption, "MODE_SEND_COMBINED_LINEAR_VELOCITY")==0)
	{
		*mValue = 0x04;	
		return 1;
	} 	
	/********* CONFIG BYTE #2 ******************/
	if (strcmp(mOption, "MODE_SEND_ACCEL")==0)
	{
		*mValue = 0x01;	
		return 2;
	} else if (strcmp(mOption, "MODE_SEND_GYRO")==0)
	{
		*mValue = 0x02;	
		return 2;
	} else if (strcmp(mOption, "MODE_SEND_MAGNET")==0)
	{
		*mValue = 0x04;	
		return 2;
	} else if (strcmp(mOption, "MODE_SEND_COMBINED_ANGLES")==0)
	{
		*mValue = 0x08;	
		return 2;
	}

	/********* CONFIG BYTE #3 ******************/
	if (strcmp(mOption, "MODE_ACCEL_2g")==0)
	{
		*mValue = 0x00;	
		return 2;
	} else if (strcmp(mOption, "MODE_ACCEL_4g")==0)
	{
		*mValue = 0x01;	
		return 2;
	} else if (strcmp(mOption, "MODE_ACCEL_8g")==0)
	{
		*mValue = 0x02;	
		return 2;
	} else if (strcmp(mOption, "MODE_ACCEL_16g")==0)
	{
		*mValue = 0x03;	
		return 2;
	}	

}
