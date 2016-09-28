// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <linux/serial.h>
#include <errno.h>
#include <termios.h>

//#include "bk_system_defs.h"
#include "global.h"
#include "roboclaw.h"
#include "vector_file.hpp"
#include "ini_file.hpp"
#include "leg_claw.hpp"


Leg::Leg( char* mDevHip, char* mDevKneeAnkle)
: m_board_hip ( mDevHip, 1000), 
  m_board_knee( mDevKneeAnkle, 1000)
{
	Init();
}

void Leg::Init()
{
	m_board_hip.open			(  );
	m_board_hip.set_baud		( B38400 );	

	m_board_knee.open			(  );
	m_board_knee.set_baud		( B38400 );		
	
	SetMainVoltageThresholds  ( );
	SetLogicVoltageThresholds ( 4.9, 5.9 );

	SetMaxCurrents( 0, 20.0 );
	SetMaxCurrents( 1, 20.0 );
	SetMaxCurrents( 2, 20.0 );
	SetMaxCurrents( 3, 20.0 );	
}

void Leg::SetMaxCurrents( int mMotorIndex, float mMaxCurrent )
{
	uint32_t maxCurrent = trunc(mMaxCurrent*10.0);	
	bool    result=false;		
	switch (mMotorIndex)
	{
	case HIP_MOTOR			: m_board_hip.SetM1MaxCurrent(0x80, maxCurrent);		break;
	case HIP_SWIVEL_MOTOR 	: m_board_hip.SetM2MaxCurrent(0x80, maxCurrent);		break;
	case KNEE_MOTOR		 	: m_board_knee.SetM1MaxCurrent(0x80, maxCurrent);		break;
	case ANKLE_MOTOR		: m_board_knee.SetM2MaxCurrent(0x80, maxCurrent);		break;
	default : break;
	}
}

void Leg::SetMainVoltageThresholds( float mMinVoltage, float mMaxVoltage )
{
	uint16_t min,max;
	min = trunc(mMinVoltage * 10);
	max = trunc(mMaxVoltage * 10);
	
	m_board_hip.SetMainVoltages ( 0x80, min, max );
	m_board_knee.SetMainVoltages( 0x80, min, max );
}
void Leg::SetLogicVoltageThresholds( float mMinVoltage, float mMaxVoltage )
{
	uint16_t min,max;
	min = trunc(mMinVoltage * 10);
	max = trunc(mMaxVoltage * 10);
	
	m_board_hip.SetLogicVoltages ( 0x80, min, max );
	m_board_knee.SetLogicVoltages( 0x80, min, max );	
}

float Leg::GetMainVoltage(  )
{
	uint16_t  mb = m_board_hip.ReadMainBatteryVoltage(0x80);
	return ((float)mb/10.0);
}
float Leg::GetLogicVoltage(  )
{
	uint16_t  lb = m_board_hip.ReadLogicBatteryVoltage(0x80);
	return ((float)lb/10.0);
}


uint16_t Leg::read_hip_status()
{
	bool valid = false;
	uint16_t res1 = m_board_hip.ReadError(0x80, &valid );
	return res1;	
}
uint16_t Leg::read_knee_status()
{
	bool valid = false;
	uint16_t res1 = m_board_knee.ReadError(0x80, &valid );
	return res1;	
}

void Leg::SavePreferences( Preferences* mpref )
{
	char str[255];
	sprintf(str, "\nLimb_type=Leg\n");	
	printf(str);				
//	mpref->write( str );

	//upper_leg.SavePreferences( mpref );
	//lower_leg.SavePreferences( mpref );
	//foot.SavePreferences	 ( mpref );

	//MotorPack::SavePreferences( mpref );
	printf("Leg:SavePref() done\n");
}

void Leg::LoadPreferences( Preferences* mpref, BOOL mConstructObjects	)
{
	char str[255];

	//upper_leg.LoadPreferences( mpref );
	//lower_leg.LoadPreferences( mpref );
	//foot.LoadPreferences	 ( mpref );

	// READ LIMB LENGTHS:
	//MotorPack::LoadPreferences( mpref );		
}

