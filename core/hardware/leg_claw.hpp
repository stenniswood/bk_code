#ifndef _LEG_H_
#define _LEG_H_

/* So far we've not had any reference to the weight, lengths, or inertia's of 
   the parts of the leg.  This will be added in a derived Leg class.
*/

//#include "vector_file.hpp"
#include <string>
#include "roboclaw.h"
#include "vector.hpp"

#define HIP_MOTOR		  0
#define HIP_SWIVEL_MOTOR  1
#define KNEE_MOTOR		  2
#define ANKLE_MOTOR		  3

#define LEG_LOCATION_NONE 0
#define LEG_LOCATION_TOE  1
#define LEG_LOCATION_HEEL 2
#define LEG_LOCATION_KNEE 3

#define SWING_LEG 			0
#define STANCE_LEG 			1
#define WEIGHT_SHARED_LEG 	2		// both legs bearing weight.

#define tDUTY_TYPE uint16_t

/***** RoboClaw Error Bits *****/
const int16_t Normal					= 0x0000;
const int16_t M1_OverCurrent_Warning	= 0x0001;
const int16_t M2_OverCurrent_Warning	= 0x0002;
const int16_t E_Stop					= 0x0004;
const int16_t Temperature_Error			= 0x0008;
const int16_t Temperature2_Error		= 0x0010;
const int16_t Main_Battery_High_Error	= 0x0020;
const int16_t Logic_Battery_High_Error	= 0x0040;
const int16_t Logic_Battery_Low_Error	= 0x0080;
const int16_t M1_Driver_Fault			= 0x0100;
const int16_t M2_Driver_Fault			= 0x0200;
const int16_t Main_Battery_High_Warning	= 0x0400;
const int16_t Main_Battery_Low_Warning	= 0x0800;
const int16_t Termperature_Warning		= 0x1000;
const int16_t Temperature2_Warning		= 0x2000;
const int16_t M1_Home					= 0x4000;
const int16_t M2_Home					= 0x8000;

/* The Pots are 300 degrees per 2v */
const float degs_per_volt = 300.0 / 2.0;
const float volts_per_deg = 1.0 / degs_per_volt;


class MotorCalibration
{
public:
	MotorCalibration();

	float		get_min_angle		();		// based on calibrated value.
	float		get_max_angle		();		// based on calibrated value.

	float		get_angle			();
	float		get_voltage			( float mAngleDegrees );
	void		set_calibrate_angle ( float mAngleDegrees );
	
	uint32_t	measured_voltage;
	uint32_t	zeroDegreeVoltage;		// Assuming Pot encoders - the voltage corresponding to 0 degs.
};

void read_main_battery(RoboClaw& m_hip, RoboClaw& m_knee);
void read_version(RoboClaw& m_hip, RoboClaw& m_knee);

/*******************************************
A Leg is a Motor Pack that handles computations of torques.
Most obvious from jog the legs are the Ankles move so easily it's
trivial, knees okay, and hips require some strength to overcome the
static friction/gravity.
*******************************************/
class Leg 
{
public:
	Leg   			( char* mDevHip, char* mDevKneeAnkle );
	~Leg			();
	
	void			Init();
	void			configure_boards();
	void			verify_working_status();	
	void			setup_encoders();	
	void			read_encoders();
	
	void			read_versions();
	
	void 			SetMainVoltageThresholds ( float mMinVoltage=10.0, float mMaxVoltage=25.0 );
	void 			SetLogicVoltageThresholds( float mMinVoltage=10.0, float mMaxVoltage=25.0 );
	
	float 			GetMainVoltage			( );
	float 			GetLogicVoltage			( );
	void 			SetMaxCurrents			( int mMotorIndex, float mMaxCurrent );
	uint16_t 		read_hip_status			( );	// See Roboclaw Error codes!
	uint16_t 		read_knee_status		( ); 	// See Roboclaw Error codes!

	void	 		read_currents			( ); 	
	void	 		read_status				( ); 		

	void			stop_all_motors			();
	int				get_num_motors			()	{ return 4; };

	// ACCESSOR FUNCTIONS:
	float 			get_hip_angle			();
	float 			get_hip_swivel			();	
	float 			get_knee_angle			();
	float 			get_ankle_angle			();

	float 			get_hip_angular_speed	();
	float 			get_hip_swivel_speed	();
	float 			get_knee_angular_speed	();
	float 			get_ankle_angular_speed	();

	bool 			set_hip_duty			(float mFraction);
	bool 			set_hip_swivel_duty		(float mFraction);
	bool 			set_knee_duty			(float mFraction);
	bool 			set_ankle_duty			(float mFraction);
//	bool 			set_duty_vector			(vector<float>& mFractions);
	bool 			set_duty_vector			(MathVector& mFractions);
	
/* Not implemented yet, b/c what to do with the duties under variable loads 
	void 			set_hip_angle			(float mAngle);
	void 			set_hip_swivel_angle	(float mAngle);
	void 			set_knee_angle			(float mAngle);
	void 			set_ankle_angle			(float mAngle); */
//	virtual void	SavePreferences		( Preferences* mpref			   );		// stops and ...
//	virtual void	LoadPreferences		( Preferences* mpref, BOOL mConstructObjects = TRUE	);	

	void			set_to_swing_leg		();
 	void			set_to_stance_leg		();
 	void			set_to_standing_leg		();
	
private:
	MotorCalibration cals[4];
	
	RoboClaw	m_board_hip;		// hip rotate and swing.
	RoboClaw	m_board_knee;		// knee and ankle.
	std::string	m_name;
	uint8_t		leg_mode;			// swing, stance, or shared (both legs bearing weight)

	float		m_battery_voltage;
	float 		m_currents[4];
	uint32_t 	m_hip_enc;
	uint32_t 	m_hip_swivel_enc;
	uint32_t 	m_knee_enc;
	uint32_t 	m_ankle_enc;	
};
 
#endif



