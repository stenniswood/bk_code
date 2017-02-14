#ifndef _JOYSTICK_
#define _JOYSTICK_

#include <string>
#include <vector>
#include <linux/input.h>


// The thread puts this into left, middle, right.
// The timeslice will clear it.
#define UNHANDLED_EVENT 0x80

#define LEFT_BUTTON_DOWN  0x10
#define RIGHT_BUTTON_DOWN 0x01


// Mouse state structure
struct js_event {
		__u32 time;     /* event timestamp in milliseconds */
		__s16 value;    /* value */
		__u8 type;      /* event type */
		__u8 number;    /* axis/button number */
	};
	
#define JS_EVENT_BUTTON         0x01    /* button pressed/released */
#define JS_EVENT_AXIS           0x02    /* joystick moved */
#define JS_EVENT_INIT           0x80    /* initial state of device */


//PS3 BUTTON DEFINITIONS:
#define PS3_SELECT 0
#define PS3_LEFT_JOY_PUSH 1
#define PS3_RIGHT_JOY_PUSH 2
#define PS3_START 3
#define PS3_UP 4
#define PS3_RIGHT 5
#define PS3_DOWN 6
#define PS3_LEFT 7
#define PS3_LEFT_BOTTOM 8
#define PS3_RIGHT_BOTTOM 9
#define PS3_LEFT_TOP 10
#define PS3_RIGHT_TOP 11
#define PS3_TRIANGLE 12
#define PS3_CIRCLE 13
#define PS3_X 14
#define PS3_SQUARE 15

// PS3 AXIS PODS:
#define PS3_AXIS_LPOD_LEFT_RIGHT 0
#define PS3_AXIS_LPOD_UP_DOWN    1
#define PS3_AXIS_RPOD_LEFT_RIGHT 2
#define PS3_AXIS_RPOD_UP_DOWN 	 3

std::string get_button_name( int mButtonIndex );
std::string get_axis_name( int mAxisIndex );

#define MAX_BUTTONS 40

class Joystick 
{
public:
	Joystick ();	
	~Joystick();
	
	void	Initialize(); 					// int mouse_init()
	void 	read_event();
	void	print_event_info();	
	void 	close();

	virtual void	handle_event();
	virtual int		time_slice	();

	// VARIABLES:
	short				m_axis_latest[4];
	bool				m_buttons_latest[MAX_BUTTONS];	
	struct js_event		m_ev;
	int					joystick_fd;
};


#endif
