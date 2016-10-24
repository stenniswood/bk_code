// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string> 
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "listbox.hpp"
#include "display.h"
#include "window.hpp"


#define Debug 1

Window::Window ( )
:Control()
{
	Initialize();
}

Window::Window ( int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{
	Initialize();
}

Window::Window( int Width, int Height )
:Control(Width, Height)
{
	Initialize();
}

Window::~Window( )
{
}

void	Window::Initialize		 (   )
{
	Control::Initialize();
	packer_vertical_position = bottom+height;
	packer_horizontal_l_position = left;		// Moves Left to Right.
	packer_horizontal_r_position = left+width;
/*	printf("Window::Initialize:  vp=%6.2f; lp=%6.2f; rp=%6.2f width=%6.1f\n", 
			packer_vertical_position,
			packer_horizontal_l_position, 
			packer_horizontal_r_position, width );	*/
}

int   	Window::draw(	 )
{
	if (Visible==false)	 return 0;
	Control::draw();
	return 1;
}

void	Window::calc_metrics	 (   )
{

}
// Works like a stack from top down of the window.  
void Window::pack_control( Control* mNewControl, byte mHorizontalPacking, byte mVerticalPacking )
{
	//printf("Window::pack_control:  packer: vert_pos=%6.2f; horiz_l=%6.2f horiz_r=%6.2f\n", 
	//		packer_vertical_position, packer_horizontal_l_position, packer_horizontal_r_position );
	//printf("Window::pack_control: <%6.1f,%6.1f> \n", mNewControl->left, mNewControl->bottom );

	set_vertical_position  ( mNewControl, mVerticalPacking   );
	set_horizontal_position( mNewControl, mHorizontalPacking );	
	register_child( mNewControl );
}

void Window::set_vertical_position( Control* mNewControl, byte mVerticalPacking )
{
	float ctrl_bottom = bottom;
	if (mVerticalPacking & PACK_FILL_PARENT)
	{ 
		// expand up & downward
		int H = (packer_vertical_position-bottom);
		mNewControl->set_width_height( mNewControl->get_width(), H );
		ctrl_bottom = bottom;
	} 
	else if (mVerticalPacking & PACKER_ALIGN_TOP)	
	{
		// default
		ctrl_bottom = packer_vertical_position-mNewControl->get_height();
		packer_vertical_position = ctrl_bottom - DefaultPadding;
	}
	else if (mVerticalPacking & PACKER_ALIGN_CENTER)	
	{
		float remaining = packer_vertical_position-bottom;		
		ctrl_bottom = (remaining - mNewControl->get_height())/2.0 + bottom;		
	}
	else if (mVerticalPacking & PACKER_ALIGN_BOTTOM)	
	{
		ctrl_bottom = bottom;		
	}
	mNewControl->move_bottom_to( ctrl_bottom );	
}

/* The packer could get complicated.
	The walls keep moving inward.  
	However, if we go below a control which was on the left, then the packer_horizontal_l_position
	should reset to "left" again.  But I'm not sure of the complete algorithm, so we'll
	not do that.
*/
void Window::set_horizontal_position( Control* mNewControl, byte mHorizontalPacking )
{
	float new_left;
	if ( mHorizontalPacking & PACK_FILL_PARENT )
	{
		new_left = packer_horizontal_l_position;
		int W = packer_horizontal_r_position - packer_horizontal_l_position;
		//packer_vertical_position -= mNewControl->height;
		mNewControl->set_width_height( W, mNewControl->get_height() );
	}
	if ( mHorizontalPacking & PACK_LEFT )
	{
		new_left = (packer_horizontal_l_position+DEFAULT_PADDING_HORIZ);
		packer_horizontal_l_position += (DEFAULT_PADDING_HORIZ+mNewControl->get_width());
	}
	if ( mHorizontalPacking & PACK_CENTER )
	{
		new_left = ((packer_horizontal_r_position-packer_horizontal_l_position) 
					- mNewControl->get_width()) / 2.0 + packer_horizontal_l_position;	
		packer_horizontal_l_position = new_left+mNewControl->get_width();
	}
	if ( mHorizontalPacking & PACK_RIGHT )
	{
		printf("Window::set_horizontal_position( PACK_RIGHT  r_pos=%6.1f  width=%6.1f\n", 
		packer_horizontal_r_position, mNewControl->get_width() );
		new_left  = packer_horizontal_r_position - mNewControl->get_width() - DEFAULT_PADDING_HORIZ;
		packer_horizontal_r_position = new_left-1; 	// left of control is right of the next control!
	}
	mNewControl->move_left_to( new_left );
}

void Window::pack_below  (Control* mNewControl, Control* mReferenceControl, byte mHorizontalPacking)
{
	mNewControl->set_position_below(mReferenceControl);

	if (mHorizontalPacking & PACK_COPY_HORIZ)
	{
		mNewControl->move_to( mReferenceControl->get_left(),  mNewControl->get_bottom() );
		mNewControl->set_width_height( mReferenceControl->get_width(), mNewControl->get_height() );
		
	} else 
		set_horizontal_position( mNewControl, mHorizontalPacking );

	register_child( mNewControl );
}

void Window::pack_above ( Control* mNewControl, Control* mReferenceControl, byte mHorizontalPacking )
{
	mNewControl->set_position_above(mReferenceControl);
	if (mHorizontalPacking & PACK_FILL_PARENT )
	{
		mNewControl->move_to( left, mNewControl->get_bottom() );
		mNewControl->set_width_height( width, mNewControl->get_height() );		
	}
	if (mHorizontalPacking & PACK_LEFT )
	{
		//mNewControl->get_left() = left;
		mNewControl->move_to( left, mNewControl->get_bottom() );
	}
	if (mHorizontalPacking & PACK_CENTER )
	{
		//mNewControl->get_left() = (width - mNewControl->width) / 2.0;		
		mNewControl->move_to( left, mNewControl->get_bottom() );		
	}
	if (mHorizontalPacking & PACK_RIGHT )
	{
		//mNewControl->get_left()  = left+width - mNewControl->get_width();
		mNewControl->move_to( left+width - mNewControl->get_width(), mNewControl->get_bottom() );				
	}		
	register_child( mNewControl );
}

/* This takes a control which has absolute positioning.  and puts it in reference of the 
   window's <left,bottom>

   This will add the window offsets before registering the control.   
   This means the Control's left, bottom are offsets from the Window's!
*/
int	Window::add_control_local_coords( Control* mControl )
{
	mControl->move_by( left, bottom ); 
	register_child( mControl );	
	return 1;
}

int	Window::onCreate(  )
{
	int retval = Control::onCreate();
	place_views();
	return retval;
}

int	Window::add_control ( Control* mControl )
{
	register_child( mControl );	
	return 1;
}

void	Window::move_by			( int dX, int dY )
{
	move_to( left+dX, bottom+dY );
}

void	Window::move_to			( float NewX, float NewY )
{
	float deltaX = NewX - left;
	float deltaY = NewY - bottom;
	Control::move_to(NewX, NewY);

	packer_horizontal_l_position += deltaX;
	packer_horizontal_r_position += deltaX;
	packer_vertical_position     += deltaY;
	
}

void	Window::set_width_height( int NewWidth, int NewHeight )
{
	width  = NewWidth;
	height = NewHeight;

	// For a new window, first time geting width height, need to do this.
	if (m_child_controls.size()==0)
	{	
		// but not if there are controls already packed!
		packer_vertical_position     = bottom+height;
		packer_horizontal_l_position = left;	// Moves Left to Right.
		packer_horizontal_r_position = left+width;
	}
}

Control*	Window::HitTest	( int x, int y )
{
	Control* result = Control::HitTest(x,y);
	if (result)		// within the bounds of this window.
	{
		// Disperse to affected child:  (do this in onClick() )
		// Not hitting any child objects...
		return this;		//Window::click( int x, int y);
	}
	else
		return NULL;
}

int Window::onClick(int x, int y, bool mouse_is_down)
{
	// Disperse to affected child:
	Control::onClick(x,y,mouse_is_down);
	
/*	list<Control*>::iterator  iter = controls.begin();
	for (int i=0; iter!=controls.end(); i++, iter++)
	{
		Control* result = (*iter)->HitTest( x, y );
		if (result) {
			(*iter)->onClick( x, y );
			return TRUE;
		}				
	}*/
	return FALSE;
}

