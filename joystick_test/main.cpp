#include <math.h>
#include <stdio.h>
#include "joystick_pendant.hpp"
#include "roboclaw.h"
#include <unistd.h>


JoystickPendant joy;

/* The way the board wiring harnesses are wired, Left & Right legs are separate 
	roboclaw stacks.  
*/
RoboClaw mot ("/dev/ttyACM0", 1000);
RoboClaw mot2("/dev/ttyACM1", 1000);

RoboClaw mot3("/dev/ttyACM2", 1000);		// RIGHT LEG
RoboClaw mot4("/dev/ttyACM3", 1000);		// RIGHT LEG


void set_motor_speed(RoboClaw& mMot, int speed1, int speed2 )
{
	printf(":");	fflush(stdout);
	if (speed1>0)
		mMot.ForwardM1( 0x80, speed1 );
	else 
		mMot.BackwardM1( 0x80, -speed1 );
	
	if (speed2>0)
		mMot.ForwardM2( 0x80, speed2 );			
	else
		mMot.BackwardM2( 0x80, -speed2 );
	printf(".");	fflush(stdout);
}

int main()
{
	printf("main()  Begun!  joy=%p\n", &joy);
	
	int speed1 = 0;
	int speed2 = 0;
	while(1) 
	{ 
		speed1 = joy.m_robot_axes_speed[LEFT_LEG_BASE_INDEX ];		// L ankle
		speed2 = joy.m_robot_axes_speed[RIGHT_LEG_BASE_INDEX ];		// R ankle
		printf("      = %4d,%4d\t", speed1, speed2);	fflush(stdout);
		set_motor_speed(mot,  speed1, speed2 );

		speed1 = joy.m_robot_axes_speed[LEFT_LEG_BASE_INDEX+1 ];	// L knee
		speed2 = joy.m_robot_axes_speed[RIGHT_LEG_BASE_INDEX+1];	// R knee
		printf("KNEE  = %4d,%4d\t", speed1, speed2 );	fflush(stdout);
		set_motor_speed( mot2, speed1, speed2 );

		speed1 = joy.m_robot_axes_speed[LEFT_LEG_BASE_INDEX+2 ];	// L hip
		speed2 = joy.m_robot_axes_speed[RIGHT_LEG_BASE_INDEX+2];	// R hip
		printf("HIP  = %4d,%4d\t", speed1, speed2 );	fflush(stdout);
		//set_motor_speed( mot3, speed1, speed2 );

		speed1 = joy.m_robot_axes_speed[LEFT_LEG_BASE_INDEX+3 ];	// L rotate
		speed2 = joy.m_robot_axes_speed[RIGHT_LEG_BASE_INDEX+3];	// R rotate
		printf("ROT  = %4d,%4d\n", speed1, speed2 );
		//set_motor_speed( mot4, speed1, speed2 ); 

	};
}
//		printf("pods   1,2 = %d,%d\t", joy.m_axis_latest[PS3_AXIS_LPOD_UP_DOWN], joy.m_axis_latest[PS3_AXIS_RPOD_UP_DOWN]);		