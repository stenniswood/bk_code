#include <unistd.h> // required for I2C device access and usleep()
#include <fcntl.h>  // required for I2C device configuration
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions
#include <stdio.h>  // required for printf statements

#include <fstream>
#include <iostream>
#include <string>

#include "servo.hpp"

std::string calfile_lines[16] = { "", "", "", "", "", "", "", "", 
								  "", "", "", "", "", "", "", ""  };

Servo* servs[16];
int    servs_used = 0;

//extern ostream cout;
Servo::Servo(  )
{
	cal_min = 5;
	cal_max = 511;
	
	Deg_min = 0;
	Deg_min = 180;
}

Servo::~Servo() 
{
}

// Goes thru entire cal file:
/*void Servo::load_cal_info() 
{
	if (calfile_lines[ServoNum].length() > 1)
	{
		parse_cal_line( calfile_lines[ServoNum] );
	}
}*/

unsigned short Servo::convert_deg_to_counts( float mDegrees )
{
	float c_range = (float)(cal_max - cal_min);
	float d_range = (float)(Deg_max - Deg_min);
	float cpd     = c_range / d_range;

	float  degrees = (mDegrees - Deg_center);
	float  count   = (unsigned short)(( degrees * cpd ) + cal_center);

//printf("c_range=%6.3f; d_range=%6.3f\n", c_range, d_range );
//printf("deg_del=%6.3f; c_center=%d\n", c_range, cal_center );

	return count;
}

float Servo::convert_counts_to_degs( float mCounts  )
{
	float c_range  = (float)(cal_max - cal_min);
	float d_range  = (Deg_max - Deg_min);
	float dpc      = d_range / c_range;
	//printf("c %4.3f \n", c_range);
	float degrees  = ((unsigned short) ((float)(mCounts - cal_center) * dpc)) + Deg_center;
	return degrees;
}

char delimiter = ',';

void Servo::parse_cal_line( std::string mStr ) 
{
	size_t pos = 0;
	std::string token;
	if ((pos = mStr.find(delimiter)) != std::string::npos) 
	{
			// MIN CAL			
	    	token = mStr.substr(0, pos);				// Up to deliminator:
    		mStr.erase(0, pos + 1);	// Erase token
			cal_min = std::stoi( token );	
//			printf("cal_min=%d\n", cal_min );	
	}
	if ((pos = mStr.find(delimiter)) != std::string::npos) 
	{
			// MAX CAL
	    	token = mStr.substr(0, pos);		
    		mStr.erase(0, pos + 1);
			cal_max = std::stoi( token );
//			printf("cal_max=%d\n", cal_max );				
	}
	if ((pos = mStr.find(delimiter)) != std::string::npos) 
	{
			// CAL CENTER:
	    	token = mStr.substr(0, pos);		
    		mStr.erase(0, pos + 1);
			cal_center = std::stoi( token );
//			printf("cal_center=%d\n", cal_center );				
	}

	if ((pos = mStr.find(delimiter)) != std::string::npos) 
	{
			// DEG MIN	
	    	token = mStr.substr(0, pos);		
    		mStr.erase(0, pos + 1);
			Deg_min = std::stof( token );						
	}
	if ((pos = mStr.find(delimiter)) != std::string::npos) 
	{
			// DEG MAX	
	    	token = mStr.substr(0, pos);
    		mStr.erase(0, pos + 1);
			Deg_max = std::stof( token );
	}
	{
			// DEG CENTER
	    	token = mStr.substr(0, mStr.length() );
    		mStr.erase(0, pos + 1);
			Deg_center = std::stof( token );
	}

	std::cout << mStr << std::endl;		
}


void Servo::print_parameters()
{
	char tmp[256];
	sprintf(tmp, "%d, %d, %4d, %6.2f, %6.2f %6.2f", 
				cal_min, cal_max, cal_center, 
				Deg_min, Deg_max, Deg_center );
	size_t len = strlen( tmp );
	printf("%s\n", tmp);
}

float Servo::get_angle()  
{
 return 0.0; 
}

unsigned short Servo::get_stop_counts( int mStopNum )
{
	switch (mStopNum)
	{
	case 0 : return cal_min;
	case 1 : return cal_max;
	case 2 : return cal_center;
	default: return 0;
	}
};


void Servo::save_cal_file_line( FILE* fd )
{
	if (fd<0)
	{   perror ("Error writing to CAL file!"); }

	char tmp[256];
	sprintf(tmp, "%d, %d, %4d, %6.2f, %6.2f, %6.2f\n", cal_min, cal_max, cal_center, Deg_min, Deg_max, Deg_center );
	size_t len = strlen( tmp );
	printf("%s", tmp);
	
	fwrite( tmp, 1, len, fd );
}


void  Servo::set_cal_min   ( unsigned short mMinCounts )
{ 
    cal_min = mMinCounts;	 		
	Deg_min = convert_counts_to_degs( mMinCounts );
}

void  Servo::set_cal_max   ( unsigned short mMaxCounts )
{ 
    cal_max = mMaxCounts;
	Deg_max = convert_counts_to_degs( mMaxCounts );
}

void  Servo::set_cal_center ( unsigned short mCenterCounts )
{ 
    cal_center = mCenterCounts;
	Deg_center = convert_counts_to_degs( mCenterCounts );
}




	