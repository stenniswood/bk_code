#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>
#include "joystick.hpp"

void help_mapping() 
{
	printf(" **** PS4 CONTROLLER MAPPING ****");
	printf(" **** PS4 CONTROLLER MAPPING ****");
	printf(" **** PS4 CONTROLLER MAPPING ****");	
}

int 		quitState  = 0;
#define    	CUR_SIZ  16				// cursor size, pixels beyond centre dot

#define Debug 1   
pthread_t inputThread_id;
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define Dprintf if (Debug) printf("%s ", __FILENAME__); if (Debug) printf

signed char mBuff[4];
char        dev_name[40];



	
// eventThread reads from the Joystick input file
void* eventThread(void *arg) 
{
	Joystick* joy = (Joystick*)arg;
	
	// Open Joystick driver
	char path_name[40] = "/dev/input/";
	strcat( path_name, dev_name );
	
	//sprintf( path_name, "/dev/input/js0" );
	printf("opening Joystick:  %s\n", path_name );
	joy->joystick_fd = open( path_name, O_RDWR );	// O_RDONLY 
	
	if (joy->joystick_fd >= 0) {
		printf( "Opened Joystick:  %s\n", path_name );
		system("aplay ./final-fantasy-v-music-victory-fanfare.wav");
	} else {
		system("aplay ./erro.wav");
		perror( "Joystick not available!");
		printf("This app uses the Linux joydev API. ");
		printf("To use a PS4 controller, you need to install and run ds4drv ");
		printf("See:  https://github.com/chrippa/ds4drv  \n\n");
		quitState = 1;
		return &quitState;
	}
	
	while (1) {
		joy->read_event();
		joy->handle_event();
		joy->print_event_info();
	}
}

Joystick::Joystick()
{
	//Dprintf("Joystick::Joystick()\n");
	Initialize();
}
Joystick::~Joystick()
{
}

void Joystick::Initialize()
{ 
	Dprintf("Joystick::Initialize() ...\n");
	for (int i=0; i<MAX_BUTTONS; i++)
		m_buttons_previous[i] = 0;
	for (int i=0; i<MAX_BUTTONS; i++)
		m_buttons_latest[i] = 0;

	// CREATE A THREAD TO READ FROM DEVICE : 
	for (int retries=0; retries<3; retries++) 
	{	
		int result = pthread_create( &inputThread_id, NULL, &eventThread, this );
		if (result != 0) {
			if (Debug) fprintf(stderr, "Unable to initialize the Joystick\n");
			sleep(1);
			printf("Retrying... connection to joystick.");
			continue;
		}
		break;
	}
} 	

#ifdef PS4
std::string get_button_name( int mButtonIndex )
{
	switch (mButtonIndex)
	{
	case PS_PAD_PUSH 		: return "TOUCH PAD PUSH";	
	case PS_BUTTON 			: return "PS";
	case PS_LEFT_JOY_PUSH 	: return "LEFT_JOY_PUSH";
	case PS_RIGHT_JOY_PUSH 	: return "RIGHT_JOY_PUSH";
	case PS_SHARE 			: return "SHARE";
	case PS_OPTIONS 		: return "OPTIONS";
	case PS_LEFT_TOP 		: return "L1";
	case PS_RIGHT_BOTTOM 	: return "R2";
	case PS_LEFT_BOTTOM 	: return "L2";
	case PS_RIGHT_TOP 		: return "R1";
	
	case PS_UP 	    		: return "UP";
	case PS_DOWN 			: return "DOWN";
	case PS_LEFT 			: return "LEFT";
	case PS_RIGHT 			: return "RIGHT";

	case PS_TRIANGLE 		: return "TRIANGLE";
	case PS_CIRCLE 			: return "CIRCLE";
	case PS_X 				: return "X";
	case PS_SQUARE 			: return "SQUARE";
	default: return "Unknown";
	};
}
#else
std::string get_button_name( int mButtonIndex )
{
	switch (mButtonIndex)
	{
	case PS_SELECT 			: return "SELECT";
	case PS_LEFT_JOY_PUSH 	: return "LEFT_JOY_PUSH";
	case PS_RIGHT_JOY_PUSH 	: return "RIGHT_JOY_PUSH";
	case PS_START 			: return "START";
	case PS_UP 				: return "UP";
	case PS_RIGHT 			: return "RIGHT";
	case PS_DOWN 			: return "DOWN";
	case PS_LEFT 			: return "LEFT";
	case PS_LEFT_TOP 		: return "L1";
	case PS_RIGHT_BOTTOM 	: return "R2";
	case PS_LEFT_BOTTOM 	: return "L2"; 
	case PS_RIGHT_TOP 		: return "R1";
	case PS_TRIANGLE 		: return "TRIANGLE";
	case PS_CIRCLE 		: return "CIRCLE";
	case PS_X 				: return "X";
	case PS_SQUARE 		: return "SQUARE";
	default: return "Unknown";
	};
}
#endif



