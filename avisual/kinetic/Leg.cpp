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
#include "CAN_Interface.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "can_txbuff.h"
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
#include "Leg.hpp"
#include "vector_file.hpp"


Leg::Leg( char* mName )
: MotorPack(mName)
{
}

void Leg::SavePreferences( Preferences* mpref		)
{
	char str[50];
	sprintf(str, "\nLimb_type=Leg\n");	
	printf(str);				
	mpref->write( str );

	MotorPack::SavePreferences( mpref );
}

void Leg::LoadPreferences( Preferences* mpref, BOOL mConstructObjects	)
{

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



/* The power put into the motor will be proportional to:
	amount needed to maintain the speed    :  a * (speed of motor)
	amount needed to overcome gravity	   :  b * (mg * sin(theta))
	amount needed to acheive desired accel :  c * (requested accel - comes from diff of position vectors)
	amount needed to overcome torques of other joints:  d * (combined torques)
	
*/
OneVector* Leg::calculate_duties ( OneVector* mPos, OneVector* mVelocity, OneVector* mAcceleration )
{
	//OneVector* duty = new OneVector( numItems );
	
	return mPos;
}

