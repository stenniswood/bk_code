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

void help_config_info(char* mBoardType)
{
	printf("Format : \n\t\tac config [instance] [config index 1..4] [mask 0..ff] [value 0..ff]\n");
	printf("\t\tac config [instance] read\n");
	if (strcmp(mBoardType, "bigmotor")==0)
		help_BigMotor();
	else if (strcmp(mBoardType, "tilt")==0)
		help_Tilt();	
	else if (strcmp(mBoardType, "button")==0)
		help_Button();	
	else if (strcmp(mBoardType, "analog")==0)
		help_Analog();	
	else if (strcmp(mBoardType, "pwm")==0)
		help_PWM();	
	else if (strcmp(mBoardType, "lcd")==0)
		help_LCD();	
	else if (strcmp(mBoardType, "all")==0)
	{
		help_BigMotor();
		help_Tilt();	
		help_Button();
		help_Analog();
		help_PWM();
		help_LCD();
	}
	printf("\n");
}

void help_BigMotor()
{
	printf("\n******** BigMotor Configuration Bits: \t\t\t====\n");
	printf("=== BYTE #1  => [Mask|0xFF] \n");			/**** CONFIG BYTE 1 - BIT DEFS ****/									
	printf("  MODE_USE_ENCODER 	0x01\tdefault=0\n");	// else use potentiometer.  default is pot (0x00)
	printf("  MODE_2_MOTORS	 	0x02\tdefault=0\n");	// 0=> 1 bidirectional;  1=> 2 unidirectional motors
	printf("  MODE_RESERVED_1 	0x04\tdefault=0\n");	// else defaults (80%)
	printf("  MODE_FREE_SPIN	0x08\tdefault=0\n");	// ignore stops.  default (0) is to stop
	printf("  MODE_TILT_RESPONDER\t0x10\tdefault=0\tRespond to ID_ACCEL_XYZ  Y axis readings\n");	// 1=> respond to tilt Y axis readings
	printf("  MODE_PID_CONTROL	0x20\tdefault=0\n");	// PID or constant speed.
	printf("  MODE_BASE_FREQ_1	0x40\tdefault=0\n");	// 
	printf("  MODE_BASE_FREQ_2	0x80\tdefault=0\n");	// 
	printf(" Base		00 ->  100Hz  \n");
	printf(" Frequency	01 ->  300Hz  \n");
	printf("		10 -> 1000hz  \n");
	printf("		11 -> 5000Hz  \n\n");

	printf("=== BYTE #2  => [Mask|0xFF]\n"	   );	/**** CONFIG BYTE 2 - BIT DEFS ****/
	printf("\tUpper nibble of config_byte_2 defines the update rate.  \n\tBigMotor will send motor angle and current readings\n");
	printf("\tevery:  Bits[4..7]\n"			   );
	printf("\t	0x0x	- No report		   		\n");
	printf("\t	0x1x	- Report every 10ms		\n");
	printf("\t	0x2x	- Report every 20ms		\n");
	printf("\t	0x3x	- Report every 50ms		\n");
	printf("\t	0x4x	- Report every 100ms	\n");
	printf("\tBit 0x01 MODE_SEND_POSITION_RAW	\n");
	printf("\tBit 0x02 MODE_SEND_POSITION_CALC  \n");
	printf("\tBit 0x04 MODE_SEND_STATUS			\n");
	printf("\tBit 0x08 MODE_SEND_ACCEL 	 \n\n");

	printf("=== BYTE #3  => [Mask|0xFF]\n");	/**** CONFIG BYTE 3 - BIT DEFS ****/

	printf("=== BYTE #4  => [Mask|0xFF]\n");	/**** CONFIG BYTE 4 - BIT DEFS ****/
	printf("CAN Network Baud rate:\n");
	printf("\tBit 0x10 CAN_BAUD_100K \n");
	printf("\tBit 0x20 CAN_BAUD_125K \n");
	printf("\tBit 0x30 CAN_BAUD_200K \n");
	printf("\tBit 0x40 CAN_BAUD_250K \n");
	printf("\tBit 0x50 CAN_BAUD_500K \n");
	printf("\tBit 0x60 CAN_BAUD_1M   \n");

	printf("\tBit 0x01 CAN_NEW_BOARD 			This is 1 after a reflash.\n");
}

