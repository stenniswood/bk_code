/***********************************************************************
* DisplayManager
*
* 
* Steve Tenniswood
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string> 
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "bk_system_defs.h"
#include "mouse.h"
#include "adrenaline_windows.h"
#include "rectangle.hpp"
#include "display_manager.hpp"
#include "avisual_menu.hpp"		// Specific system menu.  not part of adrenaline_windows



// Offer one instance for whole app;
DisplayManager MainDisplay(1920, 1080);
#define Debug 1

DisplayManager::DisplayManager(int Left, int Right, int Top, int Bottom )
: IconView(  Left,  Right,  Top,  Bottom, NULL)
{
	background_color = 0x800000B0;

	screen_width   = Right-Left;
	screen_height  = Bottom-Top;
	init_screen();
}

DisplayManager::DisplayManager( int mScreenWidth, int mScreenHeight )
: IconView( 0,  mScreenWidth,  mScreenHeight,  0, NULL )
{
	background_color = 0x800000B0;

	screen_width   = mScreenWidth;
	screen_height  = mScreenHeight;
	init_screen();
}

// Graphics initialization
void  DisplayManager::init_screen()
{
	init(&screen_width, &screen_height);	// Graphics initialization
	printf("\nDisplayManager::\tscreen_width=%d;\tscreen_height=%d\n", screen_width, screen_height );
	mouse_init( screen_width, screen_height );
	load_resources();	
}
// Screen initialization
void  DisplayManager::start_screen()
{
	Start(screen_width, screen_height);		// Start the picture
//	esTranslate(ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz);
}
void  DisplayManager::end_screen( )
{
	End();
}

int	DisplayManager::onCreate(  )
{
	m_sb.set_width_height( screen_width, 36 );
	float b = (screen_height-m_sb.get_height());
	printf("DisplayManager::onCreate()  %d,%d, %5.1f\n", screen_width, screen_height, b );

	// Top:
	m_sb.move_to		   ( 0, b );
	init_avisual_menu  ( &(m_sb.m_Menu) );
	add_object( &m_sb );
	m_sb.print_positions();

	// Bottom Status
	m_status.move_to		 ( 0, 			 m_status.get_height() );
	m_status.set_width_height( screen_width, 64 );
	add_object( &m_status );

	// RIGHT SideBar
	const int sidebar_width = 200;
	m_soft_side.move_to			( screen_width-200, m_status.get_height() );
	m_soft_side.set_width_height( 200, height-m_status.get_height()-m_sb.get_height() );
	add_object( &m_soft_side );

}

Rectangle*	DisplayManager::get_useable_rect( )
{
	static Rectangle rect;
	// TOP:
	rect.set_top( m_sb.get_bottom() );

	// Right Side:
	if (m_soft_side.is_visible()==true)
		rect.set_right( m_soft_side.get_left()-1 );
	else
		rect.set_right( screen_width );
	
	// no task bar yet!
	rect.set_left( 0 );

	// Bottom:
	if (m_status.is_visible()==true)
		rect.set_bottom( m_status.get_top() );
	else
		rect.set_bottom( 0 );
	return &rect;
}

// Perhaps this is not needed.  need to re-architect.
void DisplayManager::call_on_creates( )
{
	// System Bar will always be treated separately.
	m_sb.onCreate();
	m_soft_side.onCreate();
	m_status.onCreate();
	
	// Load all controls which are already registered.
	printf("Creating child controls\n");
	list<Control*>::iterator	iter = controls.begin();
	for (int i=0; iter!=controls.end(); i++, iter++ )
	{
		(*iter)->onCreate();
	}
}

void DisplayManager::load_resources( )
{
	if (Filename)
	{
		read_from_jpeg_file( );
	}
	// Load all controls which are already registered.
	printf("Loading child resources\n");
	list<Control*>::iterator	iter = controls.begin();
	for (int i=0; iter!=controls.end(); i++, iter++ )
	{
		(*iter)->load_resources();
	}
}

void  DisplayManager::set_title( char* Title )
{
}
//	A more preferred way of doing the background:
//void  DisplayManager::set_background( void* Image 	  )
void  DisplayManager::set_background( char* mFileName )
{
	Filename = mFileName;
}
// "/home/pi/openvg/client/desert0.jpg";

void  DisplayManager::set_background_color( long int Color )
{
	background_color = Color;
}

void  DisplayManager::add_object( Control* NewControl )
{
	NewControl->onCreate();
	controls.push_back( NewControl );
}

// This removes the object from the list.  Does not delete or free memory.
// because some may be static.  Leave that to the caller.
void  DisplayManager::remove_object( Control* NewControl )
{

}

void  DisplayManager::remove_all_objects(  )
{
	// not going to actually delete/free the objects since some are static!
	Head = NULL;
	Tail = NULL;
	controls.clear();
}

void  DisplayManager::start_draw(	)
{
	start_screen();
}

int   DisplayManager::draw(	)
{
	if (Debug) printf("\ndraw display manager\tstart:\n" );
	start_draw();
	draw_background();
	//m_sb.print_positions();
	draw_children();	
	end_draw();				// end is needed to see display!
	if (Debug) printf("draw display manager\tdone!\n\n" );
}

void  DisplayManager::end_draw(	)
{
	end_screen();	   // End the picture
}

/*void  DisplayManager::onClick( int x, int y, bool mouse_is_down=true )
{
	Control* obj = HitTest(x,y);
	obj->onClick();
}*/

