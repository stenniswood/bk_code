#ifndef _WINDOW_CONTROL_
#define _WINDOW_CONTROL_

#include "control.hpp"

#include <list>
#include <string>
using namespace std;

#define DEFAULT_VSCROLL_WIDTH  10
#define DEFAULT_HSCROLL_HEIGHT 10 

#define MAX_CONTROLS 6

#define SIDEBAR_ALIGN_TOP     0x01
#define SIDEBAR_ALIGN_CENTER  0x02
#define SIDEBAR_ALIGN_BOTTOM  0x04

#define PACK_LEFT     0x01
#define PACK_CENTER   0x02
#define PACK_RIGHT    0x04
#define PACK_COPY_HORIZ    0x08

#define PACK_FILL_PARENT    0x10			// width will be expanded to allowed room.
#define PACK_WRAP_CONTENT   0x20			// width will come from the control width.

#define PACKER_ALIGN_TOP      0x01
#define PACKER_ALIGN_CENTER   0x02
#define PACKER_ALIGN_BOTTOM   0x04

#define DEFAULT_PADDING_HORIZ 12
#define DEFAULT_PADDING_VERT  12

/***************************************************************
This control holds a collection of child controls into a single frame.
The window can be moved around, etc, like a microsoft window.

Later on maybe accept drag and drop items.
****************************************************************/
class Window : public Control 
{
public:
	Window ( );
	Window ( int Left, int Right, int Top, int Bottom );
	Window ( int Width, int Height );	
	~Window( );

	virtual void	Initialize		 (   );	
	virtual int   	draw 			 (	 );
	void			calc_metrics	 (   );

	void			print_window_positions ( );
	
	void			move_by			( int dX, int dY 			);
	virtual void	move_to			( float NewX, float NewY 	);
	virtual void	set_width_height( int NewWidth, int NewHeight );

	void			to_controls		();
	void			from_controls	();
	
	/* For drawing, the coords have to be absolute.
	For moving, we can just update all children by the change x,y; so use absolute;
	For clipping, we can use window height,width.
	For HitTest, want absolute (since mouse will be abs)
	So absolute seems to win the votes unanimously!
	*/
	void 			set_vertical_position  ( Control* mNewControl, byte mVerticalPacking   );
	void 			set_horizontal_position( Control* mNewControl, byte mHorizontalPacking );

	void			pack_control(Control* mControl,   byte mHorizontalPacking,  byte mVerticalPacking=0 ); 
	void			pack_below  (Control* mNewControl,Control* mReferenceControl, byte mHorizontalPacking );
	void			pack_above  (Control* mNewControl,Control* mReferenceControl, byte mHorizontalPacking );	

	// This will add the window offsets to it when drawing:
	//int			add_control_local_coords( Control* mControl );
	// keeps the coords as is.
	int				add_control( Control* mControl );
	int				add_control_local_coords( Control* mControl );

	virtual Control* HitTest( int x, int y );
	virtual int		 onClick( int x, int y, bool mouse_is_down=true );
	
protected:
	list<Control*>	controls;
	float			packer_vertical_position;		// Moves Top to bottom.

	float			packer_horizontal_l_position;		// Moves Left to Right.
	float			packer_horizontal_r_position;		// Moves Left to Right.	

private:
	
};

 
#endif
