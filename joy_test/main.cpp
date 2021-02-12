#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>

#include "joystick.hpp"


Joystick joy;

/*********************************************
Reads in from /dev/js0
outputs to /dev/ttyUSB0  drive five board

Joystick events are read and processed on it's own thread.

This test app will not use shared memory!
**********************************************/
int     Control_Mode = 0;
#define MAX_MODES 2

void play_sound( int mIndex )
{
	//printf("play_sound_mode()\n");
	switch (mIndex)
	{
	case 0 : system("aplay ./pluck_1beep.wav");		break;
	case 1 : system("aplay ./pluck_2.wav");			break;
	case 2 : system("aplay ./_3beeps.wav");			break;
	case 3 : system("aplay ./Laser_Blast.wav");		break;
	case 4 : system("aplay ./Sci_Fi_robot.wav");	break;

	case 5 : system("aplay ./discord-sounds.wav");	break;                   	
	case 6 : system("aplay ./discord-notification.wav");	break;                   
	case 7 : system("aplay ./drums_clap_wide_bright.wav");	break;                 
	case 8 : system("aplay ./lightsaber_02.wav");			break;                          
	case 9 : system("aplay ./sonic_ring_sound.wav");		break;                       
	case 10 : system("aplay ./minecraft-villager-sound-effect.wav");	break;        
	case 11 : system("aplay ./erro.wav");					break;                                 
	
	case 12 : system("aplay ./Clapping_1.wav");				break;          
	case 13 : system("aplay ./Clapping_2.wav");				break;                             
	case 14 : system("aplay ./Clapping_faster.wav");		break;                        
	case 15 : system("aplay ./applause.wav");				break;                               
	case 16 : system("aplay ./final-fantasy-v-music-victory-fanfare.wav");	break;  
	case 17 : system("aplay ./jennie.wav");					break;                                 
	case 18 : system("aplay ./landofcotton.wav");			break;                           
	case 19 : system("aplay ./landslide.wav");				break;           
	case 20 : system("aplay ./miniDSP_wave1.wav");			break;                          
	
	default : system("aplay ./Laser_Blast.wav");		break;
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
int main(int argc, char** argv)
{	
	if (argc==1)
		strcpy (dev_name, "js0");
	else {
		printf("argv[1]=%s\n", argv[1]);
		strcpy( dev_name, argv[1] );
	}

	printf("main(%d)  Begun!  joy=%p\n", argc, &joy);	
	printf("Entering while loop\n");
	while(1) 
	{
		if (joy.new_event_available())
		{
			if (joy.is_button())
			{
				//play_sound( joy.get_button_number() );
			}
			joy.mark_event_acknowledged();		
		}
	
//		sprintf(cam_cmd,"servo 2 %6.1f", Cam_UD_Angle );	
//		printf("%s\n", cam_cmd);	
		usleep(1000);		
	};
}


