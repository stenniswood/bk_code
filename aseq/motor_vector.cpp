/*  ***MotorPack is an equivalent cpp class***

	We may choose to setup several vectors for the boards.
	ie. LeftLeg, Right Leg, LeftArm, Right Arm

	how many vectors and how many boards in each depends on the application.
	So how can we make this dynamically changeable?
	
	Dimension a vector as in Matlab.   
			vec4 = motorVect(5);
	Then assign board instance numbers
	
	Each vector then consists of only an Array of Instance numbers.		*/
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <string>

#include "bk_system_defs.h"
#include "can_eid.h"
#include "CAN_Interface.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "packer_motor.h"

#include "motor.hpp"
#include "Appendage.hpp"
#include "vector_file.h"
#include "robot.hpp"

#include "motor_vector.h"
#include "can_txbuff.h"


#include "config_file.h"


#define float_size 4





