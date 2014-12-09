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
#include "tab_control.hpp"



TabControl::TabControl() 
:Control()
{
}

TabControl::TabControl( int Left, int Right, int Top, int Bottom ) 
:Control(Left, Right, Top, Bottom)
{ 
}

TabControl::TabControl( int Width, int Height  		 ) 
:Control(Width, Height)
{ 
}

TabControl::~TabControl() 
{ 
}

void 	TabControl::Initialize(	) 
{ 
}

int 	TabControl::calc_metrics() 
{ 
}

int 	TabControl::add_entry (stTabInfo mEntry, Control* mAssociatedPage  ) 
{ 
}

int 	TabControl::set_state ( bool mSelected ) 
{
	m_selection    = mSelected ; 
} 

int 	TabControl::enable_add_button	( bool mEnable 	 ) 
{
	m_has_add_button = mEnable;   
} 

int    	TabControl::draw_curve (	) 
{ 
}		

int    	TabControl::draw_one_tab		(	) 
{ 
}		

int    	TabControl::draw_add_tab 		(	) 
{ 
}

int 	TabControl::draw_scroll_buttons	(	) 
{ 
}

int   	TabControl::draw  	(	) 
{ 
}

int		TabControl::get_selected	 (	 )
{
}				// return currently selected tab.

int		TabControl::select	( int mTabIndex )
{
}


int 	TabControl::get_hit_index		( int Mousex, int Mousey ) 
{ 
}

int		TabControl::onClick(int x, int y, bool mouse_is_down) 
{ 
}




