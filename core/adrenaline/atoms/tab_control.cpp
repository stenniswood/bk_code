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
	strcpy (class_name, "TabControl");					
}

int 	TabControl::calc_metrics() 
{ return 0;
}

int 	TabControl::add_entry (stTabInfo mEntry, Control* mAssociatedPage  ) 
{ return 0;
}

int 	TabControl::set_state ( bool mSelected ) 
{
	m_selection    = mSelected ; 
	return 0;
} 

int 	TabControl::enable_add_button	( bool mEnable 	 ) 
{
	m_has_add_button = mEnable;   
	return 0;	
} 

int    	TabControl::draw_curve (	) 
{ 
return 0;
}		

int    	TabControl::draw_one_tab		(	) 
{ return 0;
}		

int    	TabControl::draw_add_tab 		(	) 
{ return 0;
}

int 	TabControl::draw_scroll_buttons	(	) 
{ return 0;
}

int   	TabControl::draw  	(	) 
{ return 0;
}

int		TabControl::get_selected	 (	 )
{return 0;
}				// return currently selected tab.

int		TabControl::select	( int mTabIndex )
{return 0;
}


int 	TabControl::get_hit_index		( int Mousex, int Mousey ) 
{ return 0;
}

int		TabControl::onClick(int x, int y, bool mouse_is_down) 
{ return 0;
}




