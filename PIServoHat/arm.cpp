#include <unistd.h> // required for I2C device access and usleep()
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <iomanip>

#include <string>
#include <vector>
 #include <assert.h>
//#include <string.h>
#include "drive_five.h"
#include "servo.hpp"
#include "dfServo.hpp"
#include "rcServo.hpp"

#include "arm.hpp"
#include <math.h>



//Arm  leftArm ( "dv s4 dx s5" );
//Arm  rightArm( "dv s0 dx s1" );


Arm::Arm()
{
}

Arm::Arm( std::string mChain )
{
	define_motors( mChain );
}


Arm::~Arm()
{
}
void Arm::Init()
{
	df = NULL;				// File descriptor to the opened DriveFive object.		
	/* df:v		==>  DriveFive board motor V
	   df:x     ==>  DriveFive board motor X
	   S:0		==>	 RPI Servo Hat connector #0
	   S:12		==>	 RPI Servo Hat connector #12	   
	*/
	
	for (int i=0; i<8; i++)
		servs[i] = NULL;		// Up to half of the Phat	

}
// each one 'space' deliminated:
void Arm::define_motors ( std::string mChain )
{
	std::string delimiter = " ";
	size_t pos = 0;
	std::string token;
	while ((pos = mChain.find(delimiter)) != std::string::npos) 
	{
	    	token = mChain.substr(0, pos);
    		mChain.erase(0, pos + delimiter.length());

			if (token[0]=='s')
			{
				int servo_channel = std::stoi( &(token[1]) );
				printf("Creating new servo object : channel = %d\n", servo_channel );
				servs[servo_channel] = new rcServo( servo_channel );
			}	
//			char tmp[3];
			struct stMotId tmp;
			tmp.label[0] = token[0]; 			
			tmp.label[1] = token[1]; 			
			tmp.label[2] = token[2]; 
			m_motor_sequence.push_back( tmp );

	}	
}

void Arm::load_cal_info ( )
{
	for (int i=0; i<8; i++)
	if (servs[i] != NULL)
	{
		//servs[i]->load_cal_info();
		servs[i]->print_parameters();
	}
}

void Arm::actuate_servo( int servoChannel, float  mAngle )
{	
	assert( (servoChannel>=0) && (servoChannel < 16) );	
		
	servs[servoChannel]->angle_to( mAngle );
}

void Arm::actuate_df( char mDriveLetter, float  mAngle )
{
	assert( df != NULL );
	
	
	char cmd2[80];
	sprintf(cmd2, "angle %c %6.1f\r\n", mDriveLetter, mAngle );
	
	printf(" %s", cmd2 );
//	unsigned short counts = ->convert_deg_to_counts( mAngle );
	
	bool result = df->send_command( cmd2 );
	usleep(200000);
}


void Arm::actuate_vector( std::vector<float> mAngles )
{

	char* name = df->get_device_name();
	printf("actuate_vector: %s\n", name );	
	for (int i=0; i<mAngles.size(); i++)
	{
		char token = m_motor_sequence[i].label[0];
		printf("\ttoken=%c\t", token);

		if (token=='d')
		{
			actuate_df( m_motor_sequence[i].label[1], mAngles[i] );
		} 
		else if (token=='s')
		{
			int servo_channel = std::stoi( &(m_motor_sequence[i].label[1]) );
			printf("rc channel=%d\t", servo_channel);
			actuate_servo( servo_channel, mAngles[i] );
		}
		
	}
}


