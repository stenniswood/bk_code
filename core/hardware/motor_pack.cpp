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
#include "CAN_Interface.hpp"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "can_txbuff.hpp"
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

#include "vector_file.hpp"
#include "motor_pack.hpp"


//class OneVector;


MotorPack::MotorPack( char* mName )
{
	pack_name = mName;
	
	config_bytes[0] = 0;
	config_bytes[1] = 0;
	config_bytes[2] = 0;
	config_bytes[3] = 0;
}

void  MotorPack::add_motor( Motor* NewMotor )
{
	Item* item = new Item();		// wrap into an Item.
	item->setData( NewMotor );
	Add(item);	
}

void MotorPack::set_angle_vector( float* mData, int n, BOOL mAnimationOnly )
{
	int    c    = 0;
	Item*  item = getHead();
	Motor* ptr = NULL;
	while ((item) && (c<n))
	{
		ptr = (Motor*)item->getData();
		if (mAnimationOnly)
			ptr->setAngle( mData[c] );
		else 
			ptr->move_to_angle( mData[c], 1000 );	// speed is tenth of 100 so 1000
		item = item->getNext();
		c++;
	}
}


void MotorPack::set_angle_onevector( OneVector* mOV_rad, BOOL mAnimationOnly )
{
	float* data = mOV_rad->get_data();
	set_angle_vector( data, getNumItems(), mAnimationOnly );
}

/*void MotorPack::set_duty_onevector( OneVector* mOV )
{
	float* data = mOV->get_data();
	set_duty_vector( data, getNumItems() );
}*/

/* The Limb classes will calculate this duty according to their algorithms
   and call this to set - updated each motion timeslice. 
 */
void MotorPack::set_duty_vector( float* mFractions, int n )
{
	int    c    = 0;
	Item*  item = getHead();
	Motor* ptr = NULL;
	while ((item) && (c<n))
	{
		ptr = (Motor*)item->getData();
		ptr->move_speed( mFractions[c]*100. );	// speed is tenth of 100 so 1000
		item = item->getNext();		
		c++;
	}
}

void MotorPack::get_angle_vector( float* mData, int* n )
{
	*n = getNumItems();
	int    c    = 0;
	Item*  item = getHead();
	Motor* ptr  = NULL;
	while ((item) && (c<*n))
	{
		ptr      = (Motor*)item->getData();
		mData[c] = ptr->getAngle();
		item     = item->getNext();
		c++;
	}
}

/* These are 0 indexed not 1!   Trust me. */
Motor* MotorPack::get_motor( int mIndex )
{
	int    c    = 0;
	Motor* ptr = NULL;
	Item*  item = getHead();
	while ((item) && (c<mIndex))
	{
		item = item->getNext();
		c++;
	}	
	if (item)
	{
		ptr = (Motor*)item->getData();
		return ptr;
	}
	return NULL;	
}	


void  MotorPack::start_reporting( )	// For all boards!
{
	Item*  item = getHead();
	Motor* ptr = NULL;
	while (item)
	{
		ptr = (Motor*)item->getData();
		ptr->set_config_byte( 2, config_bytes[1] );		// messages will be sent.
		item = item->getNext();		
	}
}
void	MotorPack::stop_reporting	( )	// For all boards!
{
	Item*  item = getHead();
	Motor* ptr = NULL;
	while (item)
	{
		ptr = (Motor*)item->getData();
		ptr->set_config_byte( 2, 0x00 );
		item = item->getNext();		
	}
}

void	MotorPack::set_config_byte	( byte mindex, byte mvalue    )  
{ 
	config_bytes[mindex-1] = mvalue; 
	Item*  item = getHead();
	Motor* ptr = NULL;
	while (item)
	{
		ptr = (Motor*)item->getData();
		ptr->set_config_byte( mindex, config_bytes[mindex-1] );
		item = item->getNext();		
	}
}
void MotorPack::send_stops()
{
	Item*  item = getHead();
	Motor* ptr = NULL;
	while (item)
	{
		ptr = (Motor*)item->getData();
		ptr->send_stops( );
		item = item->getNext();		
	}
}
void MotorPack::verify_stops()
{
	Item*  item = getHead();
	Motor* ptr = NULL;
	while (item)
	{
		ptr = (Motor*)item->getData();
		ptr->verifyStops( );
		item = item->getNext();		
	}
}
	
void  MotorPack::distribute_config( )
{
	Item*  item = getHead();
	Motor* ptr = NULL;
	while (item)
	{
		ptr = (Motor*)item->getData();
		ptr->set_config_byte( 1, config_bytes[0] );
		ptr->set_config_byte( 2, config_bytes[1] );
		ptr->set_config_byte( 3, config_bytes[2] );
		ptr->set_config_byte( 4, config_bytes[3] );
		item = item->getNext();		
	}
}


// dispatches to all connected motors.
int  MotorPack::distribute_CAN_msg( struct sCAN* mMsg )
{
	Item*  item = getHead();
	Motor* ptr = NULL;
	while (item)
	{
		ptr = (Motor*)item->getData();
		int result = ptr->process_CAN_msg( mMsg );
		if (result) return TRUE;
		
		item = item->getNext();
	}
	return FALSE;
}

void MotorPack::SavePreferences( Preferences* mpref )
{
	// Save name, number of motors
	char str[255];
	sprintf(str, "Limb_Name=%s; Number_of_Motors=%d\n", pack_name, getNumItems() );
	mpref->write( str );

	// Save each motor's preferences:
	Item*  item = getHead();
	Motor* ptr = NULL;
	while (item)
	{
		ptr = (Motor*)item->getData();
		ptr->SavePreferences( mpref );
		item = item->getNext();
	}
}

void MotorPack::LoadPreferences( Preferences* mpref, BOOL mConstructObjects )
{	
	// Read name, number of motors.
	char str[255];
	int  n;

	mpref->readln_nb( str );								// Name
	char** info = mpref->split(str, ';', &n);

	pack_name       = mpref->getString  ( info[0] );		// everything after the '=' sign
	byte num_motors = mpref->getIntValue( info[1] );
	printf("Limb Name=%15s; num_motors=%d\t", pack_name, num_motors );

	if (mConstructObjects) {
		for (int i=0; i<num_motors; i++)
		{
			Motor* tmp = new Motor();
			add_motor( tmp );
		}
	}

	// Read each motor's preferences:
	Item*  item = getHead();
	Motor* ptr = NULL;
	while (item)
	{
		ptr = (Motor*)item->getData();
		ptr->LoadPreferences( mpref );
		item = item->getNext();
	}
	printf("\n");
}

void MotorPack::construct_motors( byte mFirstInstanceNum, byte mNumMotors )
{
	Motor* tmp;
	for (int i=0; i<mNumMotors; i++)
	{
		tmp = new Motor(mFirstInstanceNum+i);
		add_motor( tmp );
	}	
}
 
 
 	
