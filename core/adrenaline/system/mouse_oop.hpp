#ifndef _MOUSE_
#define _MOUSE_


#include <vector>
#include <linux/input.h>


// The thread puts this into left, middle, right.
// The timeslice will clear it.
#define UNHANDLED_EVENT 0x80

#define LEFT_BUTTON_DOWN  0x10
#define RIGHT_BUTTON_DOWN 0x01


// Mouse state structure
struct mouse_t {
	VGfloat 			x,     y;
	int 				left,  middle, right;
	int 				max_x, max_y;
};

struct finger_info {
	int		finger_number;
	float	x;
	float	y;	
	float	pressure;
	int		state;		// down, released
};


class Mouse 
{
public:
	Mouse();
	~Mouse();
	
	void			init_fingers( ); 
	virtual void	Initialize(); 					// int mouse_init()

	void			select_mouse_image	(int mIndex);	
	virtual void	handle_event		();			
	void			extract_finger_info	();			// for multi-touch screens
	virtual int		time_slice			(); 	

	void			print_event_info();
	
	void 			close_fd();

	void			save_pixels		( int curx, int cury, int screen_width, int screen_height, int s);
	void			restore_pixels	( );
	void			circleCursor	( int curx, int cury, int width, int height, int s);
	void			crosshairsCursor( int curx, int cury, int width, int height );
	void			carretCursor    ( int curx, int cury, int width, int height );
	
	void			load_mouse_images();	

	void 			hide_mouse();
	void 			show_mouse();
	void 			save_mouse();

	// VARIABLES:
	std::vector<VGImage>	m_icons;
	int						m_icon_selected;
	struct finger_info		m_fingers[10];
	int						m_finger;
	int						m_num_fingers_down;	
	std::vector<struct finger_info>	m_finger_history[10];	
	struct input_event 		mouse_ev;			// info read from driver
	struct timeval			m_prev_ev_time;		// of previous mouse_ev
	
	// get these values from display manager.	
	VGImage CursorBuffer;
	int 	cursorx, cursory, cbsize;	

	VGfloat 			x,     y;					// Current lcoation <x,y>
	int 				left,  middle, right;
	int 				max_x, max_y;	

	int 	cur_sx, cur_sy, cur_w, cur_h;	// cursor location and dimensions
	int 	cur_saved;						// amount of data saved in cursor image backup
	int		mouse_mode;			// circle, cross hairs, or carret.
};



#endif
