/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <time.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "adrenaline_windows.h"
#include "display.h"
#include "system_status.hpp"
#include "keyboard.hpp"

#define Debug 0


void show_soft_keyboard( void* mSystemStatusBar )
{
	MainDisplay.show_keyboard();
	printf("show_soft_keyboard :: SHOW KEYBOARD  %x  %x \n", mSystemStatusBar,((SystemStatusBar*)mSystemStatusBar)->m_power_button );
//	if (((SystemStatusBar*)mSystemStatusBar)->invalidated)
//		printf("INVALID!\n");		
	//MainDisplay.m_keyboard.Invalidate();
}

void show_calendar( void* mSystemStatusBar )
{
	static bool is_visible = false;
	if (is_visible==false)
		MainDisplay.show_calendar();
	else 
		MainDisplay.hide_calendar();
	printf("show_calendar :: SHOW CALENDAR\n");	
}


SystemStatusBar::SystemStatusBar ( ) 
:Control( )
{ 
	Initialize();
}

SystemStatusBar::SystemStatusBar ( int Left, int Right, int Top, int Bottom ) 
:Control( Left, Right, Top, Bottom )
{
	Initialize(); 
}

SystemStatusBar::~SystemStatusBar( ) 
{ 
	Initialize();
}

void SystemStatusBar::Initialize	 (   ) 
{ 
	if (Debug) printf(" SystemStatusBar::Initialize \n");
	m_power_button = new Button(-1,-1);
	m_calendar     = new Button(-1,-1);	 
	m_power_button->hot_touch = true;
	m_calendar->hot_touch 	  = true;
	
	m_power_button->set_on_click_listener( show_soft_keyboard, this);
	m_calendar->set_on_click_listener( show_calendar, this);
	
	Control::Initialize();
	if (Debug) printf(" SystemStatusBar::Initialize done\n");
}
const int margin = 10;

int  SystemStatusBar::draw 			 (	 ) 
{ 
	Control::draw();
	
/*	int length = m_text.length();
	float x=left+margin;
	float y=bottom+height-(text_size*1.5);	
	Text( x,y, m_text.c_str(), SerifTypeface,  text_size ); */
}
void SystemStatusBar::calc_metrics	 (   ) 
{ }

int	SystemStatusBar::onCreate(  )
{
	onPlace();
	return Control::onCreate();
}

void SystemStatusBar::set_text( const char* mText ) 
{
	m_text_view.set_text( mText ); 
}
	
void SystemStatusBar::onPlace(   )
{
	// POWER BUTTON
	m_power_button->set_width_height( height, height );
	m_power_button->set_text("k");		// get icon!	
	m_power_button->move_to(left, bottom);
	if (Debug) m_power_button->print_positions();
	if (Debug) printf(" SystemStatusBar::place()\n");
	float status_left = m_power_button->get_right();
	
	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);	
	//asctime(&timeinfo)
	char time[128];
	sprintf(time, "%d-%d-%d  %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	// CALENDAR BUTTON
	m_calendar->set_text			(time, true);	//("Thu 11:33 am");
	m_calendar->onCreate();
	m_calendar->set_width_height( m_calendar->get_width(), height);
	float left_anchor = left+ width - m_calendar->get_width();
	m_calendar->move_to(left_anchor, bottom);
	if (Debug) m_calendar->print_positions();
	if (Debug) printf(" SystemStatusBar::place()\n");
	float status_right = m_calendar->get_left();
	
	m_text_view.set_width_height( status_right-status_left, height );
	m_text_view.move_to			( status_left, bottom);	
	m_text_view.show_border		( false );
	m_text_view.set_background_color( background_color );
	m_text_view.set_text_color( 0xFFCFCF00 );
	
	register_child( m_power_button );
	register_child( m_calendar ); 
	register_child( &m_text_view ); 
}

/*int	 SystemStatusBar::add_control	 ( Control* mControl, char* mText ) 
{ 	Want this to do some auto packing.  But right now no need for it!
	register_child( &m_text_view ); 
}*/

void SystemStatusBar::auto_hide		 ( bool mAutoHide ) 
{ }

void SystemStatusBar::set_alignment	 ( byte mAlignment  ) 
{ }

int	SystemStatusBar::onClick(int x, int y, bool mouse_is_down) 
{
	return Control::onClick( x,  y, mouse_is_down );
 }
