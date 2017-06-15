#include <math.h>
#include <stdio.h>
#include "joystick_pendant.hpp"
#include <unistd.h>


#ifdef USES_ROBOCLAW
#include "roboclaw.h"
#else
#include "drive_five.h"
#endif


#define USES_FIVE 1

#ifndef USES_FIVE
#define USES_ROBOCLAW 1
#endif


JoystickPendant joy;

/* The way the board wiring harnesses are wired, Left & Right legs are separate 
	roboclaw stacks.  
*/
#ifdef USES_ROBOCLAW
RoboClaw mot ("/dev/ttyACM0", 50000000);
//RoboClaw mot2("/dev/ttyACM1", 10000);
//RoboClaw mot3("/dev/ttyACM2", 10000);		// RIGHT LEG
//RoboClaw mot4("/dev/ttyACM3", 10000);		// RIGHT LEG

void init_robo_drivers()
{
	if (mot.connected == true)		
		 printf("Roboclaw #1 - Available\n"    );
	else printf("Roboclaw #1 - not available\n");
//	if (mot2.connected == true)	
//		 printf("Roboclaw #2 - Available\n"    );
//	else printf("Roboclaw #2 - not available\n");
//	if (mot3.connected == true)	
//		 printf("Roboclaw #3 - Available\n"    );
//	else printf("Roboclaw #3 - not available\n");
//	if (mot4.connected == true)	
//		 printf("Roboclaw #4 - Available\n"	   );
//	else printf("Roboclaw #4 - not available\n");

	bool valid;	// Get Status byte:
	uint16_t error = mot.ReadError(0x80, &valid );	printf("Claw #1 - Status=%4x\n", error );
//	error = mot2.ReadError(0x80, &valid );	printf("Claw #2 - Status=%4x\n", error );
//	error = mot3.ReadError(0x80, &valid );	printf("Claw #3 - Status=%4x\n", error );
//	error = mot4.ReadError(0x80, &valid );	printf("Claw #4 - Status=%4x\n", error );
	
	int16_t current1,current2;
	valid = mot.ReadCurrents (0x80, current1, current2);	printf("Claw #1 - Curr1=%d; Curr2=%d\n", current1, current2 );
//	valid = mot2.ReadCurrents(0x80, current1, current2);	printf("Claw #2 - Curr1=%d; Curr2=%d\n", current1, current2 );
//	valid = mot3.ReadCurrents(0x80, current1, current2);	printf("Claw #3 - Curr1=%d; Curr2=%d\n", current1, current2 );
//	valid = mot4.ReadCurrents(0x80, current1, current2);	printf("Claw #4 - Curr1=%d; Curr2=%d\n", current1, current2 );

	char text[64];
	mot.flush_inbuff();
	printf("VERSION\n");
	bool okay = mot.ReadVersion(0x80,  text );
	if (okay) printf("Valid version read!  %s\n", text);
/*	mot2.ReadVersion(0x80, text );	printf("Claw #2 - %s\n", text );
	mot3.ReadVersion(0x80, text );	printf("Claw #3 - %s\n", text );
	mot4.ReadVersion(0x80, text );	printf("Claw #4 - %s\n", text ); */
	
/*	bool valid = true;
	uint16_t volts1 = mot.ReadMainBatteryVoltage( 0x80, &valid );
	float volt = ((float)volts1)/10.;
	printf("\nMain Battery board #1 = %6.1f volts\n", volt );
	volts1 = mot.ReadLogicBatteryVoltage( 0x80, &valid );
	volt = ((float)volts1)/10.;
	printf("\nLogic Battery board #1 = %6.1f volts\n", volts1 ); */
}
void set_motor_speed(RoboClaw& mMot, int speed1, int speed2 )
{
//	printf(":");	fflush(stdout);
	if (speed1>0)
		mMot.ForwardM1( 0x80, speed1 );
	else 
		mMot.BackwardM1( 0x80, -speed1 );
	
	if (speed2>0)
		mMot.ForwardM2( 0x80, speed2 );			
	else
		mMot.BackwardM2( 0x80, -speed2 );
//	printf(".");	fflush(stdout);
}
#else
DriveFive mot("/dev/ttyUSB0");

void init_robo_drivers()
{
	mot.open		( );
	//mot.set_baud	( );
	
	mot.send_command( (char*)"read configuration");			
	mot.read_response();		// will read and store entire response;  scan for NAK 
	
	mot.send_command( (char*)"use encoders");				mot.read_response();
	mot.send_command( (char*)"read status");				mot.read_response();
	mot.send_command( (char*)"read robot info");			mot.read_response();
	
	mot.send_command( (char*)"pwm v0.5 w0.75 x0.25 y0.5 z0.0");	mot.read_response();	
}
#endif



int main()
{
	printf("main()  Begun!  joy=%p\n", &joy);
	init_robo_drivers();
	
	int speed1 = 0;
	int speed2 = 0;
	int speed3 = 0;
	int speed4 = 0;
	int speed5 = 0;
	while(1) 
	{ 
		speed1 = joy.m_robot_axes_speed[LEFT_LEG_BASE_INDEX ];		// L ankle
		speed2 = joy.m_robot_axes_speed[RIGHT_LEG_BASE_INDEX ];		// R ankle
		//printf("      = %4d,%4d\t", speed1, speed2);	fflush(stdout);

		speed3 = joy.m_robot_axes_speed[LEFT_LEG_BASE_INDEX+1 ];	// L knee
		speed4 = joy.m_robot_axes_speed[RIGHT_LEG_BASE_INDEX+1];	// R knee
		//printf("KNEE  = %4d,%4d\t", speed1, speed2 );	fflush(stdout);
//		set_motor_speed( mot2, speed1, speed2 );

		speed5 = joy.m_robot_axes_speed[LEFT_LEG_BASE_INDEX+2 ];	// L hip
		//speed5 = joy.m_robot_axes_speed[RIGHT_LEG_BASE_INDEX+2];	// R hip
		//printf("HIP  = %4d,%4d\t", speed1, speed2 );	fflush(stdout);
		//set_motor_speed( mot3, speed1, speed2 );

		//speed1 = joy.m_robot_axes_speed[LEFT_LEG_BASE_INDEX+3 ];	// L rotate
		//speed2 = joy.m_robot_axes_speed[RIGHT_LEG_BASE_INDEX+3];	// R rotate
		//printf("ROT  = %4d,%4d\n", speed1, speed2 );
		//set_motor_speed( mot4, speed1, speed2 ); 
#ifdef USES_ROBOCLAW
		// 4 RoboClaws = 8 motors
		set_motor_speed(mot,   speed1, speed2 );
		set_motor_speed(mot2,  speed3, speed4 );
		set_motor_speed(mot3,  speed5, speed6 );		
		set_motor_speed(mot4,  speed7, speed8 );						
#else
		char cmd[256];
		float duty1,duty2,duty3,duty4,duty5;
		duty1 = speed1/127.0;
		duty2 = speed2/127.0;
		duty3 = speed3/127.0;
		duty4 = speed4/127.0;
		duty5 = speed5/127.0;
		sprintf(cmd, "pwm v%1.3f w%1.3f x%1.3f y%1.3f z%1.3f",
			duty1, duty2, duty3, duty4, duty5 );
		printf("%s\n", cmd);
		mot.send_command( (char*)cmd );
		mot.read_response();
		//printf("%s\n", mot.m_response);
#endif
		
	};
}
//		printf("pods   1,2 = %d,%d\t", joy.m_axis_latest[PS3_AXIS_LPOD_UP_DOWN], joy.m_axis_latest[PS3_AXIS_RPOD_UP_DOWN]);		