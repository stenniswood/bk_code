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
#include "global.h"
#include "roboclaw.h"
#include "leg_claw.hpp"


uint8_t address = 0x80;		// no use with usb 
int BaudRate = 	B38400;	// B9600; B14400 B19200; B38400; 57600; 115200;

/*********************************************/
/********  CLASS - MotorCalibration  *********/
/*********************************************/
MotorCalibration::MotorCalibration()
{
	
}
float MotorCalibration::get_min_angle		()		// based on calibrated value.
{
	float delta  = (0 - zeroDegreeVoltage);
	float result = delta * degs_per_volt;
	return result;
}
float MotorCalibration::get_max_angle		()		// based on calibrated value.
{
	float delta  = (2.0 - zeroDegreeVoltage);
	float result = delta * degs_per_volt;
	return result;
}

float MotorCalibration::get_angle()
{
	float delta  = (measured_voltage - zeroDegreeVoltage);
	float result = delta * degs_per_volt;
	return result;
}
// This determines the zeroDegreeVoltage (Caller may specify an angle other than
// zero - for instance "45 degrees at current voltage"
// or "90 degrees at current voltage (measured_voltage)"
void MotorCalibration::set_calibrate_angle( float mAngleDegrees )
{
	float volts_above_zero = mAngleDegrees*volts_per_deg;
	zeroDegreeVoltage = measured_voltage - volts_above_zero;
}

float MotorCalibration::get_voltage(float mAngleDegrees )
{	
	float result = mAngleDegrees*volts_per_deg + zeroDegreeVoltage;
	return result;
}

void read_main_battery(RoboClaw& m_hip, RoboClaw& m_knee)
{
	uint16_t volts1 = m_hip.ReadMainBatteryVoltage ( 0x80 );
	uint16_t volts2 = m_knee.ReadMainBatteryVoltage( 0x80 );
	if (m_hip.connected)
		printf("\nMain Battery board #1 = %6.1f volts\n", (float)volts1/10.);
	if (m_knee.connected)
		printf("Main Battery board #2 = %6.1f volts\n",   (float)volts2/10.);
}

/************************************************************/
/************************************************************/
Leg::Leg( char* mDevHip, char* mDevKneeAnkle)
: m_board_hip ( mDevHip, 8000), 
  m_board_knee( mDevKneeAnkle, 8000)
{
	Init();
}

Leg::~Leg( )
{
	m_board_hip.close();
	m_board_knee.close();
}

void Leg::Init()
{
	usleep(10000);
	bool valid=false;
	//printf("Leg::Init()\n");
	m_board_hip.open			(  );
	m_board_hip.set_baud		( BaudRate );
	m_board_knee.open			(  );
	m_board_knee.set_baud		( BaudRate );
	
	m_board_hip.flush ();
	m_board_knee.flush();

//	read_main_battery(m_board_hip, m_board_knee);
//	read_version	 (m_board_hip, m_board_knee);

	//GetMainVoltage ();
	//GetLogicVoltage();
	//setup_encoders  ();
	//configure_boards();
	//printf("Leg::Init() done\n");	
}

void Leg::read_versions()
{
	static uint8_t address = 0x80;
	char text[80];
	memset(text,0,79);
	bool result = m_board_hip.ReadVersion( address, text );
	if (m_board_hip.connected)
		printf("\ntext=%s\n", text);

//	memset(text, 0, 79);
//	result = m_board_knee.ReadVersion( address, text );	
//	if (m_board_knee.connected)
//		printf("\ntext=%s\n", text);	
}

