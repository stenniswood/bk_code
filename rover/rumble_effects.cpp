#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <vector>

#include "rumble_effects.hpp"


std::vector<struct stEffect> Effects;
bool running_effect = false;
int  selected_effect = 0;
int  envelope_index  = 0;
int  ms_delay 		= 50;

pthread_t  effectsThread_id = 0;

// eventThread reads from the JoystickRaw input file
void* effectsThread( void *arg )
{
	JoystickRaw* joy = (JoystickRaw*)arg;

	while (1) 
	{
		struct stEffect *ePtr = &(Effects[selected_effect]);
		
		if (running_effect)
		{
			byte strong = ePtr->strongs[envelope_index];
			byte weak   = ePtr->weaks  [envelope_index];

			set_LED_color( joy->joystick_fd, strong, weak, 0x02 );	 
			
			send_Rumble( joy->joystick_fd, strong, weak );

			usleep( ePtr->ms_delay * 1000. );
			envelope_index++;
			if (envelope_index >= ePtr->strongs.size() )
			{
				envelope_index = 0;
			}
		}
		else 
			send_Rumble( joy->joystick_fd, 0, 0 );
	}
	return NULL;
}

/*int load_envelope ( int mEffectId, std::vector<byte> mEnvelope )
{
	Effects.push_back( mEnvelope );
	return Effects.size()-1;
}*/

void start_effects ()
{
	running_effect = true;
}

void stop_effects  ()
{
	running_effect = false;
}

void select_effect( int mEffectIndex )
{
	selected_effect = mEffectIndex;	
}

void create_saw_effect()
{
	struct stEffect tmpE;
	
	tmpE.strongs.push_back( 50 );
	tmpE.weaks.push_back  ( 50 );
	tmpE.strongs.push_back( 100 );
	tmpE.weaks.push_back  ( 100 );
	tmpE.strongs.push_back( 150 );
	tmpE.weaks.push_back  ( 150 );
	tmpE.strongs.push_back( 200 );
	tmpE.weaks.push_back  ( 200 );
	tmpE.strongs.push_back( 255 );
	tmpE.weaks.push_back  ( 255 );

	tmpE.strongs.push_back( 200 );
	tmpE.weaks.push_back  ( 200 );
	tmpE.strongs.push_back( 150 );
	tmpE.weaks.push_back  ( 150 );
	tmpE.strongs.push_back( 100 );
	tmpE.weaks.push_back  ( 100 );
	tmpE.strongs.push_back( 50  );
	tmpE.weaks.push_back  ( 50  );
	tmpE.strongs.push_back( 0   );
	tmpE.weaks.push_back  ( 0   );
	tmpE.ms_delay	      = 100;
	Effects.push_back ( tmpE );
}

void create_pulsed_effect()
{
	struct stEffect tmpE;
	
	tmpE.strongs.push_back( 255 );
	tmpE.weaks.push_back  ( 255 );
	tmpE.strongs.push_back( 0 );
	tmpE.weaks.push_back  ( 0 );
	tmpE.ms_delay	= 300;
	Effects.push_back ( tmpE );	
}

extern JoystickRaw joy;

void init_rumble_effects()
{
	create_saw_effect	( );
	create_pulsed_effect( );
	select_effect		( 0 );
	
	int result = pthread_create( &effectsThread_id, NULL, &effectsThread, &joy );
	if (result != 0) 
	{
		printf( "Unable to create the effects thread.\n" );
		exit(1);
	}	
	start_effects( );
}

