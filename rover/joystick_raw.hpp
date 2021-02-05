#ifndef _JOYSTICK_
#define _JOYSTICK_

#include <string>
#include <vector>
#include <linux/input.h>

#define BUFFER_SIZE 1024


// The thread puts this into left, middle, right.
// The timeslice will clear it.
#define UNHANDLED_EVENT 0x80


#define PS4 1

//PS3 BUTTON DEFINITIONS:
#ifdef PS4
#define PS_PAD_PUSH			18 				
#define PS_BUTTON			17			
#define PS_R3		 		16
#define PS_L3 				15
#define PS_OPTIONS			14
#define PS_SHARE 			13

#define PS_R2			 	12		// R2
#define PS_L2 				11		// L2
#define PS_R1 				10		// R1
#define PS_L1		 		9		// L1

#define PS_DPAD_UP 			8
#define PS_DPAD_DOWN 		7
#define PS_DPAD_LEFT 		6
#define PS_DPAD_RIGHT 		5

#define PS_TRIANGLE 		3		
#define PS_CIRCLE 			2
#define PS_X 				1
#define PS_SQUARE 			0

enum dpad { N=0, NE=1, E=2, SE=3, S=4, SW=5, W=6, NW=7, Released=8 };

#endif


// PS4 ANALOG SENSORS:
#ifdef PS4
// DATAPOS is byte index into the Data Report
#define PS_AXIS_LPOD_LR_DATAPOS		 		3
#define PS_AXIS_LPOD_UD_DATAPOS    			4
#define PS_AXIS_RPOD_LR_DATAPOS				5
#define PS_AXIS_RPOD_UD_DATAPOS 			6

#define PS_AXIS_NSEW_DPAD_DATAPOS 			7	/* North South,East,West buttons */
#define PS_AXIS_OPTSHARE_LR_DATAPOS 		8	/* R3,L3,Opt,Share, */
#define PS_COUNTER_DATAPOS 					9	/* R3,L3,Opt,Share, */

#define PS_AXIS_LTRIGGER_DATAPOS		 	10
#define PS_AXIS_RTRIGGER_DATAPOS		 	11


#define PS_AXIS_BATTERY_DATAPOS			 	32
#define PS_AXIS_ANGLEX_VELOCITY_DATAPOS 	15
#define PS_AXIS_ANGLEY_VELOCITY_DATAPOS 	17
#define PS_AXIS_ANGLEZ_VELOCITY_DATAPOS 	19

#define PS_AXIS_ACCELX_VELOCITY_DATAPOS 	22
#define PS_AXIS_ACCELY_VELOCITY_DATAPOS 	24
#define PS_AXIS_ACCELZ_VELOCITY_DATAPOS 	26

#define PS_TP_NUM_PACKETS_DATAPOS 			35
#define PS_TP_FINGERID_1_DATAPOS  			37



// NOW THE INDEX INTO m_axis_latest[]
#define PS_AXIS_LPOD_LR		 	0
#define PS_AXIS_LPOD_UD    		1
#define PS_AXIS_RPOD_LR			2
#define PS_AXIS_RPOD_UD 		3

#define PS_DPAD 				4

#define PS_AXIS_TILT_ROLL 	 	5
#define PS_AXIS_TILT_PITCH 	 	6

#define PS_AXIS_BATTERY 	 	7
#define PS_AXIS_ANGLEX_VEL 	 	8
#define PS_AXIS_ANGLEY_VEL 	 	9
#define PS_AXIS_ANGLEZ_VEL 	 	10

#define PS_AXIS_ACCELX 	 		11
#define PS_AXIS_ACCELY 	 		12
#define PS_AXIS_ACCELZ 	 		13

#define PS_AXIS_LTRIGGER 	 	14
#define PS_AXIS_RTRIGGER 	 	15

#else
#endif

std::string get_button_name( int mButtonIndex );
std::string get_axis_name  ( int mAxisIndex   );

#define MAX_BUTTONS 40
#define MAX_AXIS    20

extern char  dev_name[40];
typedef unsigned char byte;

class ButtonState {
public:
	ButtonState() {
		value		= false;
		pressed		= false;
		released	= false;	
		prev_value	= false;
	};
	void set_value( bool mNewValue ) {
			prev_value = value;
			value 	   = mNewValue;
			if ((value) && (!prev_value)) 		pressed = true; 
			else if ((!value) && (prev_value)) 	released = true; 
	};
	void	acknowledge() { if (pressed) pressed=false;	if (released) released=false;	};

	bool	value;
	bool	pressed;
	bool	released;	
	bool	prev_value;
};


class JoystickRaw 
{
public:
	JoystickRaw (  );	
	~JoystickRaw();
	
	void			Initialize(); 			// int mouse_init()

	void 			open 					( const char* mPathname = NULL );
	long int 		read_report				( );
	void 			close					( );

	void			print_button_info 		( );
	void			print_analogs_info		( );
	void			print_touchpad_info 	( );

	void 			update					( );
	void			mark_event_acknowledged	( );

	void			dump_i_buffer			( );

//	bool			new_event_available()  { return (m_new_event); };
	ButtonState&	get_button	( int mIndex )  { return (m_buttons_latest[mIndex]); };
//	bool			get_button_named(const char* mName)  { return ::get_button_name(m_ev.number); };

protected:
	void			extract_buttons 	( );
	void			extract_analogs 	( );	
	void			extract_touchpad	( );
	void			print_button		( int mAxisIndex );
	void			print_axis			( int mAxisIndex );
private:
	char			full_device_name[80];

public:
	// VARIABLES:
	bool				m_new_axis;
	bool				m_new_button;
	bool				m_new_event;
	
	int					m_bytes_read;
	
	short				m_report_counter;
	short				m_axis_latest   [MAX_AXIS];
	ButtonState			m_buttons_latest[MAX_BUTTONS];	

	int					m_num_trackpad_packets;
	int					m_num_fingerid_1;
	
	int					joystick_fd;
};


#endif
