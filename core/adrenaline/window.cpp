// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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


#define Debug 0

Window::Window ( )
:Control()
{
	Initialize();
	//printf("\t\tWindow::ctor()\n");
}

Window::Window ( int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{
	Initialize();
	//printf("\t\tWindow::ctor()\n");
}
Window::~Window( )
{
}
void	Window::Initialize		 (   )
{
	//printf("\t\tWindow::Initialize()\n");
	Control::Initialize();
	packer_vertical_position = bottom+height;
}

int   	Window::draw 			 (	 )
{
	printf("\tWindow::\t");	print_positions();

	Control::draw();
	
	list<Control*>::iterator	iter = controls.begin();
	for (int i=0; iter!=controls.end(); i++, iter++ )
	{
		printf("\t\t\t Drawing child %d\t", i);
		(*iter)->print_positions();
		//(*iter)->print_color_info();

		(*iter)->draw();
	}
}

void	Window::calc_metrics	 (   )
{

}
// Works like a stack from top down of the window.  
void Window::pack_control( Control* mNewControl, byte mHorizontalPacking, byte mVerticalPacking )
{
	printf("Window::pack_control:  packer_vertical_position=%6.2f\n", packer_vertical_position );
	//printf("Window::pack_control: <%6.1f,%6.1f> \n", mNewControl->left, mNewControl->bottom );
	if (mVerticalPacking & PACK_FILL_PARENT)
	{ 
		// expand downward
		mNewControl->set_width_height( mNewControl->width, height );
	} 
	mNewControl->move_bottom_to( packer_vertical_position-mNewControl->height );
	set_horiztonal_position( mNewControl, mHorizontalPacking );
	
	controls.push_back( mNewControl );
	packer_vertical_position = mNewControl->bottom - DefaultPadding;
}

void Window::set_horiztonal_position( Control* mNewControl, byte mHorizontalPacking )
{
	float new_left;
	if ( mHorizontalPacking & PACK_FILL_PARENT )
	{
		new_left = left;		
		mNewControl->set_width_height( width, mNewControl->height );
	}
	if ( mHorizontalPacking & PACK_LEFT )
	{
		new_left = left;
	}
	if ( mHorizontalPacking & PACK_CENTER )
	{
		new_left = (width - mNewControl->width) / 2.0;		
	}
	if ( mHorizontalPacking & PACK_RIGHT )
	{
		new_left  = left+width - mNewControl->width;
	}
	mNewControl->move_left_to( new_left );
}

void Window::pack_below  (Control* mNewControl, Control* mReferenceControl, byte mHorizontalPacking)
{
	mNewControl->set_position_below(mReferenceControl);
	if (mHorizontalPacking & PACK_FILL_PARENT )
	{
		mNewControl->left = left;
		mNewControl->width = width;
	}
	if (mHorizontalPacking & PACK_LEFT )
	{
		mNewControl->left = left;
	}
	if (mHorizontalPacking & PACK_CENTER )
	{
		mNewControl->left = (width - mNewControl->width) / 2.0;		
	}
	if (mHorizontalPacking & PACK_RIGHT )
	{
		mNewControl->left  = left+width - mNewControl->width;
	}		
	controls.push_back( mNewControl );
}

void Window::pack_above ( Control* mNewControl, Control* mReferenceControl, byte mHorizontalPacking )
{
	mNewControl->set_position_above(mReferenceControl);
	if (mHorizontalPacking & PACK_FILL_PARENT )
	{
		mNewControl->left = left;
		mNewControl->width = width;
	}
	if (mHorizontalPacking & PACK_LEFT )
	{
		mNewControl->left = left;
	}
	if (mHorizontalPacking & PACK_CENTER )
	{
		mNewControl->left = (width - mNewControl->width) / 2.0;		
	}
	if (mHorizontalPacking & PACK_RIGHT )
	{
		mNewControl->left  = left+width - mNewControl->width;
	}		
	controls.push_back( mNewControl );
}

/* This takes a control which has absolute positioning.  and puts it in reference of the 
   window's <left,bottom>
   */
// This will add the window offsets before adding:
int	Window::add_control_local_coords( Control* mControl )
{
	int dLeft   = left;
	int dBottom = bottom;
	mControl->move_by( -left, -bottom );
	controls.push_back( mControl );
}

/*int	Window::add_control_absolute_coords( Control* mControl )
{
	controls.push_back( mControl );
}*/

int		Window::add_control		 ( Control* mControl )
{
	controls.push_back( mControl );
}

void	Window::move_by			( int dX, int dY )
{
	move_to( left+dX, bottom+dY );
}

void	Window::move_to			( int NewX, int NewY )
{
	int deltaX = NewX - left;
	int deltaY = NewY - bottom;

	int tmpLeft;
	int tmpBottom;

	// Adjust all children (stored in absolute):	
	list<Control*>::iterator	iter = controls.begin();
	for (int i=0; iter!=controls.end(); i++ )
	{
		(*iter)->get_xy ( &tmpLeft, &tmpBottom 			 );
		(*iter)->move_to( tmpLeft+deltaX, tmpBottom+deltaY );
	}
}

void	Window::resize			( int NewWidth, int NewHeight )
{
	width = NewWidth;
	height = NewHeight;
}

Control*	Window::HitTest	( int x, int y )
{
	Control* result = Control::HitTest(x,y);
	if (result)
	{
		// Disperse to affected child:
		list<Control*>::iterator	iter = controls.begin();
		for (int i=0; iter!=controls.end(); i++ )
		{
			return (*iter)->HitTest ( x, y );
		}
	}
}



