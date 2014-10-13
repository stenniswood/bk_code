#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "bk_system_defs.h"
#include "mcp2515.h"
#include "pican_defines.h"
#include "CAN_base.h"
#include "packer_lcd.h"
#include "packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "can_id_list.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"


struct fXYZ RawxyzAccel;
struct fXYZ RawxyzGyro ;
struct fXYZ RawxyzMagnet;
struct tXYZ Rawxyz;


void extract_xyz( struct sCAN* mMsg, struct tXYZ* mXYZ )
{
	Rawxyz.x = (mMsg->data[0]<<8) + mMsg->data[1];
	Rawxyz.y = (mMsg->data[2]<<8) + mMsg->data[3];
	Rawxyz.z = (mMsg->data[4]<<8) + mMsg->data[5];
}


void swap_bytes( struct tXYZ* mXYZ )
{
	short tmp = mXYZ->x;
	mXYZ->x = hi(tmp) + (lo(tmp)<<8);
	tmp = mXYZ->y;
	mXYZ->y = hi(tmp) + (lo(tmp)<<8);
	tmp = mXYZ->z;
	mXYZ->z = hi(tmp) + (lo(tmp)<<8);
}


/* Observations of Accelerometer Scale :
change sensitivity with: sudo ./aconf config 9 3 0x03 [0..3]

config 3 0x03 16g: Full scale was around 1200 decimal		1/1  mg/digit
config 3 0x02 8g : Full scale was around 4000 decimal		1/4  mg/digit
config 3 0x01 4g : Full scale was around 8000 decimal		1/8  mg/digit
config 3 0x00 2g : Full scale was around 16384 decimal		1/16 mg/digit
	00 (2g)	each digital is 1/16th of an mg.  It takes 16 counts to get 1 mg.
Inother words reciprocal of the data sheet.  */
#define One_g_scale2g 0x4000			// 
#define One_g_scale4g 0x2000			// 
#define One_g_scale8g 0x1000			// 
#define One_g_scale16g 0x0800			// 
/* Need ability to read accelerometer sensitivity (configure byte 3) */

void print_raw( struct sCAN* mMsg, struct fXYZ* mRaw, BOOL mShowCANmsg )
{
//	if (mShowCANmsg)
//		print_message(mMsg);
	print_vector( mRaw );
	printf("\n");
}

/* Extracts raw data from the CAN message */
void parse_accel_msg(struct sCAN* mMsg)
{
	extract_xyz(mMsg, &Rawxyz);
	convert_to_real( &Rawxyz, &RawxyzAccel);
}
/* Extracts raw data from the CAN message */
void parse_gyro_msg(struct sCAN* mMsg)
{
	extract_xyz	   ( mMsg, &Rawxyz);
	convert_to_real( &Rawxyz, &RawxyzGyro);	
}
/* Extracts raw data from the CAN message */
void parse_magnet_msg(struct sCAN* mMsg)
{
	extract_xyz(mMsg, &Rawxyz);
	convert_to_real( &Rawxyz, &RawxyzMagnet);	
}

