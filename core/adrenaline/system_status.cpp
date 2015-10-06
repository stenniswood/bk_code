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
	MainDisplay.m_keyboard.show();
	printf("show_soft_keyboard :: SHOW KEYBOARD\n");
	MainDisplay.m_keyboard.Invalidate();
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
	 
	m_power_button->set_on_click_listener( show_soft_keyboard, this);
	Control::Initialize();
	if (Debug) printf(" SystemStatusBar::Initialize done\n");
}
const int margin = 10;

int  SystemStatusBar::draw 			 (	 ) 
{ 
	Control::draw();
	
	int length = m_text.length();
	float x=left+margin;
	float y=bottom+height-(text_size*1.5);	
	Text( x,y, m_text.c_str(), SerifTypeface,  text_size );
}
void SystemStatusBar::calc_metrics	 (   ) 
{ }

int	SystemStatusBar::onCreate(  )
{
	onPlace();
}

void	SystemStatusBar::onPlace		(   )
{
	m_power_button->set_width_height( 64, height );
	m_power_button->set_text("o");		// get icon!	
	m_power_button->move_to(left, bottom);
	if (Debug) m_power_button->print_positions();
	if (Debug) printf(" SystemStatusBar::place()\n");

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);	
	//asctime(&timeinfo)
	char time[128];
	sprintf(time, "%d-%d-%d  %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	//m_calendar.set_width_height( 64, height );
	m_calendar->set_text			(time);		
	//m_calendar->set_text			("Thu 11:33 am");		// get icon!	
	m_calendar->onCreate();
	m_calendar->set_width_height( m_calendar->get_width(), height);
	float left_anchor = left+ width - m_calendar->get_width();
	m_calendar->move_to(left_anchor, bottom);
	if (Debug) m_calendar->print_positions();
	if (Debug) printf(" SystemStatusBar::place()\n");
	
	register_child( m_power_button );
	register_child( m_calendar ); 
}

int	 SystemStatusBar::add_control	 ( Control* mControl, char* mText ) 
{ 
}

void SystemStatusBar::hide			 ( bool mHide ) 
{ }

void SystemStatusBar::auto_hide		 ( bool mAutoHide ) 
{ }

void SystemStatusBar::set_alignment	 ( byte mAlignment  ) 
{ }

int	SystemStatusBar::onClick(int x, int y, bool mouse_is_down) 
{ }
