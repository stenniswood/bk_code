#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <string.h>
#include <vector>

#include "drive_five.h"
#include "motor_mapping.h"
#include "cal_file.hpp"
#include "cal_motor.hpp"


/************************  CLASS CalMotor  *****************************/
CalMotor::CalMotor()
{
	disabled     = false;
	reversed	 = false;
	drive_letter = '?';
	strcpy( alias,	    "??"		  );
	strcpy( joint,      "undefined"   );
	strcpy( board_name, "unknown" 	  );
	strcpy( device_name, "/dev/ttyUSBX" );

	min_travel 		= 0;
	max_travel 		= 2600;
	counts_per_rev 	= 500;	
	zero_offset 	= 1300;
	
	cal_pt1  = 0;
	cal_pt1A = 0;
	cal_pt2  = 2600;
	cal_pt2A = 360;	
}

CalMotor::~CalMotor()
{
}

void CalMotor::disable_motor( )
{
	disabled = true;
}

void CalMotor::set_counts_per_rev( long counts  )
{
	counts_per_rev = counts;
}

void CalMotor::set_counts_per_degs	( long counts, float angle )
{
	counts_per_rev = ((float)counts / angle) * 360.0;
}

void CalMotor::set_zero_offset( long counts  )
{
	zero_offset = counts;
}

void CalMotor::set_min_max_travel( long min, long max )
{
	min_travel = min;
	max_travel = max;
}

void CalMotor::set_cal_pt1     ( long count, float angle )
{
	cal_pt1 = count;
	cal_pt1A = angle;
}
void CalMotor::set_cal_pt2     ( long count, float angle )
{
	cal_pt2 = count;
	cal_pt2A = angle;
}

void CalMotor::set_map_info( struct stMap *mInfo       )
{
	drive_letter = mInfo->assigned_motor;
	strcpy( alias, to_alias( mInfo->limb ));
	strcpy( joint, mInfo->joint );
	strcpy( board_name,  mInfo->board_name   );
	strcpy( device_name, mInfo->device_name );
	
	df = mInfo->df;
}

long CalMotor::convert_angle_count( float angle )
{
/*	float arange = (cal_pt2A - cal_pt1A);
//	long  crange = (cal_pt2 - cal_pt1);
	float a = (angle - cal_pt1A) / arange * crange;
	long  c = round(a) + cal_pt1; */
	
	float revs      = (angle/360.0);
	long int counts = revs * counts_per_rev;
	counts += zero_offset;	
	return counts;
}

float CalMotor::convert_count_angle( long count )
{
/*	float arange = (cal_pt2A - cal_pt1A);
	long  crange = (cal_pt2 - cal_pt1);
	
	float c = (count - cal_pt1) / crange * arange;
	float a = c + cal_pt1A;*/
	
	float adjusted = count - zero_offset;
	float revs = ( adjusted / counts_per_rev );		
	return revs * 360.0;
}

char* CalMotor::get_joint()
{
	return joint;
}

char* CalMotor::get_alias()
{
	return alias;
}
void CalMotor::print_cal_data()
{
	printf(" %-4s  %-10s\t", alias, joint  );
	char tmp[50];
	sprintf(tmp, "[%d..%d]", min_travel, max_travel );	
	printf("%-15s", tmp );
	
	sprintf(tmp, "[%6.1f  %6.1f]", get_min_angle(), get_max_angle() );
	printf("%-17s", tmp );
	printf(" %5d\t", counts_per_rev  );
	printf(" %5d",    zero_offset     );
	printf("\n");
}

float	CalMotor::get_max_angle		()
{
	float angle = convert_count_angle( max_travel );
	return angle;
}
float	CalMotor::get_min_angle		()
{
	float angle = convert_count_angle( min_travel );
	return angle;
}
bool CalMotor::valid_range( long int mCount )
{
	if (mCount < min_travel)	return false;
	if (mCount > max_travel)	return false;
	return true;
}

void CalMotor::send_pid_request( float mAngle    )
{
	if (disabled) { 
			printf("%s %s - Disabled!  \n", alias, joint );  
			return;
	};
	
	if (df)
	{
		char tmp[50];
		long int count = convert_angle_count( mAngle );
		bool valid = valid_range( count );
		if (valid==false)  { printf("Count is out of range!\n");  return; }
		
		sprintf(tmp, "pos %c %d", drive_letter, count );	
		printf("%s : %s\n", device_name, tmp);
		df->send_command( (const char*)tmp );
		
	} else {
		printf("No DriveFive board assigned to this CalMotor!  %3s : %s;  %s\n", alias, joint, device_name );
	}
}
		
void CalMotor::send_pid_request( long int mCount )
{
	if (disabled) { printf("%s %c - Disabled!  \n", alias, drive_letter );  return;  };
	
	if ((mCount > min_travel) && (mCount< max_travel))
	{
		
	}
}	