/***************** GET POSITIONS FROM ROBOCLAW BOARD **********************/
float Leg::get_hip_angle			()
{	
	bool    valid;
	uint8_t status;
	uint32_t reading = m_board_hip.ReadEncM1(0x80, &status, &valid);	
	float result = reading;
	//float result = convert_encoder_to_angle(reading);		
	return result;
}
float Leg::get_hip_swivel			()
{
	bool    valid;
	uint8_t status;
	uint32_t reading = m_board_hip.ReadEncM2(0x80, &status, &valid);
	float result = reading;
	//float result = convert_encoder_to_angle(reading);		
	return result;
}

float Leg::get_knee_angle			()
{
	bool    valid;
	uint8_t status;
	uint32_t reading = m_board_knee.ReadEncM1(0x80, &status, &valid);
	float result = reading;
	//float result = convert_encoder_to_angle(reading);		
	return result;
}
float Leg::get_ankle_angle			()
{
	bool    valid;
	uint8_t status;
	uint32_t reading = m_board_knee.ReadEncM2(0x80, &status, &valid);
	float result = reading;
	//float result = convert_encoder_to_angle(reading);		
	return result;
}

/***************** GET SPEEDS FROM ROBOCLAW BOARD **********************/
float Leg::get_hip_angular_speed	()
{	
	bool    valid;
	uint8_t status;
	uint32_t reading = m_board_hip.ReadSpeedM1(0x80, &status, &valid);
	float result = reading;
	//float result = convert_encoder_to_angle(reading);		
	return result;
}
float Leg::get_hip_swivel_speed		()
{
	bool    valid;
	uint8_t status;
	uint32_t reading = m_board_hip.ReadSpeedM2(0x80, &status, &valid);
	float result = reading;
	//float result = convert_encoder_to_angle(reading);		
	return result;
}
float Leg::get_knee_angular_speed	()
{
	bool    valid;
	uint8_t status;
	uint32_t reading = m_board_knee.ReadSpeedM1(0x80, &status, &valid);
	float result = reading;
	//float result = convert_encoder_to_angle(reading);		
	return result;
}
float Leg::get_ankle_angular_speed	()
{
	bool    valid;
	uint8_t status;
	uint32_t reading = m_board_knee.ReadSpeedM2(0x80, &status, &valid);
	float result = reading;
	//float result = convert_encoder_to_angle(reading);		
	return result;
}

/***************** SET DUTIES VIA ROBOCLAW BOARD **********************/
bool Leg::set_hip_duty				(tDUTY_TYPE mSpeed)
{
	bool result = m_board_hip.DutyM1(0x80,  mSpeed);
	//bool result = m_board_hip.ForwardM1(0x80, speed);
	return result;
}
bool Leg::set_hip_swivel_duty		(tDUTY_TYPE mSpeed)
{
	bool result = m_board_hip.DutyM2(0x80,  mSpeed);
	//bool result = m_board_hip.ForwardM2(0x80, speed);
	return result;

}
bool Leg::set_knee_angle_duty		(tDUTY_TYPE mSpeed)
{
	bool result = m_board_hip.DutyM1(0x80,  mSpeed);
	//bool result = m_board_knee.ForwardM1(0x80, speed);
	return result;
}
bool Leg::set_ankle_duty			(tDUTY_TYPE mSpeed)
{
	bool result = m_board_hip.DutyM2(0x80,  mSpeed);
	//bool result = m_board_knee.ForwardM2(0x80, speed);
	return result;
}

/***************** SET POSITIONS VIA ROBOCLAW BOARD **********************
void Leg::set_hip_angle		( float mAngleDeg )
{	
	
	Motor* m = get_motor(0);
	return m->setAngle(mAngleDeg);
}
void Leg::set_hip_swivel	( float mAngleDeg )
{	
	Motor* m = get_motor(0);
	return m->setAngle(mAngleDeg);
}
void Leg::set_knee_angle	( float mAngleDeg )
{
	Motor* m = get_motor(1);
	return m->setAngle(mAngleDeg);
}
void Leg::set_ankle_angle	( float mAngleDeg )
{
	Motor* m = get_motor(2);
	return m->setAngle(mAngleDeg);
}
*/

void Leg::inverse_kinematic( OneVector* mPosition, byte mLegLocation )
{
	float angle=0.;
	switch(mLegLocation)
	{
	case LEG_LOCATION_TOE:
			
			break;
	case LEG_LOCATION_HEEL:
			// 					
			//set_hip_angle ( angle );
			//set_knee_angle( angle );
			break;
	case LEG_LOCATION_KNEE:
			// extract the yz angle - if the distance isn't right, set the angle anyway.
			//*mPosition -= leg_attachment_position;
			//angle = asin( (*mPosition)[1] / upper_leg.length) * (180.0 / M_PI);
			//set_hip_angle( angle );
			break;
	default:
			break;
	}
}

