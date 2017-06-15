#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>
#include "joystick.hpp"



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
	printf("eventThread(): Joystick object=%p\t", joy);
	sprintf( dev_name, "/dev/input/js0" );
	joy->joystick_fd = open( dev_name, O_RDONLY );
	
	if (joy->joystick_fd >= 0) {
		printf( "Opened Joystick:  %s\n", dev_name );
	} else {
		printf("Joystick not available!\n");
		fprintf(stderr, "Error opening Joystick!\n");
		quitState = 1;
		return &quitState;
	}
	
	while (1) {
		joy->read_event();
		joy->handle_event();
		//joy.print_event_info();
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
	
	// CREATE A THREAD TO READ FROM DEVICE:
	int result = pthread_create( &inputThread_id, NULL, &eventThread, this );
	if (result != 0) {
		if (Debug) fprintf(stderr, "Unable to initialize the Joystick\n");
	}
} 	

std::string get_button_name( int mButtonIndex )
{
	switch (mButtonIndex)
	{
	case PS3_SELECT 		: return "SELECT";
	case PS3_LEFT_JOY_PUSH 	: return "LEFT_JOY_PUSH";
	case PS3_RIGHT_JOY_PUSH : return "RIGHT_JOY_PUSH";
	case PS3_START 			: return "START";
	case PS3_UP 			: return "UP";
	case PS3_RIGHT 			: return "RIGHT";
	case PS3_DOWN 			: return "DOWN";
	case PS3_LEFT 			: return "LEFT";
	case PS3_LEFT_TOP 		: return "LEFT_TOP";
	case PS3_RIGHT_BOTTOM 	: return "RIGHT_BOTTOM";
	case PS3_LEFT_BOTTOM 	: return "LEFT_BOTTOM";
	case PS3_RIGHT_TOP 		: return "RIGHT_TOP";
	case PS3_TRIANGLE 		: return "TRIANGLE";
	case PS3_CIRCLE 		: return "CIRCLE";
	case PS3_X 				: return "X";
	case PS3_SQUARE 		: return "SQUARE";
	default: return "Unknown";
	};
}

std::string get_axis_name( int mAxisIndex )
{
	switch (mAxisIndex)
	{
	case PS3_AXIS_LPOD_LEFT_RIGHT : return "LEFT - LR";
	case PS3_AXIS_LPOD_UP_DOWN    : return "LEFT - UD";
	case PS3_AXIS_RPOD_LEFT_RIGHT : return "RIGHT - LR";
	case PS3_AXIS_RPOD_UP_DOWN 	  : return "RIGHT - UD";
	default: return "unknown"; break;
	}
}

void Joystick::print_event_info()
{
	if (Debug==0) return;

	if (m_ev.type==1) {
		//printf("time=%lu; Button #%d = %d\n", m_ev.time, m_ev.number, m_ev.value );
		std::string button_name = get_button_name( m_ev.number );
		if (m_ev.value==1)
			printf("time=%lu; Button %d %s down\n", m_ev.time, m_ev.number, button_name.c_str() );
		else 
			printf("time=%lu; Button %d %s up\n", m_ev.time, m_ev.number, button_name.c_str() );
	}
	else if (m_ev.type==2) {
		if (m_ev.number==25)
				return;
				
		std::string axis_name = get_axis_name( m_ev.number );
		printf("time=%lu; Axis   %d %s = %d\n", m_ev.time, m_ev.number, axis_name.c_str(), m_ev.value );	
	} else 
		printf("time=%lu; value=%d; type=%x; number=%d\n", m_ev.time, m_ev.value, m_ev.type, m_ev.number );	
}
void Joystick::read_event()
{
	//joy.m_prev_ev_time = joy.Joystick_ev.time;
	read( joystick_fd, &m_ev, sizeof(m_ev) );	
	
}
void Joystick::handle_event()
{		
	if ((m_ev.type==JS_EVENT_AXIS) && (m_ev.number<4))
	{	
		m_axis_latest[m_ev.number] = m_ev.value;
	}
	if ((m_ev.type==JS_EVENT_BUTTON) && (m_ev.number<MAX_BUTTONS))
	{
		m_buttons_latest[m_ev.number] = (m_ev.value==1);
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

