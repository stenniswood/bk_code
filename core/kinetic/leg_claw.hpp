#ifndef _LEG_H_
#define _LEG_H_

#include "Container.hpp"
#include <string>

//#include "motor.hpp"
//#include "pendulumn.hpp"

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


/*******************************************
A Leg is a Motor Pack that handles computations of torques.
Most obvious from jog the legs are the Ankles move so easily it's
trivial, knees okay, and hips require some strength to overcome the
static friction/gravity.
*******************************************/
class Leg 
{
public:
	Leg( char* mDevHip, char* mDevKneeAnkle);
	void			Init();
	void 			SetMainVoltageThresholds ( float mMinVoltage=10.0, float mMaxVoltage=25.0 );
	void 			SetLogicVoltageThresholds( float mMinVoltage,      float mMaxVoltage );
	float 			GetMainVoltage			();
	float 			GetLogicVoltage			();
	void 			SetMaxCurrents			( int mMotorIndex, float mMaxCurrent );
	uint16_t 		read_hip_status			();	// See Roboclaw Error codes!
	uint16_t 		read_knee_status		(); // See Roboclaw Error codes!
		
	// ACCESSOR FUNCTIONS:
	float 			get_hip_angle			();
	float 			get_hip_swivel			();	
	float 			get_knee_angle			();
	float 			get_ankle_angle			();

	float 			get_hip_angular_speed	();
	float 			get_hip_swivel_speed	();
	float 			get_knee_angular_speed	();
	float 			get_ankle_angular_speed	();

	bool 			set_hip_duty			(tDUTY_TYPE mSpeed);
	bool 			set_hip_swivel_duty		(tDUTY_TYPE mSpeed);
	bool 			set_knee_angle_duty		(tDUTY_TYPE mSpeed);
	bool 			set_ankle_duty			(tDUTY_TYPE mSpeed);
	
/* Not implemented yet, b/c what to do with the duties under variable loads 
	void 			set_hip_angle			(float mAngle);
	void 			set_hip_swivel_angle	(float mAngle);
	void 			set_knee_angle			(float mAngle);
	void 			set_ankle_angle			(float mAngle); */

	void			set_to_swing_leg		();
 	void			set_to_stance_leg		();
 	void			set_to_standing_leg		();

	OneVector*		get_robot_coordinate_leg_attachment_position();
	void 			set_robot_coordinate_leg_attachment_position(OneVector* mAttachementPosition );

	// This can be used to specify "toe to <x,y,z>" or "heel to <xyz>" or even "knee to <xyz>"
	void 			inverse_kinematic( OneVector* mPosition, byte mLegLocation 	   );
	OneVector*		getKneePosition();
	OneVector*		getHeelPosition();
	OneVector*		getToePosition ();

	virtual void	SavePreferences		( Preferences* mpref			   );		// stops and ...
	virtual void	LoadPreferences		( Preferences* mpref, BOOL mConstructObjects = TRUE	);	

	/* the parameters come from the vector file.  They are based on requested values */
	void			update_pendulumn_parameters();	
	virtual OneVector* calculate_duties ( OneVector* mPos, OneVector* mVelocity, OneVector* mAcceleration );
	void 			   apply_vector		( VectorSequence* mvs );
	
private:
	OneVector*	leg_attachment_position;	// used for finding knee and toe positions, etc.
	
	RoboClaw	m_board_hip;		// hip rotate and swing.
	RoboClaw	m_board_knee;		// knee and ankle.
	std::string	m_name;
	
	float		ankle_height;		// distance from heel to ankle.	
	byte		leg_mode;			// swing, stance, or shared (both legs bearing weight)
};
 
#endif



/* So far we've not had any reference to the weight, lengths, or inertia's of 
   the parts of the leg.
   These should be stored into a config file.  Or they could be determined 
   algorithmically by trial runs of how leg responds to a duty.   
*/

/*	Pendulum	upper_leg;
	Pendulum	lower_leg;
	Pendulum	foot;		*/
