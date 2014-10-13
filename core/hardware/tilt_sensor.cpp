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
#include "robot.hpp"


TiltSensor::TiltSensor( )
{
}

// Accessors:
float*	TiltSensor::getOrientation() 
{
 return NULL; 
}
float*	TiltSensor::getPosition()	 
{
 return NULL; 
}

float*	TiltSensor::getAngularVelocity() 
{
 return NULL; 
}
float*	TiltSensor::getLinearVelocity()  
{
 return NULL; 
}


// Sequencer:
// none	

// RECEIVE CAN MESSAGE : 
int  TiltSensor::process_CAN_msg	  ( struct sCAN* mMsg )
{}
byte TiltSensor::can_parse_motor_angle( struct sCAN* mMsg )
{}
byte TiltSensor::can_parse_motor_value( struct sCAN* mMsg )
{}

// Save/Load to/from a .ini configuration file on the Raspberry Pi.
void	TiltSensor::SavePreferences		( FILE* mfd		)
{
}
		
void	TiltSensor::LoadPreferences		( FILE* mfd		)
{
}		// sensitivity, last known position