OneVector*	Leg::getKneePosition()
{
	//OneVector* retval = new OneVector(leg_attachment_position);
	//float hip = get_hip_angle();
	
	//retval[1] = sin(hip)*upper_leg.length;
	//retval[2] = - cos(hip)*upper_leg.length;
	// z is lower near the ground.
	//return retval;
}

OneVector*	Leg::getHeelPosition()
{
	OneVector* retval = getKneePosition();
	float tmp = get_knee_angle();	
	//retval[1] = sin(tmp)*lower_leg.length;
	//retval[2] = - cos(tmp)*lower_leg.length;
	// z is lower near the ground.

	// need to add small distance from ankle to heel:
	tmp = get_ankle_angle();
	//retval[1] = sin(tmp)*lower_leg.length;
	//retval[2] = - cos(tmp)*lower_leg.length;
	return retval;
}

OneVector*	Leg::getToePosition()
{
	OneVector* retval = getHeelPosition();
	float tmp = get_ankle_angle();	
	
	//retval[1] = cos(tmp)*foot.length;
	//retval[2] = sin(tmp)*foot.length;
	return retval;	
}

/* Retrieve real measured angles & velocities and put into the Pendulumn "calculator" 
   objects.
*/
void Leg::update_pendulumn_parameters()
{
/*	upper_leg.angle = get_hip_angle();
	lower_leg.angle = get_knee_angle();
	foot.angle 		= get_ankle_angle();	
	
	upper_leg.angular_velocity = get_hip_angular_speed();
	lower_leg.angular_velocity = get_knee_angular_speed();
	foot.angular_velocity 	   = get_ankle_angular_speed(); */
}
	
void	Leg::set_to_swing_leg		()		
{
	leg_mode = SWING_LEG;  		
/*	upper_leg.set_to_swing_leg();
	lower_leg.set_to_swing_leg();
	foot.set_to_swing_leg(); */
};

void	Leg::set_to_stance_leg		()		
{
	leg_mode = STANCE_LEG;  		
/*	upper_leg.set_to_stance_leg();
	lower_leg.set_to_stance_leg();
	foot.set_to_stance_leg();	*/
};
void	Leg::set_to_standing_leg	()		
{
	leg_mode = WEIGHT_SHARED_LEG;
};


/* The power put into the motor will be proportional to:
	amount needed to maintain the speed    :  a * (speed of motor)
	amount needed to overcome gravity	   :  b * (mg * sin(theta))
	amount needed to acheive desired accel :  c * (requested accel - comes from diff of position vectors)
	amount needed to overcome torques of other joints:  d * (combined torques)
	
*/
OneVector* Leg::calculate_duties ( OneVector* mPos, OneVector* mVelocity, OneVector* mAcceleration )
{
/*	OneVector* duty = new OneVector( numItems );
	if (leg_mode == SWING_LEG)
	{
		* we start assuming each pendulumn is independant.  *
		float torque = upper_leg.find_motor_torque( (*mVelocity)[0] );
		duty->set_element( get_motor(0)->getDuty( torque ), 0 );
		
		torque = lower_leg.find_motor_torque( (*mVelocity)[1] );
		duty->set_element( get_motor(0)->getDuty( torque ), 1 );

		torque = foot.find_motor_torque( (*mVelocity)[2] );
		duty->set_element( get_motor(0)->getDuty( torque ), 2 );
		return duty;
	}
	else if (leg_mode == STANCE_LEG)
	{
	}
	else 
	{
	}	
	*/
	return mPos;
}

/* For now, let's have each sequence start at <0,0,0>.  So we can assume there's no
	huge catchup that has to be done on the position.  The first position will be a reasonable
	difference from 0 within the velocity requested.  ie. the velocity is consistent with  
	the 2nd position.

*/
void Leg::apply_vector	( VectorSequence* mvs )
{
	//update_pendulumn_parameters();
	//OneVector* duties = calculate_duties( mvs->get_vector(), mvs->get_ivelocity_vector(), mvs->get_iaccel_vector() );
	//set_duty_vector( duties->get_data(), duties->get_dimension() );	
};