void help_Tilt()
{
	printf("\n******** Tilt Configuration Bits: \t\t\t====\n");
	printf("=== BYTE #1  => \n");	/**** CONFIG BYTE 1 - BIT DEFS ****/
	printf("Bit 0x01  MODE_SEND_COMBINED_ANGLES_VELOCITY \n");
	printf("Bit 0x02  MODE_SEND_COMBINED_LINEAR 		 \n");
	printf("Bit 0x04  MODE_SEND_COMBINED_LINEAR_VELOCITY \n");

	printf("=== BYTE #2  => \n");	/**** CONFIG BYTE 2 - BIT DEFS ****/
	printf("\tUpper nibble of config_byte_2 defines the update rate. \n");
	printf("\tevery:\nBits[4..7]\n"	);
	printf("\t	0x0x	- No report		   \n" );
	printf("\t	0x1x	- Report every 10ms\n" );
	printf("\t	0x2x	- Report every 20ms\n" );
	printf("\t	0x3x	- Report every 50ms\n" );
	printf("\t	0x4x	- Report every 100ms\n");
	printf("\tBit 0x01 MODE_SEND_ACCEL 			 \n");
	printf("\tBit 0x02 MODE_SEND_GYRO	 			 \n");
	printf("\tBit 0x04 MODE_SEND_MAGNET 			 \n");
	printf("\tBit 0x08 MODE_SEND_COMBINED_ANGLES 	 \n");

	printf("=== BYTE #3  => \n");	/**** CONFIG BYTE 3 - BIT DEFS ****/
	printf("MODE_ACCEL_SENSITIVITY_MASK		 0x03\n");
	printf("MODE_ACCEL_2g	 				 0x00\n");
	printf("MODE_ACCEL_4g	 				 0x01\n");
	printf("MODE_ACCEL_8g	 				 0x02\n");
	printf("MODE_ACCEL_16g	 				 0x03\n");

	printf("=== BYTE #4  => \n");	/**** CONFIG BYTE 4 - BIT DEFS ****/
}
void help_Button()
{
	printf("\n******** Button Configuration Bits: \t\t\t====\n");
	printf("=== BYTE #1  => \n");	/**** CONFIG BYTE 1 - BIT DEFS ****/
	printf("\t	0x0x	- Send Roster		   \n" );
	printf("\t	0x0x	- Send Buttons		   \n" );
		
	printf("=== BYTE #2  => \n");	/**** CONFIG BYTE 2 - BIT DEFS ****/

	printf("=== BYTE #3  => \n");	/**** CONFIG BYTE 3 - BIT DEFS ****/

	printf("=== BYTE #4  => \n");	/**** CONFIG BYTE 4 - BIT DEFS ****/
}

void help_Analog()
{
	printf("\n******** Analog Configuration Bits: \t\t\t====\n");
	printf("=== BYTE #1  => \n");	/**** CONFIG BYTE 1 - BIT DEFS ****/
	printf(" MODE_ENABLE_ADC_CHIP1		 0x01\n"	);
	printf(" MODE_ENABLE_ADC_CHIP2		 0x02\n"	);
	printf(" MODE_ENABLE_ADC_CHIP3		 0x04\n"	);
	printf(" MODE_ENABLE_ADC_CHIP4 		 0x08\n"	);
	printf(" MODE_SEND_DERIVATIVES_1 	 0x10\n"	);
	printf(" MODE_SEND_DERIVATIVES_2 	 0x20\n"	);
	printf(" MODE_SEND_DERIVATIVES_3 	 0x40\n"	);
	printf(" MODE_SEND_DERIVATIVES_4 	 0x80\n"	);
	printf(" Board chip layout :\n");
	printf("1	3\n");
	printf("4	2\n");


	printf("=== BYTE #2  => \n");	/**** CONFIG BYTE 2 - BIT DEFS ****/
	printf("\tUpper nibble of config_byte_2 defines the update rate.\n");
	printf("\tevery:\nBits[4..7]\n"			   );
	printf("\t	0x0x	- No report		   \n" );
	printf("\t	0x1x	- Report every 10ms\n" );
	printf("\t	0x2x	- Report every 20ms\n" );
	printf("\t	0x3x	- Report every 50ms\n" );
	printf("\t	0x4x	- Report every 100ms\n");

	printf("=== BYTE #3  => \n");	/**** CONFIG BYTE 3 - BIT DEFS ****/
	printf(" MODE_ENABLE_ADC_CHIP1_MID4	0x01 disable channels4-7\n");	// disable channels 4-7
	printf(" MODE_ENABLE_ADC_CHIP2_MID4	0x02\n");	// 
	printf(" MODE_ENABLE_ADC_CHIP3_MID4	0x04\n");	// 
	printf(" MODE_ENABLE_ADC_CHIP4_MID4	0x08\n");	// 
	printf(" MODE_ENABLE_ADC_CHIP1_UPPER8	0x10 disable channels8-15\n");	// disable channels 8-15
	printf(" MODE_ENABLE_ADC_CHIP2_UPPER8	0x20\n");	// 
	printf(" MODE_ENABLE_ADC_CHIP3_UPPER8	0x40\n");	// 
	printf(" MODE_ENABLE_ADC_CHIP4_UPPER8	0x80\n");	// 

	printf("=== BYTE #4  => \n");	/**** CONFIG BYTE 4 - BIT DEFS ****/

}
void help_PWM()
{
	printf("\n******** PWM Configuration Bits: \t\t\t====\n");
	printf("=== BYTE #1  => \n");	/**** CONFIG BYTE 1 - BIT DEFS ****/

	printf("=== BYTE #2  => \n");	/**** CONFIG BYTE 2 - BIT DEFS ****/

	printf("=== BYTE #3  => \n");	/**** CONFIG BYTE 3 - BIT DEFS ****/

	printf("=== BYTE #4  => \n");	/**** CONFIG BYTE 4 - BIT DEFS ****/

}
void help_LCD()
{
	printf("\n******** LCD Configuration Bits: \t\t\t====\n");
	printf("=== BYTE #1  => \n");	/**** CONFIG BYTE 1 - BIT DEFS ****/

	printf("=== BYTE #2  => \n");	/**** CONFIG BYTE 2 - BIT DEFS ****/
	printf("\tUpper nibble of config_byte_2 defines the update rate.\n");
	printf("\tevery:\nBits[4..7]\n"			   );
	printf("\t	0x0x	- No report		   \n" );
	printf("\t	0x1x	- Report every 10ms\n" );
	printf("\t	0x2x	- Report every 20ms\n" );
	printf("\t	0x3x	- Report every 50ms\n" );
	printf("\t	0x4x	- Report every 100ms\n");

	printf("=== BYTE #3  => \n");	/**** CONFIG BYTE 3 - BIT DEFS ****/

	printf("=== BYTE #4  => \n");	/**** CONFIG BYTE 4 - BIT DEFS ****/
}

