#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <signal.h>
#include <time.h>
#include "mcp2515.h"
#include "pican_defines.h"
#include <wiringPi.h>
#include <string.h>
#include "bk_system_defs.h"
#include "can_eid.h"
#include "CAN_Interface.hpp"
#include "can_instance.hpp"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "system_msgs_callback.h"
#include "board_list_oop.hpp"
#include "packer_motor.h"
//#include "motor_vector.h"
//#include "config_file.h"
#include "vector_file.hpp"
#include "string_util.h"
#include "robot.hpp"
#include "point_mass.hpp"


//  
PointMass::PointMass( float mMass, float mElasticity )
{
	Position 	 = new OneVector( 3 );
	Velocity 	 = new OneVector( 3 );
	Acceleration = new OneVector( 3 );
	Elasticity   = mElasticity;
	Mass		 = mMass;
}

// Copy ctor
PointMass::PointMass( PointMass* mpm )
{
	Position 	 = new OneVector( mpm->Position 	);
	Velocity 	 = new OneVector( mpm->Velocity 	);
	Acceleration = new OneVector( mpm->Acceleration );
	
	Elasticity	 = mpm->Elasticity;
	Mass		 = mpm->Mass;		
}

/**********************************************************
	Tell how far away anothe object is.
***********************************************************/
float PointMass::get_geometrical_distance(PointMass* mObject2 )
{
//	PVector delta = PVector.sub(Position, mObject2->Position );
	return 0; //delta.mag();
}

void	PointMass::set_position			( OneVector* ov	 		)
{
}

OneVector*	PointMass::get_kinetic_energy		(  			)
{
	return NULL;
}

OneVector*	PointMass::get_potential_energy	( float mGroundHeight 	)
{
	return NULL;
}



float PointMass::get_speed() 				
{ 
	return sqrt((Velocity->x()*Velocity->x()) + (Velocity->y()*Velocity->y()) + (Velocity->z()*Velocity->z()));   
}
float PointMass::get_horizontal_velocity()    
{ 
	return sqrt((Velocity->x()*Velocity->x()) + (Velocity->z()*Velocity->z()));                 
}    
float PointMass::get_horizontal_accel()
{ 
	return sqrt((Acceleration->x()*Acceleration->x()) + (Acceleration->z()*Acceleration->z())); 
}  
float PointMass::get_vertical_velocity()      
{ 
	return Velocity->y();     
}  
float PointMass::get_vertical_accel()         
{ 
	return Acceleration->y(); 
}

/*void PointMass::PrintPosition( String mName ) 
{
	System.out.printf("\n%s TranslationalPosition x=%6.1f \ty=%6.1f \tz=%6.1f ", mName, TranslationalPosition.x(), TranslationalPosition.y(), TranslationalPosition.z() );
}
void PointMass::PrintVelocity( String mName ) 
{
	System.out.printf("\n%s Velocity x=%6.1f \ty=%6.1f \tz=%6.1f ", mName, Velocity->x(), Velocity->y(), Velocity->z() );
}
void PointMass::PrintAcceleration( String mName ) 
{
	System.out.printf("\n%s Acceleration x=%6.1f \ty=%6.1f \tz=%6.1f ", mName, Acceleration->x(), Acceleration->y(), Acceleration->z() );
}*/



