// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "global.h"
#include "CAN_base.h"

#include "can_id_list.h"
#include "buttons.h"
#include "vector_math.h"
#include "arm_claw.hpp"


Arm::Arm( const char* mName )
: m_name(mName), 
m_shoulder("/dev/ttyACM81",1000), 
m_elbow   ("/dev/ttyACM91",1000)
{
}

Arm::Arm()
: m_name("Arm"), 
m_shoulder("/dev/ttyACM81",1000), 
m_elbow   ("/dev/ttyACM91",1000)
{
}

/***************** GET POSITIONS FROM ROBOCLAW BOARD **********************/

float Arm::get_shoulder_angle		()
{
	bool     valid;
	uint8_t  status;
	uint32_t reading = m_shoulder.ReadEncM1(0x80, &status, &valid);	
	float    result = reading;
	return   result;
}
float Arm::get_shoudler_swing_angle()
{
	bool     valid;
	uint8_t  status;
	uint32_t reading = m_shoulder.ReadEncM2(0x80, &status, &valid);	
	float    result = reading;
	return   result;
}
float Arm::get_elbow_angle			()
{
	bool     valid;
	uint8_t  status;
	uint32_t reading = m_elbow.ReadEncM1(0x80, &status, &valid);	
	float    result = reading;
	return   result;
}
float Arm::get_elbow_rotate_angle	()
{
	bool     valid;
	uint8_t  status;
	uint32_t reading = m_elbow.ReadEncM2(0x80, &status, &valid);	
	float    result = reading;
	return   result;
}

/***************** SET POSITIONS FROM ROBOCLAW BOARD **********************/
bool Arm::set_shoulder_duty		  ( float mFraction )
{
	uint32_t duty = mFraction * 32767;
	bool result = m_shoulder.DutyM1(0x80,  duty);
	return result;
}
bool Arm::set_shoulder_swivel_duty( float mFraction )
{
	uint32_t duty = mFraction * 32767;
	bool result = m_shoulder.DutyM1(0x80,  duty);
	return result;
}
bool Arm::set_elbow_duty		  ( float mFraction )
{
	uint32_t duty = mFraction * 32767;
	bool result = m_elbow.DutyM1(0x80,  duty);
	return result;
}
bool Arm::set_elbow_swivel_duty	  ( float mFraction )
{
	uint32_t duty = mFraction * 32767;
	bool result = m_elbow.DutyM1(0x80,  duty);
	return result;
}


void Arm::SavePreferences( Preferences* mpref		)
{
	char str[255];
	sprintf(str, "\nLimb_type=Arm\n" );					
	mpref->write( str );

	printf("Arm:SavePref() done\n");
}

void Arm::LoadPreferences( Preferences* mpref, BOOL mConstructObjects )
{
	//char str[255];
	//mpref->readln_nb( str );

	//MotorPack::LoadPreferences( mpref );		
}

void Arm::stop_all_motors()
{
	m_shoulder.ForwardM1(0x80, 0);
	m_shoulder.ForwardM2(0x80, 0);
	
	m_elbow.ForwardM1(0x80, 0);
	m_elbow.ForwardM2(0x80, 0);	
}

OneVector* Arm::calculate_duties( OneVector* positionVector )
{
	return NULL;
}

