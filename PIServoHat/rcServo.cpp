#include <unistd.h> // required for I2C device access and usleep()
#include <fcntl.h>  // required for I2C device configuration
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions
#include <stdio.h>  // required for printf statements

#include <fstream>
#include <iostream>
#include <string>

#include "rcServo.hpp"



rcServo::rcServo( int mServoNumber )
{
	ServoNum = mServoNumber;
}

rcServo::~rcServo() 
{

}

unsigned char StartAddresses[16] = { 
0x06,	0x0A,	0x0E,	0x12,	
0x16,	0x1A,	0x1E,	0x22,	
0x26,	0x2A,	0x2E,	0x32,	
0x36,	0x3A,	0x3E,	0x42 };	

unsigned char StopAddresses[16] = { 
0x08,	0x0C,	0x10,	0x14,
0x18,	0x1C,	0x20,	0x24,
0x28,	0x2C,	0x30,	0x34,
0x38,	0x3C,	0x40,	0x44
};	


unsigned char rcServo::get_start_address(  )
{
	return StartAddresses[ServoNum];
}

unsigned char rcServo::get_stop_address(  )
{
	return StopAddresses[ServoNum];
}

void rcServo::print_parameters()
{
	printf("S%d :  ", ServoNum);
	Servo::print_parameters();
}

extern int file_i2c;
char buffer[5];   // Create a buffer for transferring data to the I2C device

void rcServo::set_start_time( unsigned short mCounts )
{
  // Write the start time out to the chip. This is the time when the chip will
  //  generate a high output.

  buffer[0] = StartAddresses[ServoNum];  // "start time" reg for channel 0
  buffer[1] = 0;     // We want the pulse to start at time t=0
  buffer[2] = 0;
  int length = 3;        // 3 bytes total written
  write(file_i2c, buffer, length); // initiate the write	
}

void rcServo::set_stop_time( unsigned short mCounts )
{
  buffer[0] = StopAddresses[ServoNum];  // "start time" reg for channel 0
  buffer[1] = mCounts & 0xff;     // We want the pulse to start at time t=0
  buffer[2] = (mCounts>>8) & 0xff;
  int length = 3;        // 3 bytes total written
  write(file_i2c, buffer, length); // initiate the write	
}

void rcServo::count_to( unsigned short mCounts )
{
	m_curr_counts = mCounts;
	m_curr_angle  = convert_counts_to_degs( mCounts );
	//printf("S%d : Counts = %d \n", ServoNum, mCounts );	
		
	set_stop_time( mCounts );	
}

void rcServo::angle_to( float mDegree )
{
	m_curr_angle  = mDegree;
	m_curr_counts = convert_deg_to_counts( mDegree );
		
	printf("S%d : Angle %6.1f = %d \n", ServoNum, mDegree, m_curr_counts );	
	set_stop_time( m_curr_counts );	
}

void rcServo::load_current_position( std::ifstream& fd )
{
	std::string oneLine;
	std::getline(fd, oneLine);	

	sscanf(oneLine.c_str(), "%d : %d \n", &ServoNum, &m_curr_counts );
	printf("Loaded: %d = %d\n", ServoNum, m_curr_counts );

}

void rcServo::save_current_position( FILE* fd )
{
	char tmp[256];
	sprintf(tmp, "%d : %d \n", ServoNum, m_curr_counts );
	size_t len = strlen( tmp );	
	fwrite(tmp, 1, len, fd );
}
	
void rcServo::save_cal_file_line( FILE* fd )
{
	char tmp[256];
	sprintf(tmp, "rc %d, ", ServoNum );
	size_t len = strlen( tmp );	
	fwrite(tmp, 1, len, fd );
	
	Servo::save_cal_file_line( fd );
}




