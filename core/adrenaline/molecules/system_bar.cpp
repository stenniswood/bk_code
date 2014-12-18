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
#include <string>
#include <fontinfo.h>
#include <shapes.h>
#include "Graphbase.hpp"
#include "adrenaline_windows.h"
#include "system_bar.hpp"
#include "avisual_menu.hpp"



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

void	SystemBar::onPlace		(	)
{ 
	printf("\n\nSystemBar::onPlace() %6.1f,%6.1f\n",  width, height );
	m_MenuEnd_x   = 3.*width/4.;
	m_MenuStart_x = 100.;
	
	printf("SystemBar::onPlace():m_Menu=%x", m_Menu);	
	if (m_Menu)
	{

		m_Menu->set_width_height( (m_MenuEnd_x-m_MenuStart_x),  get_height() );
		m_Menu->move_to         ( (left+m_MenuStart_x),  bottom );
		m_Menu->calc_metrics	();
		printf("SystemBar::onPlace:m_Menu:");
		m_Menu->print_positions( );
	}
	
	m_show_taskbar.set_width_height( 25, height );
	m_show_sidebar.set_width_height( 25, height );
	
	m_show_taskbar.move_to( 0,        bottom);
	m_show_sidebar.move_to( width-25, bottom);
}

int   	SystemBar::draw (	) 
{ 
	printf("SystemBar::draw()\n");
	print_positions();
	printf("SystemBar::m_Menu::\t");
	m_Menu->print_positions();
		
	//printf("SystemBar::m_Menu::draw:");
	//m_Menu.draw();
	Control::draw();
}

void 	SystemBar::set_width_height  	  ( int Width, int Height )
{   
	Control::set_width_height( Width, Height );
	//printf("SystemBar::set_width_height:");
	//print_positions();
	onPlace();
}

void  	SystemBar::move_to	  		  	  ( float Left,   float  Bottom	 )
{
	Control::move_to(Left,Bottom);
	printf("SystemBar::"); 
	print_positions();

	onPlace();
}

Control*	SystemBar::HitTest		  ( int x, int y 	)
{
	//Control* result = m_Menu.HitTest( x,y );
	//if (result)
	//	return result;
	return Control::HitTest(x,y); 
}

int			SystemBar::onHover		  (  			 	) 
{
	return Control::onHover(); 
}

int		SystemBar::onClick(int x, int y, bool mouse_is_down)
{ 
//  Control::onClick(x,y,mouse_is_down);
//	int result = m_Menu.onClick( x,y, mouse_is_down );
//  if (result) return result;	
	return Control::onClick(x,y,mouse_is_down); 		
}

int			SystemBar::onDoubleClick  ( 				)
{ 
}

void show_sidebar(void* mObj )
{
	printf("\n\nshow_sidebar\n");
	Control* obj = (Control*) mObj;
	if (obj->is_visible()==true)
		obj->hide( ); 
	else
		obj->show( );
}

int	SystemBar::onCreate(  )
{
	// Inflate the menu's:
	static bool first_time = true;
	if (first_time)
	{	init_system_hmenu(  );	first_time = false; 	};	
	m_Menu = &system_hmenu;	

	m_show_sidebar.set_on_click_listener( show_sidebar, (void*)&(MainDisplay.m_soft_side) );
	//m_show_taskbar.set_on_click_listener( show_taskbar, MainDisplay.m_show_sidebar );

	register_child( m_Menu );	
	register_child( &m_show_sidebar );
	register_child( &m_show_taskbar );
}

