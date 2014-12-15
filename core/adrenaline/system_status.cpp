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
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "adrenaline_windows.h"
#include "display.h"
#include "system_status.hpp"




SystemStatusBar::SystemStatusBar ( ) 
:Window( )
{ 
	Initialize();
}

SystemStatusBar::SystemStatusBar ( int Left, int Right, int Top, int Bottom ) 
:Window( Left, Right, Top, Bottom )
{
	Initialize(); 
}

SystemStatusBar::~SystemStatusBar( ) 
{ 
	Initialize();
}

void SystemStatusBar::Initialize	 (   ) 
{ 
	Window::Initialize();
}
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

int	 SystemStatusBar::add_control	 ( Control* mControl, char* mText=NULL ) 
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