Control* DisplayManager::HitTest( int x, int y )
{
	Control* result = m_sb.HitTest(x,y);
	if (result)  return result;
	
	list<Control*>::iterator iter = controls.begin();
	for (int i=0; iter!=controls.end(); i++, iter++ )
	{
		result = (*iter)->HitTest(x,y);
		if (result) {
			printf ("DisplayManager::HitTest:  found a hit. child number %d\n", i );
			return (result);
			//return result;
		}
	}
	return NULL;	
}

int   DisplayManager::draw_children( )
{
	m_sb.draw();
	m_soft_side.draw();
	m_status.draw();
	
	printf("\t\tside bar	\t");	m_soft_side.print_positions();
	printf("\t\tstatus bar	\t");	m_status.print_positions();
		
	list<Control*>::iterator	iter = controls.begin();
	for (int i=0; iter!=controls.end(); i++, iter++ )
	{
		if (Debug) printf("draw child %d\n", i);	
		(*iter)->draw();		
	}
	return -1;	
}

int   DisplayManager::draw_text( 	)
{
	return -1;
}

int   DisplayManager::draw_background( 	)
{
	if (file_loaded)
		IconView::draw();	
	else if(1) {
		// TEST PATTERN:
		// [intensity, Red, Green, Blue, Alpha]
		VGfloat stops[] = {
		0.0, 0.0, 0.0, 1.0, 1.0,
		0.5, 0.0, 0.0, 0.5, 1.0,
		1.0, 1.0, 1.0, 1.0, 1.0
		};	
		float x1 = 0.0;				float y1 = 0.0;
		float x2 = screen_width;	float y2 = screen_height+1;
		FillLinearGradient(x1, y2, x1, y1, stops, 3);
		Rect(x1, y1, x2-x1, y2-y1);
	} else {
		Background( //(background_color & 0xFF000000)>>24,
				(background_color & 0x00FF0000)>>16,
				(background_color & 0x0000FF00)>>8,
				(background_color & 0x000000FF) );
	}
	return 1;
}

/*Control*  DisplayManager::Find_object( Control* NewControl )
{
	Control* ctrl = Head;
	while (ctrl)
	{
		if (ctrl->id==NewControl->id)
			return ctrl;
		ctrl = ctrl->getNext();
	}
	return NULL;
}*/
