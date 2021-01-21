#include <unistd.h> // required for I2C device access and usleep()
#include <fcntl.h>  // required for I2C device configuration
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions
#include <stdio.h>  // required for printf statements

#include <fstream>
#include <iostream>
#include <string>

#include "dfServo.hpp"


dfServo::dfServo( char mServoLetter )
{
	m_motorLetter = mServoLetter;
}

dfServo::~dfServo() 
{

}

float	dfServo::get_angle (  						)
{
return 0.0;
}
void    dfServo::angle_to  ( float mDegree 		 	)
{
}
void    dfServo::count_to  ( unsigned short mCounts 	)
{
	// CMD= SET_PID
	// Count

}
	
void dfServo::save_cal_file_line( FILE* fd )
{
	Servo::save_cal_file_line( fd );
}

