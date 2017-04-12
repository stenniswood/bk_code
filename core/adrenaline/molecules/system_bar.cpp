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
#include "global.h"
#include "stereo_power.hpp"

#include "system_bar.hpp"
#include "client_list_control.hpp"
//#include "home_screen.hpp"
//#include "draw_app.hpp"

#define Debug 0


StereoPowerLevels volume_control;

/*********************** CALL BACKS ****************************/
void show_sidebar(void* mObj )
{
	if (Debug) 	printf("\n\nshow_sidebar\n");
	Control* obj = (Control*) mObj;
	if (obj->is_visible()==true)
		obj->hide( ); 
	else
		obj->show( );
}
void show_wifi(void* mObj )
{
	if (Debug) 	printf("\n\nshow_wifi\n");
	Control* obj = (Control*) mObj;
	if (obj->is_visible()==true)
		obj->hide( ); 
	else
		obj->show( );
}
void show_volume(void* mObj )
{
	Control* obj = (Control*) mObj;
	if (Debug) 	printf("\n\nshow_volume callback\n");
	if (obj->is_visible()==true) {
		obj->hide( ); 
		// System bar should be a benign object, redrawing should be handled
		//at the display manager level.
		//init_home_screen();
	} else {
		printf("\n\nshow_volume callback - Showing\n");
		obj->show( );
		//MainDisplay.remove_all_objects(	);
		//MainDisplay.add_object( obj );	
	}
}
/******************** END CALL BACKS ****************************/

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
	if (Debug) printf("\n\nSystemBar::onPlace() %6.1f,%6.1f\n",  width, height );
	m_MenuEnd_x   = 3.*width/4.;
	m_MenuStart_x = 100.;
	
	if (Debug) 	printf("SystemBar::onPlace():m_Menu=%p\n", m_Menu );
	if (m_Menu)
	{
		m_Menu->set_width_height( (m_MenuEnd_x-m_MenuStart_x),  height );
		m_Menu->move_to         ( (left+m_MenuStart_x),  bottom );
		m_Menu->calc_metrics	(  );
		if (Debug) {
			printf("SystemBar::onPlace:m_Menu:%5.1f ",bottom);
			m_Menu->print_positions( );
		}
	}

	m_wifi.set_width_height  ( 25, height );
	m_volume.set_width_height( 25, height );

	m_wifi.move_to  ( m_MenuEnd_x, 		  bottom );
	m_volume.move_to( m_wifi.get_right(), bottom );	

	m_show_taskbar.set_width_height( 25, height );
	m_show_sidebar.set_width_height( 25, height );
	
	m_show_taskbar.move_to( 0,        bottom);
	m_show_sidebar.move_to( width-25, bottom);

	float quarts   = width / 4.;
	float vc_left  = 3.* quarts;
	volume_control.set_width_height( 75, 150);
	volume_control.move_to( vc_left, bottom-volume_control.get_height() );
	volume_control.set_max  		( 100.0 );
	volume_control.set_min  		(   0.0 );
	volume_control.set_level_left	(  75.0 );
	volume_control.set_level_right	(  75.0 ); 	
}

int   	SystemBar::draw (	) 
{ 
	if (Debug) {
		printf("SystemBar::draw()\n");
		print_positions();
		printf("SystemBar::draw():m_Menu::\t");
		m_Menu->print_positions();
	}	
	return Control::draw();
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
	if (Debug) 	printf("SystemBar::"); 
	if (Debug) 	print_positions();

	onPlace();
}

Control*	SystemBar::HitTest		  ( int x, int y 	)
{
	//Control* result = m_Menu.HitTest( x,y );
	//if (result)
	//	return result;
	Control* retval = Control::HitTest(x,y); 
	//printf("SystemBar::HitTest()= %x\n", retval );
	return retval;
}

int			SystemBar::onHover		  ( int x, int y ) 
{
	return Control::onHover(x,y); 
}

int		SystemBar::onClick(int x, int y, bool mouse_is_down)
{ 
//  Control::onClick(x,y,mouse_is_down);
//	int result = m_Menu.onClick( x,y, mouse_is_down );
//  if (result) return result;	
	return Control::onClick(x,y,mouse_is_down); 		
}

int	SystemBar::onDoubleClick( 		)
{ 
	return 0;
}


void SystemBar::set_menu( HorizontalMenu* mMenu )
{
	if (Debug) 	printf("SystemBar::set_menu ( %p ) old=%p\n", mMenu, m_Menu );
	unregister_child( m_Menu );	
	if (Debug) printf("after removal 1: \n");		
	m_Menu = mMenu;
	register_child( m_Menu );
	if (Debug) 	print_children();	
	onPlace();
}

int	SystemBar::onCreate(  )
{
	// Inflate the menu's:
	if (Debug) printf("\tSystemBar::onCreate()\n" );	
	
	static bool first_time = true;
	if (first_time)
	{	
		//init_system_hmenu(  );	
		//m_Menu
		first_time = false; 	
	};	
	if (Debug) printf("\tSystem Menu:  sysmenu:%p  draw_menu: \n", &m_Menu );
//	m_Menu = &system_hmenu;	

//	m_show_sidebar.set_on_click_listener( show_sidebar, (void*)&(MainDisplay.m_side) );
	//m_show_taskbar.set_on_click_listener( show_taskbar, MainDisplay.m_show_sidebar );
	//m_wifi.set_on_click_listener  ( show_wifi,   (void*)&(MainDisplay.m_wifi  ) );
	m_volume.set_on_click_listener( show_volume, (void*)&(volume_control) );

	//m_child_controls.clear();
	register_child( m_Menu );
	register_child( &m_show_sidebar );
	register_child( &m_show_taskbar );
	register_child( &m_wifi 		);
	register_child( &m_volume 		);
	return Control::onCreate();
}

