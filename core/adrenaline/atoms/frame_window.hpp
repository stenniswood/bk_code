#ifndef _FRAME_WINDOW_CONTROL_
#define _FRAME_WINDOW_CONTROL_

#include "window.hpp"

#include <list>
#include <string>
using namespace std;


#define CLOSE_HIT 1
#define MIN_HIT   2
#define MAX_HIT   3
#define CLIENT_AREA_HIT 98
#define DRAG_HIT   99


#define LEFT_PADDING_CLOSE 10
#define MIN_MAX_CLOSE_CIRCLE_RADIUS  7

#define LEFT 	0
#define RIGHT 	1
#define TOP 	2
#define BOTTOM 	3

/***************************************************************
This control holds a collection of child controls into a single frame.
The window can be moved around, etc, like a microsoft window.

Later on maybe accept drag and drop items.
****************************************************************/
class FrameWindow : public Window
{
public:
	FrameWindow ( );
	FrameWindow ( int Left, int Right, int Top, int Bottom );
	~FrameWindow( );

	virtual void	Initialize		( 		);	

	virtual int   	draw 			(	 	);
	int   			draw_header		( 						);
	int   			draw_close		( bool mHovering=false	);
	int   			draw_min 		( bool mHovering=false 	);
	int   			draw_max 		( bool mHovering=false 	);
	void			calc_metrics	(   					);


	virtual void			move_to			( float NewX, float NewY );
	virtual void			set_width_height( int NewWidth, int NewHeight );

	// This will add the window offsets to it when drawing:
	//int			add_control_local_coords( Control* mControl );
	// keeps the coords as is.
	int				add_control				( Control* mControl );
	int				add_control_local_coords( Control* mControl );

//	virtual Control* HitTest(int x, int y);
	void 			allocate_save_buff	();		// for current dimensions.
	void 			save_pixels		  	(/*int screen_width, int screen_height*/);		// for current dimensions.
	void 			restore_pixels		();		// for current dimensions.

	virtual Control* HitTest		(int x, int y);
	// need to put mouse_is_down into Control:: and all derived classes!
	int		 	onClick		(int x, int y, bool mouse_is_down);
			
protected:
	int			 HitTestArea		(int x, int y);
	float		close_button_coords[4];
	float		min_button_coords[4];
	float		max_button_coords[4];
	float		body_height;
	bool		is_dragging;
	
private:
	// The contents of the display are saved to memory 
	// for later restoring:
	int 	save_sx, save_sy, save_w, save_h;	// cursor location and dimensions
	int 	saved_bytes;	// amount of data saved in cursor image backup
	VGImage	restore_image;
	
};

 
#endif