void Leg::setup_encoders()
{
	uint8_t M1mode = 0x81;
	uint8_t M2mode = 0x81;
	uint8_t address= 0x80;

	printf("Leg::setup_encoders() Hip board\n");
	bool result = m_board_hip.SetM1EncoderMode( address, M1mode );
	result = m_board_hip.SetM2EncoderMode( address, M2mode );
	
	printf("Leg::setup_encoders() Knee board\n");	
	result = m_board_knee.SetM1EncoderMode( address, M1mode );
	result = m_board_knee.SetM2EncoderMode( address, M2mode );
	
}
void Leg::read_encoders()
{
	uint8_t M1mode = 0x81;
	uint8_t M2mode = 0x81;

	bool result = m_board_hip.ReadEncoderModes( address, M1mode, M2mode );
	printf("read_encoders()  Modes m1,m2 = %x,%x \n", M1mode, M2mode );

	uint8_t status;
	bool valid= false;
	m_hip_enc 		 = m_board_hip.ReadEncM1(address, &status, &valid);
	m_hip_swivel_enc = m_board_hip.ReadEncM2(address, &status, &valid);
	printf("Encoders m1,m2= %6.2f, %6.2f \n", 2.0*m_hip_enc/2047., 2.0*m_hip_swivel_enc/2047. );

	m_knee_enc 		 = m_board_knee.ReadEncM1(address, &status, &valid);
	m_ankle_enc		 = m_board_knee.ReadEncM2(address, &status, &valid);
	
	result = m_board_knee.ReadEncoders( address, m_knee_enc, m_ankle_enc);
	printf("Encoders m1,m2= %6.2f, %6.2f \n", 2.0*m_knee_enc/2047., 2.0*m_ankle_enc/2047. );
}

void Leg::read_status( )
{
	uint16_t error1 = read_hip_status();
	uint16_t error2 = read_knee_status();
	//printf("hip,knee claw Error = %4x, %4x\n", error1, error2);
}
void Leg::read_currents	( )
{
	int16_t current1=0;
	int16_t current2=0;	
	bool result = m_board_hip.ReadCurrents( address, current1, current2 );
	m_currents[0] = current1/100.0;
	m_currents[1] = current2/100.0;

	result = m_board_knee.ReadCurrents( address, current1, current2 );
	m_currents[2] = current1/100.0;
	m_currents[3] = current2/100.0;

	printf("Hip  Current: m1,m2= %6.2f, %6.2f\n", m_currents[0], m_currents[1] );
	printf("Knee Current: m1,m2= %6.2f, %6.2f\n", m_currents[2], m_currents[3] );
}

void Leg::configure_boards()
{
	SetMainVoltageThresholds ();
	SetLogicVoltageThresholds();
	
	m_board_hip.SetConfig ( 0x80, 0x0060 );	// B38400 baud		
	m_board_knee.SetConfig( 0x80, 0x0060 );	// B38400 baud
	usleep(100000);
	
	setup_encoders();
	
	SetMaxCurrents( 0, 20.0 );
	SetMaxCurrents( 1, 20.0 );
	SetMaxCurrents( 2, 20.0 );
	SetMaxCurrents( 3, 20.0 ); 	
	usleep(100000);	
}
void Leg::verify_working_status()
{
	read_main_battery(m_board_hip, m_board_knee);
	read_versions();

	uint16_t config1,config2;
	bool result = m_board_hip.GetConfig( address, config1 );
	result = m_board_knee.GetConfig( address, config2 );
	printf("Hip,Knee claw config=%4x, %4x\n", config1, config2);

	read_status();
}

void Leg::SetMaxCurrents( int mMotorIndex, float mMaxCurrent )
{
	printf("Leg::SetMaxCurrents() \n");	
	bool    result=false;		
	switch (mMotorIndex)
	{
	case HIP_MOTOR			: m_board_hip.SetM1MaxCurrent(0x80, mMaxCurrent);		break;
	case HIP_SWIVEL_MOTOR 	: m_board_hip.SetM2MaxCurrent(0x80, mMaxCurrent);		break;
	case KNEE_MOTOR		 	: m_board_knee.SetM1MaxCurrent(0x80, mMaxCurrent);		break;
	case ANKLE_MOTOR		: m_board_knee.SetM2MaxCurrent(0x80, mMaxCurrent);		break;
	default : break;
	}
}

void Leg::SetMainVoltageThresholds( float mMinVoltage, float mMaxVoltage )
{
	printf("Leg::SetMainVoltageThresholds() \n");		
	m_board_hip.SetMainVoltages ( 0x80, mMinVoltage, mMaxVoltage );
	m_board_knee.SetMainVoltages( 0x80, mMinVoltage, mMaxVoltage );
}
void Leg::SetLogicVoltageThresholds( float mMinVoltage, float mMaxVoltage )
{
	printf("Leg::SetLogicVoltageThresholds() \n");
	m_board_hip.SetLogicVoltages ( 0x80, mMinVoltage, mMaxVoltage );
	m_board_knee.SetLogicVoltages( 0x80, mMinVoltage, mMaxVoltage );
}

