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
#include "display.h"
#include "CAN_base.h"

/* When the CAN_app is active, change the menu as below */

HorizontalMenu   can_menu(-1,-1);
VerticalMenu     draw_view_menu(-1,-1);
VerticalMenu     draw_file_menu(-1,-1);

Drawing2D  drawing(1000,600);

void init_draw_app()
{
	printf("\n\ninit_file_browser\n\n");
	Rectangle* rect = MainDisplay.get_useable_rect();
	drawing.move_to      	( rect.get_left(), rect.get_bottom() );
	drawing.set_width_height( rect.get_width(), rect.get_height() );
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &drawing );
	printf("\n\ninit_file_browser done \n\n");
}

int draw_menu_actions( void* menuPtr, int mMenuIndex )
{
/*	switch(mMenuIndex) 
	{
	case 0: show_messages		();		break;
	case 1: show_message_flowchart();	break;
	case 2: show_variable_graph ();		break;
	case 3: show_robot_limbs	();		break;
	case 4: show_button_boards	();		break;
	default: break;
	} */
}

void init_draw_menu()
{
	draw_view_menu.add_simple_command( "Clear Board" 	 );
	draw_view_menu.add_simple_command( "Save" );
	draw_view_menu.add_simple_command( "Graph "		 );
	draw_view_menu.add_simple_command( "Robot Limbs"  );
	draw_view_menu.add_simple_command( "Button boards");

	draw_view_menu.add_callback( 0, show_CAN_screens  );
	draw_view_menu.add_callback( 1, show_CAN_screens  );
	draw_view_menu.add_callback( 2, show_CAN_screens  );
	draw_view_menu.add_callback( 3, show_CAN_screens  );
	draw_view_menu.add_callback( 4, show_CAN_screens  );
}

void init_draw_file_menu()
{
	draw_file_menu.add_simple_command( "Clear Board" 	 );
	draw_file_menu.add_simple_command( "Save" );
	draw_file_menu.add_simple_command( "Graph "		 );
}

void init_main_draw_menu()
{
	draw_menu.add_sub_menu( "File", &draw_file_menu );
	draw_menu.add_entry_text( "Edit" );
	draw_menu.add_sub_menu( "View", &draw_view_menu );
}


Drawing2D::Drawing2D() 
:Control()
{ 
	Initialize();
}
Drawing2D::Drawing2D( Rectangle* mRect ) 
:Control( mRect->get_left(),mRect->get_right(), mRect->get_top(), mRect->get_bottom() )
{
	Initialize(); 
}
Drawing2D::Drawing2D( int Width, int Height  ) 
:Control( Width, Height )
{ 
	Initialize();
}
Drawing2D::~Drawing2D() 
{ 
}
void 	Drawing2D::Initialize(	) 
{ 
	Control::Initialize();
	//init_view_menu();
	//init_main_CAN_menu();
}

int		Drawing2D::calc_metrics() 
{ 
}

// chance to load resources, call functions which use fonts
int		Drawing2D::onCreate(  )
{
	place_views();

	
	Control::onCreate();
}

void Drawing2D::accept_poly_line( )
{
	m_all_lines.push_back( m_points );	// accept
	m_points.clear();					// start over!
}

void accept_line(void* mThis)
{
	void (*callback)(void*);
	Drawing2D* ptr = (Drawing2D*)mThis;
	ptr->accept_poly_line();
}

int		Drawing2D::place_views()
{
	printf("Drawing2D::place_views()\t\t");
	//print_positions();
	
	m_new_poly_line = new Button(-1,-1);
	m_color_chooser = new Button(-1,-1);	
	m_stroke_width  = new Spinner(-1,-1);
	
	m_new_poly_line->set_text("New");
	m_new_poly_line->set_on_click_listener( accept_line, this );					
	
	m_color_chooser->set_text("New");
	m_stroke_width->set_text("Width");
	m_stroke_width->set_max  (50.);
	m_stroke_width->set_min  (1.);
	m_stroke_width->set_value(1.);
	
	register_child( &m_new_poly_line );
	register_child( &m_color_chooser );
	register_child( &m_stroke_width  );
}

int		Drawing2D::handle_incoming_msg	( struct sCAN* msg ) 
{
	//struct sCAN* result = m_filter_view.filter_incoming_msg( msg );
	//if (result)	
		m_msg_view.add_message( msg );
	// test if it's a trigger for another message.
	//     then send response	
}
int		Drawing2D::setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms ) 
{ 
}
int		Drawing2D::setup_triggered_response_msg( struct sCAN* mTriggerMsg, struct sCAN* mResponse ) 
{ 
}
int   	Drawing2D::draw		 		(				) 
{ 
	return Control::draw();
}
int		Drawing2D::get_hit_index		( int Mousex, int Mousey ) 
{ 
}
int		Drawing2D::onClick(int x, int y, bool mouse_is_down) 
{ 
	//int result =  Control::onClick(x,y,mouse_is_down);
	m_points.push_back( x );
	m_points.push_back( y );

	m_all_lines.push_back( m_points );
		
}

