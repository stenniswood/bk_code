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
//#include "can_txbuff.h"
#include "packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
//#include "system_msgs_callback.h"
//#include "board_list.hpp"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "fuse_ag.h"
#include "fuse_accel.h"
#include "fuse_gyro.h"
#include "fuse_magnet.h"
#include "fuse_log.h"

//extern struct tXYZ RawxyzAccel;
//extern tXYZ GyroReading;
//extern fXYZ MagnetAngles;
//extern fXYZ AccelAngles;
//extern fXYZ AccelAngularVelocity;
//extern fXYZ RawxyzAccel;


/*********************************************************************
 ********************     LOGGING   FUNCTIONS     ********************
 *********************************************************************/
FILE* logfile = NULL;
char* logFileName = "tilt.csv";

void print_header()
{
	fprintf(logfile, "Raw_Accel_X," );
	fprintf(logfile, "Raw_Accel_Y," );
	fprintf(logfile, "Raw_Accel_Z," );

	fprintf(logfile, "Raw_Gyro_x, " );
	fprintf(logfile, "Raw_Gyro_y, " );
	fprintf(logfile, "Raw_Gyro_z, " );

	fprintf(logfile, "Raw_Magnet_x, " );
	fprintf(logfile, "Raw_Magnet_y, " );
	fprintf(logfile, "Raw_Magnet_z, ");

	fprintf(logfile, "|procd|, Net G Force," );
	fprintf(logfile, "AccelAngles_rx," );
	fprintf(logfile, "AccelAngles_ry," );
	fprintf(logfile, "AccelAngles_rz," );

	fprintf(logfile, "GyroAngles_rx,"  );	// these will have "no_gyro_x"
	fprintf(logfile, "GyroAngles_ry,"  );
	fprintf(logfile, "GyroAngles_rz,"  );

	fprintf(logfile, "MagnetAngles_rx,"  );
	fprintf(logfile, "MagnetAngles_ry,"  );
	fprintf(logfile, "MagnetAngles_rz,"  );

	fprintf(logfile, "AccelAngleDelta_rx," );
	fprintf(logfile, "AccelAngleDelta_ry,"   );
	fprintf(logfile, "AccelAngleDelta_rz, \n");

	fprintf(logfile, "MagnetAngleDelta_rx,"   );
	fprintf(logfile, "MagnetAngleDelta_rx,"   );
	fprintf(logfile, "MagnetAngleDelta_rx, \n");
}

void open_log_file()
{
	logfile = fopen(logFileName,"a+");
	print_header();
}

/* 
First test is to find an intercept between Change of Angles & Gyro readings.
*/
void save_sample_raw(  )
{
	fprintf(logfile, "%6.3f, ",  RawxyzAccel.x );
	fprintf(logfile, "%6.3f, ",  RawxyzAccel.y );
	fprintf(logfile, "%6.3f, ",  RawxyzAccel.z );

	fprintf(logfile, "%6.3f, ",  GyroAdjusted.rx );	// GyroReading is correct
	fprintf(logfile, "%6.3f, ",  GyroAdjusted.ry );
	fprintf(logfile, "%6.3f, ",  GyroAdjusted.rz );

	fprintf(logfile, "%6.3f, ", RawxyzMagnet.x );
	fprintf(logfile, "%6.3f, ", RawxyzMagnet.y );
	fprintf(logfile, "%6.3f, ", RawxyzMagnet.z );
}

void save_sample_angles(  )
{
	fprintf(logfile, "%6.3f, ",  AccelAngularPosition.rx );
	fprintf(logfile, "%6.3f, ",  AccelAngularPosition.ry );
	fprintf(logfile, "%6.3f, ",  AccelAngularPosition.rz );

	fprintf(logfile, "no_gyro_x, " );
	fprintf(logfile, "no_gyro_y, " );
	fprintf(logfile, "no_gyro_z, " );
	
	fprintf(logfile, "%6.3f, ", MagnetAngularPosition.rx );
	fprintf(logfile, "%6.3f, ", MagnetAngularPosition.ry );
	fprintf(logfile, "%6.3f, ", MagnetAngularPosition.rz );
}

void save_sample_angular_rate( )
{
	// Unit here should be degrees per second (dps)
	// (not the deltas! scale by time period )
	fprintf(logfile, "%6.3f, ",  AccelAngularVelocity.rx );
	fprintf(logfile, "%6.3f, ",  AccelAngularVelocity.ry );
	fprintf(logfile, "%6.3f, ",  AccelAngularVelocity.rz );

	fprintf(logfile, "%6.3f, ",  GyroAdjusted.rx );
	fprintf(logfile, "%6.3f, ",  GyroAdjusted.ry );
	fprintf(logfile, "%6.3f, ",  GyroAdjusted.rz );

	fprintf(logfile, "%6.3f, ", MagnetAngularVelocity.rx );
	fprintf(logfile, "%6.3f, ", MagnetAngularVelocity.ry );
	fprintf(logfile, "%6.3f, ", MagnetAngularVelocity.rz );
}

void save_sample_intermediate( )
{
	fprintf( logfile, "|procd|," );
	// Total G Force:
	fprintf(logfile, "%6.3f, ",  magnitude(&RawxyzAccel) );
}

void save_sample_result( )
{
	// Prints the combined (final) Orientation values
	fprintf( logfile, "|result|," );

	// ************** ROTATIONAL ***********************
	fprintf(logfile, "%6.3f, ",  BestAngles.rx );
	fprintf(logfile, "%6.3f, ",  BestAngles.ry );
	fprintf(logfile, "%6.3f, ",  BestAngles.rz );	
	fprintf(logfile, "%6.3f, ",  BestAngularRate.rx );
	fprintf(logfile, "%6.3f, ",  BestAngularRate.ry );
	fprintf(logfile, "%6.3f\n",  BestAngularRate.rz );

	// *************** LINEAR *************************
/*	fprintf(logfile, "%6.3f, ",  BestLinearPosition.x );
	fprintf(logfile, "%6.3f, ",  BestLinearPosition.y );
	fprintf(logfile, "%6.3f, ",  BestLinearPosition.z );
	fprintf(logfile, "%6.3f, ",  BestLinearRate.x );	// velocity
	fprintf(logfile, "%6.3f, ",  BestLinearRate.y );
	fprintf(logfile, "%6.3f\n",  BestLinearRate.z );	
	fprintf(logfile, "%6.3f, ",  BestLinearAccel.x );
	fprintf(logfile, "%6.3f, ",  BestLinearAccel.y );
	fprintf(logfile, "%6.3f\n",  BestLinearAccel.z ); */
}

void save_timeslice_data()
{
	save_sample_raw();
	save_sample_angles();
	save_sample_angular_rate();
	//save_sample_intermediate();
	//save_sample_result();
}

void close_log_file()
{
	fclose(logfile);
}


