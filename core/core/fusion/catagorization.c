/*
Purpose :  To catagorize position such as :
		  "on back", "face down", "standing", "upside down"
		  "reclined", "leaning forward", etc

	1 of 6:  Gives most domination axis direction (+-)
	
	Given a vector (x,y,z) accelerometer without linear acceleration.	
*/
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "pican_defines.h"
#include "CAN_Interface.h"
#include "packer_lcd.h"
#include "can_txbuff.h"
#include "packer.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "catagorization.h"


/*
	
	Return : 1=>X;  2=>Y; 3=>Z is largest magnitude
*/
byte get_dominant_axis(struct fXYZ* mGravityVector)
{
	float x = fabs(mGravityVector->x);
	float y = fabs(mGravityVector->y);
	float z = fabs(mGravityVector->z);
	
	if (x > y)
	{
		if (x > z)
			return 1;	// X dominant
		else
			return 3;	// Z dominant
	} else {
		if (y > z)
			return 2;	// Y dominant
		else
			return 3;	// Z dominant
	}
	return -1;
}



enum Catagory get_one_of_six(struct fXYZ* mGravityVector)
{
	byte da = get_dominant_axis(mGravityVector);
	switch(da)
	{
	// The order of these may need correcting.  Depends on 
	// the part's orientation on the PCB, AND
	// the boards orientation in the robot.
	case 1 :  if (mGravityVector->x > 0)
				return OnLeftSide;
			  else
			  	return OnRightSide; 					
			break;
	case 2 : if (mGravityVector->y > 0)
				return OnBack;
			  else
			  	return FaceDown; 					
			break;
	case 3 : if (mGravityVector->z > 0)
				return Standing;
			  else
			  	return UpsideDown; 					
			break;
	default:	return Standing; break;
	}
}


void classify_timeslice_100ms()
{

//	get_one_of_six( &global_orientation);	
}
