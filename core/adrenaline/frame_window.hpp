#ifndef _FRAME_WINDOW_CONTROL_
#define _FRAME_WINDOW_CONTROL_

#include "window.hpp"

#include <list>
#include <string>
using namespace std;

#define MIN_MAX_CLOSE_CIRCLE_RADIUS  6

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

	// This will add the window offsets to it when drawing:
	//int			add_control_local_coords( Control* mControl );
	// keeps the coords as is.
	int				add_control				( Control* mControl );
	int				add_control_local_coords( Control* mControl );
//	virtual Control* HitTest(int x, int y);

protected:
	float		close_button_coords[4];
	float		min_button_coords[4];
	float		max_button_coords[4];
	
private:
	
};

 
#endif