std::string get_axis_name( int mAxisIndex )
{
	switch (mAxisIndex)
	{
	case PS_AXIS_LPOD_LEFT_RIGHT : return "LEFT - LR";
	case PS_AXIS_LPOD_UP_DOWN    : return "LEFT - UD";
	case PS_AXIS_RPOD_LEFT_RIGHT : return "RIGHT - LR";
	case PS_AXIS_RPOD_UP_DOWN 	  : return "RIGHT - UD";
	case 6 	  : return "TILT - ROLL";
	case 7 	  : return "TILT - PITCH";	
	case 8 	  : return "TILT - YAW";		
	
	default: return "unknown"; break;
	}
}

void Joystick::print_event_info()
{
	if (Debug==0) return;

	if (m_ev.type==JS_EVENT_BUTTON) 
	{
		//printf("time=%lu; Button #%d = %d\n", m_ev.time, m_ev.number, m_ev.value );
		std::string button_name = get_ps4_button_name( m_ev.number );
		if (m_ev.value==1)
			printf("time=%lu; BUTTON %d %s down\n", m_ev.time, m_ev.number, button_name.c_str() );
		else 
			printf("time=%lu; BUTTON %d %s up\n", m_ev.time, m_ev.number, button_name.c_str() );
	}
	else if (m_ev.type==JS_EVENT_AXIS) 
	{
		//if (m_ev.number==25)
		//		return;

		// PS4 pods use (0 & 1) and right (2 & 5) 
				
		// Only Print 0,1,2,5 which are the anlog pod positions.  Not the tilt angles.
		if ((m_ev.number==PS_AXIS_LPOD_LEFT_RIGHT)  || (m_ev.number==PS_AXIS_LPOD_UP_DOWN) ||
		    (m_ev.number==PS_AXIS_RPOD_LEFT_RIGHT) || (m_ev.number==PS_AXIS_RPOD_UP_DOWN) )		
		{
			std::string axis_name = get_axis_name( m_ev.number );
			printf("\ttime=%lu; Axis   %d %s = %d\n", m_ev.time, m_ev.number, axis_name.c_str(), m_ev.value );	
		}
		else 
			printf("\ttime=%lu; Axis   %d = %d\n", m_ev.time, m_ev.number,  m_ev.value );			
	} else if (m_ev.type & JS_EVENT_INIT) 
	{
		char str[10];
		sprintf(str,"unknown");
		if ((m_ev.type&0x7F)==JS_EVENT_AXIS)
			sprintf( str, "AXIS ");
		else if ((m_ev.type&0x7F)==JS_EVENT_BUTTON)
			sprintf (str, "Button");
		printf("time=%lu; INIT %s; number=%d; value=%8x; \n", m_ev.time, str, m_ev.number, m_ev.value );	
	} else 
		printf("time=%lu; type=0x%2x; number=%d; value=%8x; \n", m_ev.time, m_ev.type, m_ev.number, m_ev.value );	
}

void Joystick::read_event()
{
	//joy.m_prev_ev_time = joy.Joystick_ev.time;
	read( joystick_fd, &m_ev, sizeof(m_ev) );
}

void  Joystick::mark_event_acknowledged()
{
	m_new_axis = false;
	m_new_button = false;
	m_new_event = true;
}
	
extern void play_sound( int mIndex );

void Joystick::handle_event()
{
	//if (m_ev.type!=JS_EVENT_AXIS)
	if ((m_ev.number==103) || (m_ev.number==108))
		printf("Joystick::handle_event() - type=%d; num=%d; val=%d\n", m_ev.type, m_ev.number, m_ev.value );

	m_new_event = true;
	
	//printf("Joystick::handle_event()\n");		
	if ((m_ev.type==JS_EVENT_AXIS) && (m_ev.number<MAX_AXIS))
	{	
		m_axis_latest[m_ev.number] = m_ev.value;
	}
	if ((m_ev.type==JS_EVENT_BUTTON) && (m_ev.number<MAX_BUTTONS))
	{
		m_new_button = true;
		m_button_ev  = m_ev;

		m_buttons_previous[m_ev.number] = m_buttons_latest[m_ev.number];
		m_buttons_latest[m_ev.number]   = (m_ev.value==1);
		//printf("  BUTTON=%d VALUE=%d  \n", m_ev.number, m_ev.value );
		// m_buttons_previous[m_ev.number], m_buttons_latest[m_ev.number] );
	}
}

int	Joystick::time_slice()
{
/*	if (left & UNHANDLED_EVENT)
	{
		left &= (~UNHANDLED_EVENT);	// clear the flag.
		int retval = left;
		return (retval<<4);			// 1=> button down.  0=> button up
	}
	if (right & UNHANDLED_EVENT)
	{
		right &= (~UNHANDLED_EVENT);	// clear the flag.
		return ((right)<<4);			// 1=> button down.  0=> button up
	}*/
	return 0;
} 

void Joystick::close()
{ 
	::close(joystick_fd);
}

