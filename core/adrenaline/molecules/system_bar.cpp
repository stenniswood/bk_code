/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <string.h>
#include <fontinfo.h>
#include <shapes.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "icon.hpp"
#include "system_bar.hpp"


SystemBar::SystemBar() 
:Control()
{ 
	Initialize(); 
}

SystemBar::SystemBar( int mScreenWidth, int mScreenHeight )
:Control( mScreenWidth, mScreenHeight )
{ 
	Initialize();
}

SystemBar::~SystemBar			(	) 
{ 
}

void 	SystemBar::Initialize	(	) 
{ 
	Control::Initialize();

}

void	SystemBar::place_items	(	) 
{ 
	//printf("SystemBar::place_items() %6.1f,%6.1f\n",  width, height );
	m_MenuEnd_x   = 3.*width/4.;
	m_MenuStart_x = 100.;
	
	m_Menu.set_width_height( (m_MenuEnd_x-m_MenuStart_x),  get_height() );
	m_Menu.move_to( (left+m_MenuStart_x),  bottom );
	//printf("SystemBar::place_items:m_Menu:");
	//m_Menu.print_positions( );
}

int   	SystemBar::draw 		(	) 
{ 
	//printf("SystemBar::draw()\n");
	print_positions();
	//printf("SystemBar::m_Menu::draw:");
	m_Menu.print_positions();
	m_Menu.draw();
}

void 	SystemBar::set_width_height  	  ( int Width, int Height )
{   
	Control::set_width_height( Width, Height );
	printf("SystemBar::set_width_height:"); 
	print_positions();
	
	place_items();
}

void  	SystemBar::move_to	  		  	  ( float Left,   float  Bottom	 )
{
	Control::move_to(Left,Bottom);
	printf("SystemBar::"); 
	print_positions();

	place_items();
}

Control*	SystemBar::HitTest		  ( int x, int y 	)
{
	Control* result = m_Menu.HitTest( x,y );
	if (result)
		return result;
	return Control::HitTest(x,y); 
}

int			SystemBar::onHover		  (  			 	) 
{
	return Control::onHover(); 
}

int		SystemBar::onClick(int x, int y, bool mouse_is_down) 
{ 
	int result = m_Menu.onClick( x,y, mouse_is_down );
	if (result) return result;
	
	return Control::onClick(x,y); 		
}

int			SystemBar::onDoubleClick  ( 				)
{ 
}