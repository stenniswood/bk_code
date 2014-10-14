/************************************************************************
RASPBERRY PI SIDE OF THINGS.  SENDS SPI DATA TO ATMEL!

Use a binary protocol.  Simple exchange.  1 byte will indicate
the command.
************************************************************************/
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "bk_system_defs.h"
#include "pican_defines.h"
#include "help.h"
#include <pthread.h>
#include "atmel_spi.h"

/* Add these VARS to the bkInstant - dummy purposes to allow this to 
   compile for both.		*/
byte	LED_status;				// LED_Status
byte	LS_Values;				// Lowside Drivers
byte 	Buttons;				// 3 front edge buttons
word 	Analog_Values[8];		// Really is defined in atmel_spi.h !
float 	Tilt = 0.0;
float 	Pan  = 0.0;

byte analog_channel=0;

void print_buff( byte* mbuff, int length )
{
	printf("Buffer: ");
	for (int i=0; i<length; i++)
		printf(" %x ", mbuff[i] );
	printf("\n");
}

/* Use 16 bit fixed point:  +-32,000  ==> +-320.00 degrees
   way more than enough range.  Precision fine.
*/
extern int cmd;
void update_pan_tilt_angles( float mPan, float mTilt )
{ 
	short pan  = round(mPan  * 100.0);
	short tilt = round(mTilt * 100.0);
	
	// Atmel is slave, so we have to request a read.
	uint8_t buff[3];
	buff[0] = cmd; //UPDATE_PAN_TILT;
	buff[1] = hi(pan);
	buff[2] = lo(pan);
	buff[3] = hi(tilt);
	buff[4] = lo(tilt);
	print_buff( buff, 5 );
	int result = wiringPiSPIDataRW(analog_channel, buff, 5 );
}

/******************************************************
update_low_side_driver()
mBit 	- index of which input to change
mValue	- value to change to.  [0=off; 1=ON]
Change just 1 of the driver outputs.
******************************************************/
void update_low_side_driver(byte mBit, byte mValue)
{
	// Atmel is slave, so we have to request a read.
	uint8_t buff[3];
	buff[0] = UPDATE_LOWSIDE_DRIVER;
	buff[1] = mBit;
	buff[2] = mValue;
	int result = wiringPiSPIDataRW(analog_channel, buff, 3 );
	word retval = (buff[1]<<8) | (buff[2]);
}

/******************************************************
update_low_side_drivers()
mValue	- value to change to.  packed byte of bits [6..0]
Change all of the driver outputs.
******************************************************/
void update_low_side_drivers(byte mValues)
{
	// Atmel is slave, so we have to request a read.
	uint8_t buff[3];
	buff[0] = UPDATE_LOWSIDE_DRIVERS;
	buff[1] = mValues;	
	buff[2] = 0xFF;		
	int result = wiringPiSPIDataRW(analog_channel, buff, 3 );
}

/******************************************************
update_low_side_drivers()
mBit 	- index of which input to change
mValue	- value to change to.  [0=off; 1=ON]
Change all of the driver outputs.
******************************************************/
word read_analog(byte mChannel)
{
	// Atmel is slave, so we have to request a read.
	uint8_t buff[3];
	buff[0] = READ_ANALOG;
	buff[1] = mChannel;	
	buff[2] = 0xFF;		
	int result = wiringPiSPIDataRW(analog_channel, buff, 3 );
	word retval = (buff[1]<<8) | (buff[2]);
	return result;	
}
#define NUM_ANALOG_CHANNELS 8
word Analog_Values[NUM_ANALOG_CHANNELS];

/******************************************************
read_all_analogs()
Read all of the analog inputs.
******************************************************/
void read_all_analogs()
{
	// Atmel is slave, so we have to request a read.
	int length = 3+NUM_ANALOG_CHANNELS*2;
	uint8_t buff[3+NUM_ANALOG_CHANNELS*2];
	buff[0] = READ_ANALOGS;

	for (int i=1; i<NUM_ANALOG_CHANNELS*2.0; i+=2)		// 16 bit values each
	{
		buff[i+1] = 0xFF;		// fill don't care.  
		buff[i+2] = 0xFF;		// returned values will be analog readings.
	}
	int result = wiringPiSPIDataRW (analog_channel, buff, 3+NUM_ANALOG_CHANNELS*2);

	// Analog Values are now in the buff[] array.
	for (int i=0; i<NUM_ANALOG_CHANNELS; i++)
	{		
		Analog_Values[i] = buff[2+i*2] + buff[3+i*2];
	}
		
}

/******************************************************
test_command()
******************************************************/
void test_command(byte mCmd)
{
	// Atmel is slave, so we have to request a read.
	uint8_t buff[2];
	buff[0] = mCmd;
	print_buff(buff, 1);
	
	int result = wiringPiSPIDataRW(0, buff, 1 );
}
