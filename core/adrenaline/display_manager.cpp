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
#define Debug 0
#define Debug2 0

DisplayManager::DisplayManager(int Left, int Right, int Top, int Bottom )
: IconView(  Left,  Right,  Top,  Bottom, NULL)
{
	background_color = 0x800000B0;

	screen_width   = Right-Left;
	screen_height  = Bottom-Top;
	init_screen();
	Initialize();
}

DisplayManager::DisplayManager( int mScreenWidth, int mScreenHeight )
: IconView( 0,  mScreenWidth,  mScreenHeight,  0, NULL )
{
	background_color = 0x800000B0;

	screen_width   = mScreenWidth;
	screen_height  = mScreenHeight;
	init_screen();
	Initialize();
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
// Perhaps this is not needed.  need to re-architect.
void DisplayManager::call_on_creates( )
{
	// System Bar will always be treated separately.
	
	// Load all controls which are already registered.
	if (Debug) printf("Creating child controls\n");
	Control::onCreate();
}

void DisplayManager::Initialize()
{	
	// put actual icons on it:
	init_default_sidebar( &m_side );
	m_current_running_app = -1;
}

int	DisplayManager::onPlace( )
{
	if (Debug) printf("DisplayManager::onPlace() \n");
	// System Bar on Top:
	float system_bar_height = 36.;
	float status_height = 64.;

	m_sb.set_width_height( screen_width, system_bar_height );
	float b = (screen_height- system_bar_height);
	m_sb.move_to	    ( 0,  b );
	m_sb.onPlace		( );
	if (Debug) 
	{
		printf("DisplayManager::onPlace()  %d,%d, %5.1f\n", screen_width, screen_height, b );
		print_children();
		m_sb.print_positions( );
	}
	
	// Status Bar on Bottom:
	m_status.move_to		 ( 0, 0 );
	m_status.set_width_height( screen_width, status_height );
	if (Debug) m_status.print_positions (  );
	m_status.onPlace		 (  );
	
	// RIGHT SideBar
	int sidebar_width = m_side.get_expanded_width();
	float h 		  = height - status_height - system_bar_height;
	// m_sb.get_height() = 36.
	// m_status.get_height() = 64
	m_side.move_to			( screen_width-sidebar_width, status_height );
	m_side.set_width_height( sidebar_width, h );
	if (Debug) m_side.print_positions();
	m_side.onPlace( );
}

int	DisplayManager::onCreate(  )
{	
	Control::onCreate();
	onPlace();
}

void DisplayManager::start_app( Application* mApp )
{
//	mApp->onCreate();
	m_running_apps.push_back( mApp );
	m_current_running_app = m_running_apps.size()-1;
	printf("DISPLAY MANAGER:  START APP #%d .......... \n", m_current_running_app );

	// register with DM 
	remove_all_objects(	);
	add_object( mApp->m_main_window 	 );
	set_menu  ( &(mApp->m_hMenu)  	 	 );
	m_side.load_controls( &(mApp->m_sidebar_controls) );	
	m_status.set_text( mApp->m_welcome_status.c_str() );	

}

void DisplayManager::idle_tasks( )
{
	if ((m_current_running_app>=0) && (m_current_running_app<m_running_apps.size())) {
		//printf("idle task: %d \n", m_current_running_app );
		m_running_apps[m_current_running_app]->background_time_slice();	
	}	
}

void DisplayManager::close_app( Application* mApp )
{
	for (int i=0; i<m_running_apps.size(); i++)
	{
		if (m_running_apps[i] == mApp)
			m_running_apps.erase( m_running_apps.begin()+i );
	}
	delete mApp;
}

void DisplayManager::set_menu( HorizontalMenu* mHMenu )
{
	if (Debug)  printf("\t\tDisplayManager::set_menu( )\n");
	if (mHMenu==NULL)
	{
		m_sb.set_menu( &system_hmenu );
		onPlace();
		m_sb.Invalidate();
	} else {
		if (Debug) printf("\t\tDisplayManager::set_menu( )\n");	
		m_sb.set_menu( mHMenu );
		if (Debug) m_sb.print_positions();
		if (Debug) m_sb.m_Menu->print_positions();

		onPlace();
		m_sb.draw();
		m_sb.Invalidate();		
	}
}

Rectangle*	DisplayManager::get_useable_rect( )
{
	static Rectangle rect;
	// TOP:
	rect.set_top( m_sb.get_bottom() );

	// Right Side:
	if (Debug) printf("DisplayManager::get_useable_rect( )\n");
	
	if (m_side.is_visible()==true)
		rect.set_right( m_side.get_left()-1 );
	else
		rect.set_right( screen_width );
	//printf("right = %6.1f\n", rect.get_right() );
	
	// no task bar yet!
	rect.set_left( 0 );

	// Bottom:
	if (m_status.is_visible()==true)
		rect.set_bottom( m_status.get_top() );
	else
		rect.set_bottom( 0 );
	return &rect;
}


void DisplayManager::load_resources( )
{
	if (Filename)
	{
		read_from_jpeg_file( );
	}
	
	// Load all controls which are already registered.
	if (Debug) printf("Loading child resources\n");
	vector<Control*>::iterator	iter = m_child_controls.begin();
	for (int i=0; iter!=m_child_controls.end(); i++, iter++ )
	{
		(*iter)->load_resources();
	}
}


//	A more preferred way of doing the background:
void  DisplayManager::set_background( char* mFileName )
{
	Filename = mFileName;
}


void  DisplayManager::add_object( Control* NewControl )
{
	NewControl->onCreate();
	register_child( NewControl );	
}

// This removes the object from the list.  Does not delete or free memory.
// because some may be static.  Leave that to the caller.
void  DisplayManager::remove_object( Control* NewControl )
{

}

void  DisplayManager::remove_all_objects(  )
{
	// not going to actually delete/free the objects since some are static!
	m_child_controls.clear();

	// Put back in the bare essentials!		
	register_child( &m_status );	
	register_child( &m_sb   );
	register_child( &m_side );
}

int   DisplayManager::draw(	)
{
	if (Debug2) printf("\n======display manager draw===========\tStart:\n" );
	start_screen();
	draw_background();
	draw_children();
	end_draw();				// end is needed to see display!
	if (Debug2) printf("======display manager draw===========\tDone!\n\n" );
}

int DisplayManager::any_invalid_children()
{
	vector<Control*>::iterator	iter = m_child_controls.begin();
	for (int i=0; iter!=m_child_controls.end(); i++, iter++ )
	{
		if 	((*iter)->is_invalid())
			return TRUE;	
	}	
	return FALSE;
}
/* Mark all objects as valid, since we just redrew */
int	DisplayManager::update_invalidated(  )
{
	vector<Control*>::iterator	iter = m_child_controls.begin();
	for (int i=0; iter!=m_child_controls.end(); i++, iter++ )
	{
		(*iter)->Revalidate();
	}	
}
void  DisplayManager::end_draw(	)
{
	end_screen();	   // End the picture
}

int   DisplayManager::draw_children( )
{
/*	m_sb.draw();
	m_side.draw();
	m_status.draw(); */	
	
	if (Debug) { printf("\t\tside bar	\t");	m_side.print_positions();	}
	if (Debug) { printf("\t\tstatus bar	\t");	m_status.print_positions();	}

	vector<Control*>::iterator	iter = m_child_controls.begin();
	for (int i=0; iter!=m_child_controls.end(); i++, iter++ )
	{
		if (Debug2) printf("draw child %d\n", i);
		(*iter)->draw();		
	}
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

Control* DisplayManager::HitTest( int x, int y )
{
	//print_children();
	//printf("m_sb=%x\n", &m_sb);
	//	m_sb.print_children();

	Control* retval = Control::HitTest(x,y);
	//printf("\tDisplayManager::HitTest() %x \n", retval );
	if (retval==this)
		return NULL;	// don't want display manager empty space to do anything.
	else
		return retval;
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

/*void  DisplayManager::onClick( int x, int y, bool mouse_is_down=true )
{
	Control* obj = HitTest(x,y);
	obj->onClick();
}*/
