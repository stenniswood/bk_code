#include <math.h>
#include <stdio.h>
#include "joystick_pendant.hpp"
#include <unistd.h>
#include <termios.h>

#ifdef USES_ROBOCLAW
#include "roboclaw.h"
#else
#include "drive_five.h"
#endif

#include "sequencer_memory.hpp"
#include "pan_tilt.hpp"

#include <unistd.h>


#define USES_FIVE 1

#ifndef USES_FIVE
#define USES_ROBOCLAW 1
#endif

/*********************************************
Reads in from /dev/js0
outputs to /dev/ttyUSB0  drive five board

Joystick events are read and processed on it's own thread.

This test app will not use shared memory!
**********************************************/


//JoystickPendant joy;
Joystick joy;

DriveFive mot("/dev/ttyUSB0");
PanTilt pant("/dev/ttyACM0");

char use_encoder_msg[] = "use encoders v w x y z";
char read_status_msg[] = "read status";
void init_pan_tilt()
{
	pant.open();
	sleep(0.2);
	pant.set_baud (B38400);
	sleep(0.2);
	if (pant.available() )
	{
		printf("Data was found in Pan Tilt receive buffer.\n");
		
		pant.read_response();		// Need to clear read buffers - from introductory welcome text.
		pant.print_response();
		// Skip over help if its in that state (2 returns needed).		
	}
	
}
void init_robo_drivers()
{
	mot.open		( );
	sleep(0.2);
	mot.set_baud (B38400);
	sleep(0.2);
	
	if (mot.available() )
	{
		printf("Data was found in DriveFive receive buffer.\n");
		
		mot.read_response();		// Need to clear read buffers - from introductory welcome text.
		mot.print_response();
		// Skip over help if its in that state (2 returns needed).		
	}

	mot.send_command( use_encoder_msg );				
	mot.read_response();		// Need to clear read buffers 
	mot.send_command( read_status_msg );				
	mot.read_response();		// Need to clear read buffers 

//	mot.set_baud	  ( );
//	mot.send_command. ( (char*)"read configuration");			
//	mot.read_response ( );		// will read and store entire response;  scan for NAK
//	mot.send_command. ( (char*)"read robot info");			mot.read_response();
//	mot.send_command. ( (char*)"pwm v0.5 w0.75 x0.25 y0.5 z0.0");	mot.read_response();	
}


#include <stdlib.h>

int Control_Mode = 0;
#define MAX_MODES 2

void play_sound_mode()
{
printf("play_sound_mode()\n");
	switch (Control_Mode)
	{
	case 0 : system("aplay ./pluck_1beep.wav");		break;
	case 1 : system("aplay ./pluck_2beeps.wav");	break;
	case 2 : system("aplay ./pluck_3beeps.wav");	break;
	default : system("aplay ./Laser_Blast.wav");	break;
	}
}

void toggle_mode( )
{
	Control_Mode++;
	if (Control_Mode>MAX_MODES)
		Control_Mode = 0;
}
void control(float mValue, float mMin, float mMax )
{
	if (mValue>mMax)	mValue = mMax;
	if (mValue>mMin)	mValue = mMin;
}
int main()
{
	play_sound_mode();
	
	printf("main()  Begun!  joy=%p\n", &joy);
	init_robo_drivers();
	init_pan_tilt();
	
	int speed1 = 0;
	int speed2 = 0;
	int speed3 = 0;
	int speed4 = 0;
	int speed5 = 0;
	int speed6 = 0;
	printf("Entering while loop\n");

	char  cam_cmd[50];
	float Cam_LR_Angle = 90.0;
	float Cam_UD_Angle = 90.0;

	while(1) 
	{ 
		// DRIVE FIVE BOARD
		char cmd[256];
		float duty1,duty2,duty3,duty4,duty5;

		if (joy.m_buttons_latest[4]) {
			duty3 = ((float)joy.m_axis_latest[0]/32767); 
			duty4 = ((float)joy.m_axis_latest[2]/32767); 
		} else {
			duty1 = ((float)joy.m_axis_latest[1])/32767; 
			duty2 = ((float)joy.m_axis_latest[5])/32767; 
		}

		// SQUARE CHANGES THE MODE:		
		if ((joy.m_buttons_previous[PS_SQUARE]==1) && (joy.m_buttons_latest[PS_SQUARE]==0))
		{
			joy.m_buttons_previous[PS_SQUARE] = 0;
			toggle_mode();
			play_sound_mode();
		}

		// LEFT & RIGHT ARROW BUTTONS MOVE THE CAMERA : 
		if ((joy.m_buttons_latest[PS_LEFT_BOTTOM]==1))
		{
			//joy.m_buttons_latest[PS_LEFT_BOTTOM] = 0;
			Cam_LR_Angle += 4.0;
		}
		if ((joy.m_buttons_latest[PS_RIGHT_BOTTOM]==1))
		{
			//joy.m_buttons_latest[PS_RIGHT_BOTTOM] = 0;
			Cam_LR_Angle -= 4.0;
		}
		control( Cam_LR_Angle,  10,  180 );
		sprintf(cam_cmd,"servo 1 %6.1f", Cam_LR_Angle );	printf("%s\n", cam_cmd);
		pant.send_command( cam_cmd );

		// UP & DOWN ARROW BUTTONS MOVE THE CAMERA : 
		if ((joy.m_buttons_latest[PS_TRIANGLE]==1))
		{
			//joy.m_buttons_previous[PS_TRIANGLE] = 0;
			Cam_UD_Angle += 4.0;
		}
		if ((joy.m_buttons_latest[PS_X]==1))
		{
			//joy.m_buttons_previous[PS_X] = 0;
			Cam_UD_Angle -= 4.0;
		}
		control( Cam_UD_Angle,  10,  180 );		
		sprintf(cam_cmd,"servo 2 %6.1f", Cam_UD_Angle );	printf("%s\n", cam_cmd);
		pant.send_command( cam_cmd );
			
		sprintf(cmd, "pwm v%1.3f w%1.3f x%1.3f y%1.3f z%1.3f",
						  duty1, duty2, duty3, duty4, duty5 );
		mot.send_command( (char*)cmd );
		mot.read_response();
		usleep(1000);		
	};
}


