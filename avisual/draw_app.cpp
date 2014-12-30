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

/* When the CAN_app is active, change the menu as below */

HorizontalMenu   draw_menu(-1,-1);
VerticalMenu     draw_app_menu (-1,-1);
VerticalMenu     draw_file_menu(-1,-1);
VerticalMenu     draw_view_menu(-1,-1);

Drawing2D*  drawing;

void init_drawing_app() 
{
	printf("\ninit_drawing_app\n");
	drawing = new Drawing2D( 1000,600 );

	Rectangle* rect = MainDisplay.get_useable_rect();
	drawing->move_to      	 ( rect->get_left(), rect->get_bottom() );
	drawing->set_width_height( rect->get_width(), rect->get_height() );

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( drawing );
	MainDisplay.set_menu( &draw_menu );
	printf("\ninit_drawing_app done \n");
}

int draw_file_menu_actions( void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex) 
	{
	case 0: drawing->file_new	 ();		break;
	/*case 1: file_open	 ();		break;
	case 2: file_save	 ();		break;
	case 3: file_save_as ();		break; */
	default: break;
	} 
}

int draw_view_menu_actions( void* menuPtr, int mMenuIndex )
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

void init_draw_app_menu()
{
	draw_app_menu.add_simple_command( "About"	);
	draw_app_menu.add_simple_command( "Preferences" );
	draw_app_menu.add_simple_command( "testing" );
	draw_app_menu.add_simple_command( "Quit"    );

	draw_app_menu.add_callback( 0, draw_view_menu_actions  );
	draw_app_menu.add_callback( 1, draw_view_menu_actions  );
	draw_app_menu.add_callback( 2, draw_view_menu_actions  );
}

void init_draw_file_menu()
{
	draw_file_menu.add_simple_command( "New"   );
	draw_file_menu.add_simple_command( "Open"  );
	draw_file_menu.add_simple_command( "Save"  );
	draw_file_menu.add_simple_command( "Save as..."	);
	
	draw_file_menu.add_callback( 0, draw_file_menu_actions  );
	draw_file_menu.add_callback( 1, draw_file_menu_actions  );
	draw_file_menu.add_callback( 2, draw_file_menu_actions  );
	draw_file_menu.add_callback( 3, draw_file_menu_actions  );	
}

void init_draw_view_menu()
{
	draw_view_menu.add_simple_command( "Graph "		 );
	draw_view_menu.add_simple_command( "Robot Limbs"  );
	draw_view_menu.add_simple_command( "Button boards");

	draw_view_menu.add_callback( 0, draw_view_menu_actions  );
	draw_view_menu.add_callback( 1, draw_view_menu_actions  );
	draw_view_menu.add_callback( 2, draw_view_menu_actions  );
} 

void init_main_draw_menu()
{
	draw_app_menu.clear_all();
	draw_file_menu.clear_all();
	draw_view_menu.clear_all();
	draw_menu.clear_all();

	init_draw_app_menu();	
	init_draw_file_menu();	
	init_draw_view_menu();


	draw_menu.add_sub_menu  ( "Draw App", &draw_app_menu );
	draw_menu.add_sub_menu  ( "File",     &draw_file_menu );
	draw_menu.add_entry_text( "Edit" );
	draw_menu.add_sub_menu  ( "View",     &draw_view_menu );
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
	init_main_draw_menu();
}

int		Drawing2D::calc_metrics() 
{ 
}

void  Drawing2D::file_new()
{
	m_points.clear();	
	m_all_lines.clear();	
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
	m_stroke_width  = new SpinnerControl(-1,-1);
	
	m_new_poly_line->set_text("New");
	m_new_poly_line->set_on_click_listener( accept_line, this );					
	
	m_color_chooser->set_text("New");
	m_stroke_width->set_text("Width");
	m_stroke_width->set_max  (50.);
	m_stroke_width->set_min  (1.);
	m_stroke_width->set_value(1.);
	
	register_child( m_new_poly_line );
	register_child( m_color_chooser );
	register_child( m_stroke_width  );	
}

int Drawing2D::draw_poly_line ( int mIndex )
{
	int size = m_all_lines[mIndex].size();
	
	VGubyte * commands  = new VGubyte[size];
	//VGfloat * coords    = new VGfloat[size];
	VGint numCmds		= size;
	VGint numCoords 	= size;
	VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD,
								VG_PATH_DATATYPE_F,
								1.0f, 0.0f, 		// scale,bias
								numCmds, numCoords,
								VG_PATH_CAPABILITY_ALL);

	commands[0] = VG_MOVE_TO_ABS;
	for (int i=1; i<size; i++)
		commands[i] = VG_LINE_TO_ABS;
	
	VGfloat * coords = m_all_lines[mIndex].data();
//	coords[0]   = 12.0;		coords[1] = 24.0;
//	coords[2]   = 100.0;	coords[3] = 350.0;

	vgAppendPathData(path, numCmds, commands, coords);
	vgDrawPath		(path, VG_STROKE_PATH			);
	delete commands;
	//delete coords;
}

int Drawing2D::draw( )
{ 
	for (int i=0; i < m_all_lines.size(); i++)
	{
		draw_poly_line(i);
	}		
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

	//m_all_lines.push_back( m_points );
		
}

