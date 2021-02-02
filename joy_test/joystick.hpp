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


#define PS4 1

//PS3 BUTTON DEFINITIONS:
#ifdef PS4
#define PS_PAD_PUSH			17 				
#define PS_BUTTON			16			
#define PS_RIGHT_JOY_PUSH 	11
#define PS_LEFT_JOY_PUSH 	10
#define PS_OPTIONS			9
#define PS_SHARE 			8
#define PS_RIGHT_BOTTOM 	7		// R2
#define PS_LEFT_BOTTOM 		6		// L2
#define PS_RIGHT_TOP 		5		// R1
#define PS_LEFT_TOP 		4		// L1
#define PS_TRIANGLE 		3		
#define PS_CIRCLE 			2
#define PS_X 				1
#define PS_SQUARE 			0
#define PS_UP 		12
#define PS_DOWN 	13
#define PS_LEFT 	14
#define PS_RIGHT 	15
#else 		// PS3
#define PS_SELECT 0
#define PS_LEFT_JOY_PUSH 1
#define PS_RIGHT_JOY_PUSH 2
#define PS_START 3
#define PS_UP 4
#define PS_RIGHT 5
#define PS_DOWN 6
#define PS_LEFT 7
#define PS_LEFT_BOTTOM 8
#define PS_RIGHT_BOTTOM 9
#define PS_LEFT_TOP 10
#define PS_RIGHT_TOP 11
#define PS_TRIANGLE 12
#define PS_CIRCLE 13
#define PS_X 14
#define PS_SQUARE 15
#endif

// PS3 AXIS PODS:

#ifdef PS4
#define PS_AXIS_LPOD_LEFT_RIGHT 0
#define PS_AXIS_LPOD_UP_DOWN    1
#define PS_AXIS_RPOD_LEFT_RIGHT 2
#define PS_AXIS_RPOD_UP_DOWN 	 5
#define PS_AXIS_TILT_LEFT_RIGHT 6
#define PS_AXIS_TILT_FORE_AFT 	 7
#else
#define PS_AXIS_LPOD_LEFT_RIGHT 0
#define PS_AXIS_LPOD_UP_DOWN    1
#define PS_AXIS_RPOD_LEFT_RIGHT 2
#define PS_AXIS_RPOD_UP_DOWN 	 3
#endif

std::string get_button_name( int mButtonIndex );
std::string get_axis_name  ( int mAxisIndex   );

#define MAX_BUTTONS 40
#define MAX_AXIS   10

extern char        dev_name[40];

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
	void			mark_event_acknowledged();

	bool			new_event_available()  { return (m_new_event); };
	bool			is_button		 ()  { return (m_ev.type == JS_EVENT_BUTTON); };
	int				get_button_number()  { return (m_button_ev.number); 		  };
	std::string		get_button_name  ()  { return ::get_button_name(m_ev.number); };

		
	// VARIABLES:
	bool				m_new_axis;
	bool				m_new_button;
	bool				m_new_event;
	
	short				m_axis_latest[MAX_AXIS];
	bool				m_buttons_latest[MAX_BUTTONS];	
	bool				m_buttons_previous[MAX_BUTTONS];	
	struct js_event		m_ev;
	struct js_event		m_button_ev;		// the last button event
	int					joystick_fd;
};


#endif
