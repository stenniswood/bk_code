/***********************************************************************
* DisplayManager
*		This handles the System bar (at top), side bar, status bar,
*		and Application window.  In other words all the windowing for 
*		Adrenaline at the root level.
* 
* Steve Tenniswood
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string> 
#include <vector>
#include <list>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
//#include "Graphbase.hpp"
#include "mouse.h"
#include "rectangle.hpp"

#include "calendar.hpp"
#include "display_manager.hpp"
#include "avisual_menu.hpp"		// Specific system menu.  not part of adrenaline_windows




// Offer one instance for whole app;
DisplayManager MainDisplay(1920, 1080);
#define Debug  0
#define Debug2 0

Keyboard		m_keyboard;
Control			mctrl;
Calendar		m_calendar;


DisplayManager::DisplayManager(int Left, int Right, int Top, int Bottom )
: IconView(  Left,  Right,  Top,  Bottom, NULL)
{
	openvg_initialized = false;
	background_color = 0x800000B0;
	init_screen();
	Initialize();
} 

DisplayManager::DisplayManager( int mScreenWidth, int mScreenHeight )
: IconView( 0,  mScreenWidth,  mScreenHeight,  0, NULL )
{
	openvg_initialized = false;
	background_color = 0x800000B0;
	
	screen_width   = mScreenWidth;
	screen_height  = mScreenHeight;
	init_screen();		// will override above!
	Initialize();
}

// Graphics initialization
void  DisplayManager::init_screen()
{
	init(&screen_width, &screen_height);	// Graphics initialization
	openvg_initialized = true;
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
	IconView::Initialize();
	// put actual icons on it:
	init_default_sidebar( &m_side );
	m_current_running_app = -1;
}

void DisplayManager::show_keyboard	(   )
{
	if (Debug)  printf("DisplayManager::show_keyboard	(   )\n");
	m_keyboard.show();
	m_keyboard.draw();
	m_keyboard.z_order = ++m_z_order_counter;	// for the hittest 
	if (Debug)  printf("DisplayManager::show_keyboard  %d	\n", m_z_order_counter);
	sort_z_order();
}

void DisplayManager::hide_keyboard	(   )
{
	if (Debug)  printf("DisplayManager::hide_keyboard	(   )\n");
	m_keyboard.hide();
	//m_keyboard.Invalidate();
	draw();
}
void DisplayManager::show_calendar	(   )
{
	printf("DisplayManager::show_calendar	(   )\n");

	m_calendar.show();
	m_calendar.draw();
	m_calendar.z_order = ++m_z_order_counter;
	printf("DisplayManager::show_calendar  %d	\n", m_z_order_counter);
	sort_z_order();
}
void DisplayManager::hide_calendar	(   )
{
	printf("DisplayManager::hide_calendar	(   )\n");
	m_calendar.hide();
	//m_calendar.Invalidate();
	draw();
}

/*  DisplayManager - Places 
							System Bar, Side Bar, and Status Bar  */
int	DisplayManager::onPlace( )
{
	if (Debug) printf("DisplayManager::onPlace() \n");

	// SYSTEM Bar on Top:
	float system_bar_height = 36.;
	m_sb.set_width_height( screen_width, system_bar_height );
	float b = (screen_height- system_bar_height);
	m_sb.move_to	    ( 0,  b );
	m_sb.onPlace		( );
	if (Debug) 
	{
		if (Debug)  printf("DisplayManager::onPlace()  %d,%d, %5.1f\n", screen_width, screen_height, b );
		print_children();
		m_sb.print_positions( );
	}

	// STATUS Bar on Bottom:
	float status_height = 32.;	
	m_status.move_to		 ( 0, 0 );
	m_status.set_width_height( screen_width, status_height );
	if (Debug) m_status.print_positions(  );
	m_status.onPlace		 (  );

	// RIGHT SideBar
	int sidebar_width = m_side.get_expanded_width();
	float h 		  = screen_height - status_height - system_bar_height;
	m_side.move_to		   ( screen_width-sidebar_width, status_height );
	m_side.set_width_height( sidebar_width, h );
	if (Debug) m_side.print_positions();
	m_side.onPlace( );
	m_side.hide   ( );

	// PLACE KEYBOARD : 
	m_keyboard.set_position ( 0, screen_width, screen_height, 0.0 );
	m_keyboard.adjust_height(  );
	m_keyboard.hide			(  );
	bool vis = m_keyboard.is_visible( );
	if (vis==true)
		printf("KB Visible!\n");

	mctrl.hide();
	bool vis2 = mctrl.is_visible();
	if (vis2==true)
		printf("mCTRL VIsible!\n");

	m_keyboard.invalidated = false;
	printf("DisplayManager::onPlace() keyboard.hide() \n");

	// PLACE CALENDAR : 
	m_calendar.set_position( screen_width-m_calendar.get_width(), screen_width, m_calendar.get_height(), 0.0 );
	m_calendar.onCreate    ( );
	m_calendar.hide		   ( );
	return 1;
}

int	DisplayManager::onCreate(  )
{	
	int retval = Control::onCreate();
	onPlace();
	return retval;
}