float Leg::GetMainVoltage(  )
{
	uint16_t  mb = m_board_hip.ReadMainBatteryVoltage(0x80);
	m_battery_voltage = mb / 10.0;
	if (m_board_hip.connected)
		printf("Leg::GetMainVoltage() = %6.1f \n", m_battery_voltage);
	return (m_battery_voltage);
}
float Leg::GetLogicVoltage(  )
{
	uint16_t  lb = m_board_hip.ReadLogicBatteryVoltage(0x80);
	float battery_voltage = lb / 10.0;	
	if (m_board_hip.connected)
		printf("Leg::GetLogicVoltage() = %6.1f \n", battery_voltage);
	return (battery_voltage);
}

uint16_t Leg::read_hip_status()
{
	bool valid = false;
	uint16_t res1 = m_board_hip.ReadError(0x80, &valid );
	char* ptr = get_error_string( res1 );
	printf("Hip status: %4x = %s\n", res1, ptr );	
	return res1;	
}
uint16_t Leg::read_knee_status()
{
	bool valid = false;
	uint16_t res1 = m_board_knee.ReadError(0x80, &valid );
	char* ptr = get_error_string( res1 );
	printf("Knee status: %4x = %s\n", res1, ptr );
	return res1;	
}

/*void Leg::SavePreferences( Preferences* mpref )
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
} */

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

void Leg::stop_all_motors			()
{
	m_board_hip.ForwardM1(0x80, 0);
	m_board_hip.ForwardM2(0x80, 0);
	
	m_board_knee.ForwardM1(0x80, 0);
	m_board_knee.ForwardM2(0x80, 0);	
}

/***************** SET DUTIES VIA ROBOCLAW BOARD **********************/
bool Leg::set_hip_duty				(float mFraction)
{
	/**** NOTE:  Hip=knee!  So had to switch!  ****/
	
	uint32_t duty  = mFraction * 32767;
	uint8_t speed = fabs(mFraction * 127);
	bool result;
	if (mFraction<0)
		result = m_board_hip.BackwardM2(0x80, speed);
	else 	
		result = m_board_hip.ForwardM2(0x80, speed);
	//bool result = m_board_hip.DutyM2(0x80,  duty);		
	return result;
}
bool Leg::set_hip_swivel_duty		(float mFraction)
{
	uint32_t duty  = mFraction * 32767;
	uint8_t speed = fabs(mFraction * 127);	
	///bool result = m_board_hip.DutyM1(0x80,  duty);
	bool result;
	if (mFraction<0)
		result = m_board_knee.BackwardM1(0x80, speed);
	else
		result = m_board_knee.ForwardM1(0x80, speed);
	return result;
}
bool Leg::set_knee_duty				(float mFraction)
{
	uint32_t duty  = mFraction * 32767;
	uint8_t  speed = fabs(mFraction * 127);

	bool result;
	if (mFraction<0)
		result = m_board_hip.BackwardM1(0x80, speed);
	else 	
		result = m_board_hip.ForwardM1(0x80, speed);
	return result;

}
bool Leg::set_ankle_duty			(float mFraction)
{
	uint32_t duty = mFraction * 32767;
	uint8_t speed = fabs(mFraction * 127);

	bool result;
	if (mFraction<0)
		result = m_board_knee.BackwardM2(0x80, speed);
	else 
		result = m_board_knee.ForwardM2 (0x80, speed);
	//bool result = m_board_knee.ForwardM2(0x80, speed);
	return result;
}

bool Leg::set_duty_vector(MathVector& mFractions)
{
	set_hip_swivel_duty	(mFractions[0]);
	set_hip_duty		(mFractions[1]);
	set_knee_duty		(mFractions[2]);
	set_ankle_duty		(mFractions[3]);	
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


/* Retrieve real measured angles & velocities and put into the Pendulumn "calculator" 
   objects.
*/
void	Leg::set_to_swing_leg		()		
{
	leg_mode = SWING_LEG;  		
};

void	Leg::set_to_stance_leg		()		
{
	leg_mode = STANCE_LEG;  		
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

/* For now, let's have each sequence start at <0,0,0>.  So we can assume there's no
	huge catchup that has to be done on the position.  The first position will be a reasonable
	difference from 0 within the velocity requested.  ie. the velocity is consistent with  
	the 2nd position.

*/
