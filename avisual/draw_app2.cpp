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
#include <ctype.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <fontinfo.h>
#include <shapes.h>
#include "Graphbase.hpp"
#include "adrenaline_windows.h"
#include "draw_app.hpp"
#include "CAN_base.h"



VerticalMenu     draw_app_menu (-1,-1);
VerticalMenu     draw_file_menu(-1,-1);
VerticalMenu     draw_view_menu(-1,-1);

DrawingApp*  drawing;

int draw_app_menu_actions( void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex) 
	{
	case 0: drawing->About();			break;
	case 4: show_button_boards	();		break;
	default: break;
	} 
}

int draw_file_menu_actions( void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex) 
	{
	case 0: drawing->file_new	 ();		break;
	default: break;
	} 
}

int draw_view_menu_actions( void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex) 
	{
	case 0: show_messages		();		break;
	case 4: show_button_boards	();		break;
	default: break;
	} 
}


void init_draw_app_menu()
{
	draw_app_menu.add_simple_command( "About"		);
	draw_app_menu.add_simple_command( "Preferences" );
	draw_app_menu.add_simple_command( "Quit"    	);
	draw_app_menu.add_callback_all_items( 0, draw_app_menu_actions  );
}

void init_draw_view_menu()
{
	draw_view_menu.add_simple_command( "Graph "		 );
	draw_view_menu.add_simple_command( "Robot Limbs"  );
	draw_view_menu.add_simple_command( "Button boards");
	draw_view_menu.add_callback_all_items( draw_view_menu_actions  );
} 


DrawApp::DrawApp () 
{ 
}
DrawApp::DrawApp ( Rectangle* mRect ) 
{ 
}
DrawApp::~DrawApp() 
{ 
}

void 	DrawApp::Initialize		(	) 
{ 
}	// create all the objects here.

int				DrawApp::onPlace			(	) 
{ 
}

void	DrawApp::setup_menu  	( ) 
{ 
	draw_app_menu.clear_all();
	draw_view_menu.clear_all();

	draw_file_menu.create_std_file_menu();
	draw_file_menu.add_callback_all_items( draw_file_menu_actions );

	init_draw_app_menu ();
	init_draw_view_menu();

	m_hMenu.clear_all();
	m_hMenu.add_sub_menu  ( "Draw App", &draw_app_menu  );
	m_hMenu.add_sub_menu  ( "File",     &draw_file_menu );
	m_hMenu.add_entry_text( "Edit" );
	m_hMenu.add_sub_menu  ( "View",     &draw_view_menu );
}

void	DrawApp::setup_app_menu  ( ) 
{ 
}
void 	DrawApp::register_with_display_manager() 
{ 
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( drawing );
	MainDisplay.set_menu  	( &m_Hmenu );
}	

int				DrawApp::About			(	) 
{ 
}
int				DrawApp::Preferences		(	) 
{ 
}

void	DrawApp::file_new		( )
{ 
}
void	DrawApp::file_open		( ) 
{ 
}	
void	DrawApp::file_open_recent( ) 
{ 
}
	
void	DrawApp::file_save		( ) 
{ 
}
void	DrawApp::file_save_as	( ) 
{ 
}

void 			DrawApp::draw_play		( ) 
{ 
}
void 			DrawApp::configure_wave_views(int mChannels, short* mDataCh1, short* mDataCh2 ) 
{ 
}

void 			DrawApp::zoom_in			() 
{ 
}
void 			DrawApp::zoom_out		() 
{ 
}
void 			DrawApp::show_mixer		() 
{ 
}
void 			DrawApp::show_frequency	() 
{ 
}
void 			DrawApp::show_fft  		() 
{ 
}


