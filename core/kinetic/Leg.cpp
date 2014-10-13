// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "bk_system_defs.h"
#include "CAN_base.h"
//#include "CAN_Interface.h"
#include "packer_lcd.h"
#include "packer_motor.h"
//#include "can_txbuff.h"
#include "packer.h"
#include "can_id_list.h"
#include "vector_math.h"
#include "cmd_process.h"
#include "parser_tilt.h"
#include "buttons.h"
#include "fuse_ag.h"
#include "fuse_accel.h"
#include "fuse_magnet.h"
#include "fuse_gyro.h"
#include "fuse_log.h"
#include "dataset.hpp"
#include "leveler.hpp"
#include "display.h"
#include "line_graph.hpp"
#include "leveler.hpp"
#include "histogram.hpp"
#include "stick_leg.hpp"
#include "window_layouts.hpp"
#include "callbacks.hpp"
#include "motor.hpp"
#include "motor_pack.hpp"
#include "vector_file.hpp"
#include "ini_file.hpp"
#include "Leg.hpp"



Leg::Leg( char* mName )
: MotorPack(mName)
{
}

void Leg::SavePreferences( Preferences* mpref		)
{
	char str[255];
	sprintf(str, "\nLimb_type=Leg\n");	
	printf(str);				
	mpref->write( str );

	upper_leg.SavePreferences( mpref );
	lower_leg.SavePreferences( mpref );
	foot.SavePreferences	 ( mpref );

	MotorPack::SavePreferences( mpref );
	printf("Leg:SavePref() done\n");
}

void Leg::LoadPreferences( Preferences* mpref, BOOL mConstructObjects	)
{
	char str[255];

	upper_leg.LoadPreferences( mpref );
	lower_leg.LoadPreferences( mpref );
	foot.LoadPreferences	 ( mpref );

	// READ LIMB LENGTHS:
	MotorPack::LoadPreferences( mpref );		
}

float Leg::get_hip_angle			()
{	
	Motor* m = get_motor(0);
	return m->getAngle();
}
float Leg::get_knee_angle			()
{
	Motor* m = get_motor(1);
	return m->getAngle();
}
float Leg::get_ankle_angle			()
{
	Motor* m = get_motor(2);
	return m->getAngle();
}


float Leg::get_hip_angular_speed	()
{
	Motor* m = get_motor(0);
	return m->getSpeed();
}
float Leg::get_knee_angular_speed	()
{
	Motor* m = get_motor(1);
	return m->getSpeed();
}
float Leg::get_ankle_angular_speed	()
{
	Motor* m = get_motor(2);
	return m->getSpeed();
}


void Leg::set_hip_angle		( float mAngleDeg )
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

void Leg::inverse_kinematic( OneVector* mPosition, byte mLegLocation )
{
	float angle=0.;
	switch(mLegLocation)
	{
	case LEG_LOCATION_TOE:
			
			break;
	case LEG_LOCATION_HEEL:
			// 					
			set_hip_angle ( angle );
			set_knee_angle( angle );
			break;
	case LEG_LOCATION_KNEE:
			// extract the yz angle - if the distance isn't right, set the angle anyway.
			*mPosition -= leg_attachment_position;
			angle = asin( (*mPosition)[1] / upper_leg.length) * (180.0 / M_PI);
			set_hip_angle( angle );
			break;
	default:
			break;
	}
}

OneVector*	Leg::getKneePosition()
{
	OneVector* retval = new OneVector(leg_attachment_position);
	float hip = get_hip_angle();
	
	retval[1] = sin(hip)*upper_leg.length;
	retval[2] = - cos(hip)*upper_leg.length;
	// z is lower near the ground.
	return retval;
}

OneVector*	Leg::getHeelPosition()
{
	OneVector* retval = getKneePosition();
	float tmp = get_knee_angle();	
	retval[1] = sin(tmp)*lower_leg.length;
	retval[2] = - cos(tmp)*lower_leg.length;
	// z is lower near the ground.

	// need to add small distance from ankle to heel:
	tmp = get_ankle_angle();
	retval[1] = sin(tmp)*lower_leg.length;
	retval[2] = - cos(tmp)*lower_leg.length;
	return retval;
}

OneVector*	Leg::getToePosition()
{
	OneVector* retval = getHeelPosition();
	float tmp = get_ankle_angle();	
	
	retval[1] = cos(tmp)*foot.length;
	retval[2] = sin(tmp)*foot.length;
	return retval;	
}

/* Retrieve real measured angles & velocities and put into the Pendulumn "calculator" 
   objects.
*/
void Leg::update_pendulumn_parameters()
{
	upper_leg.angle = get_hip_angle();
	lower_leg.angle = get_knee_angle();
	foot.angle 		= get_ankle_angle();	
	
	upper_leg.angular_velocity = get_hip_angular_speed();
	lower_leg.angular_velocity = get_knee_angular_speed();
	foot.angular_velocity 	   = get_ankle_angular_speed();
}
	
void	Leg::set_to_swing_leg		()		
{
	leg_mode = SWING_LEG;  		
	upper_leg.set_to_swing_leg();
	lower_leg.set_to_swing_leg();
	foot.set_to_swing_leg();
};

void	Leg::set_to_stance_leg		()		
{
	leg_mode = STANCE_LEG;  		
	upper_leg.set_to_stance_leg();
	lower_leg.set_to_stance_leg();
	foot.set_to_stance_leg();	
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
	OneVector* duty = new OneVector( numItems );
	if (leg_mode == SWING_LEG)
	{
		/* we start assuming each pendulumn is independant.  */
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
	return mPos;
}

/* For now, let's have each sequence start at <0,0,0>.  So we can assume there's no
	huge catchup that has to be done on the position.  The first position will be a reasonable
	difference from 0 within the velocity requested.  ie. the velocity is consistent with  
	the 2nd position.

*/
void Leg::apply_vector	( VectorSequence* mvs )
{
	update_pendulumn_parameters();
	OneVector* duties = calculate_duties( mvs->get_vector(), mvs->get_ivelocity_vector(), mvs->get_iaccel_vector() );
	set_duty_vector( duties->get_data(), duties->get_dimension() );	
};
