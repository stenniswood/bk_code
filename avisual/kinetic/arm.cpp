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
#include "arm.hpp"


Arm::Arm( char* mName )
: MotorPack(mName)
{
}


void Arm::SavePreferences( Preferences* mpref		)
{
	char str[255];
	sprintf(str, "\nLimb_type=Arm\n" );					
	mpref->write( str );

	MotorPack::SavePreferences( mpref );	
}

void Arm::LoadPreferences( Preferences* mpref, BOOL mConstructObjects )
{
	//char str[255];
	//mpref->readln_nb( str );

	MotorPack::LoadPreferences( mpref );		
}

OneVector* Arm::calculate_duties( OneVector* positionVector )
{
	return NULL;
}