/* 
	Run an app - Setting up it's menu, main_window, and sidebar controls.
*/
void DisplayManager::start_app( Application* mApp )
{
	if (mApp==NULL) 
	{
		printf("ERROR:  Application could not be started because it is NULL!\n");
		return;
	}

	m_running_apps.push_back( mApp );
	m_current_running_app = m_running_apps.size()-1;
	printf("DISPLAY MANAGER:  START APP #%d .......... \n", m_current_running_app );

	// register with DM 
	Rectangle* rect = get_useable_rect();
	if (mApp->m_main_window) {
		//mApp->m_main_window->onCreate();		// this is done inside Application::onCreate() !
		mApp->m_main_window->set_position( rect );		
	}
	mApp->onCreate();
	mApp->register_with_display_manager();
}

void DisplayManager::set_main_window( Control* mNewWindow )
{
	Rectangle* rect = get_useable_rect();
	if (mNewWindow)
		mNewWindow->set_position( rect );
	mNewWindow->onCreate();
	m_running_apps[m_current_running_app]->m_main_window = mNewWindow;
	m_running_apps[m_current_running_app]->register_with_display_manager();
	//mApp->m_main_window = mNewWindow;
	//mApp->register_with_display_manager();
}

void DisplayManager::idle_tasks( )
{
	if ((m_current_running_app>=0) && (m_current_running_app<m_running_apps.size())) 
	{
		//printf("idle task: %d \n", m_current_running_app );
		//m_running_apps[m_current_running_app]->background_time_slice();	
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

float DisplayManager::get_aspect_ratio()
{
	float ratio = screen_width / screen_height;
	return ratio;
}	

Rectangle*	DisplayManager::get_useable_rect( )
{
	static Rectangle rect;
	if (Debug) printf("DisplayManager::get_useable_rect( )\n");

	// TOP : 
	float top2 = m_sb.get_bottom();
	if (Debug)  printf("get_bottom() = %6.1f\n", top2 );
	float top2_ = m_sb.get_bottom_();
	if (Debug)  printf("get_bottom() = %6.1f\n", top2_ );	

	m_sb.print_positions();	
	rect.set_top( top2 );

	// Right Side:	
	if (m_side.is_visible()==true) {
		float l2 = m_side.get_left_()-1; 
		if (Debug)  printf("right side = %6.1f\n", l2 );
		rect.set_right( l2 );
	} else
		rect.set_right( screen_width );
	if (Debug)  printf("right = %6.1f\n", rect.get_right() );
	
	// no task bar yet!
	rect.set_left( 0 );

	// Bottom:
	if (m_status.is_visible()==true)
		rect.set_bottom( m_status.get_top() );
	else if (m_keyboard.is_visible()==true)
		rect.set_bottom( m_keyboard.get_height() );
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
	register_child( &m_sb     );
	register_child( &m_side   );
	register_child( &m_keyboard );
	register_child( &m_calendar );
}

int   DisplayManager::draw(	)
{
	if (Debug2) printf("\n======display manager draw===========\tStart:\n" );
	start_screen();
	if (Debug)  printf("SystemBar: %x\n", &m_sb );
	if (Debug)  printf("Side  Bar: %x\n", &m_side );
	if (Debug)  printf("StatusBar: %x\n", &m_status );
	if (Debug)  printf("Keyboard: %x\t" , &m_keyboard );
	if (m_keyboard.is_visible()) printf("KB Visible!");
	if (Debug)  printf("\n");

	draw_background();
	draw_children();
	end_draw();				// end is needed to see display!
	if (Debug2) printf("======display manager draw===========\tDone!\n\n" );
	return 1;
}

int DisplayManager::any_invalid_children()
{
	vector<Control*>::iterator	iter = m_child_controls.begin();
	for (int i=0; iter!=m_child_controls.end(); i++, iter++ )
	{
		if 	((*iter)->is_invalid())
		{
			printf("invalid child:  %x\n", (*iter) );
			return TRUE;	
		}
	}	
	return FALSE;
}
int	DisplayManager::draw_invalid_children( )
{
	vector<Control*>::iterator	iter = m_child_controls.begin();
	for (int i=0; iter!=m_child_controls.end(); i++, iter++ )
	{
		if 	((*iter)->is_invalid())
			(*iter)->draw();
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
	return 1;
}
void  DisplayManager::end_draw(	)
{
	end_screen();	   // End the picture
}

int   DisplayManager::draw_children( )
{	
	if (Debug) { printf("\t\tside bar	\t");	m_side.print_positions();	}
	if (Debug) { printf("\t\tstatus bar	\t");	m_status.print_positions();	}

	sort_z_order();	
	vector<Control*>::iterator	iter = m_child_controls.begin();
	for (int i=0; iter!=m_child_controls.end(); i++, iter++ )
	{
		if (Debug2) printf("draw child %d %x\n", i, *iter );
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
	Control* retval = Control::HitTest(x,y);
	if (Debug)  printf("DisplayManager::HitTest()   %x \n", retval );
	
	if (retval==this)
		return NULL;	// don't want display manager empty space to do anything.
	else {
		return retval;
	}
}


bool DisplayManager::relay_mouse(   )
{
	if (mouse_capture_control==NULL) return false;
	mouse_capture_control->HitTest(mouse.x, mouse.y);
	return true;	
}
