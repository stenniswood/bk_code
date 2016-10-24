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
#include "draw_canvas.hpp"
#include "CAN_base.h"

/* When the CAN_app is active, change the menu as below */

HorizontalMenu   draw_menu(-1,-1);
VerticalMenu     draw_app_menu (-1,-1);
VerticalMenu     draw_file_menu(-1,-1);
VerticalMenu     draw_view_menu(-1,-1);

Drawing2D*  drawing;


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
{
	Initialize();
}
Drawing2D::Drawing2D( int Width, int Height  ) 
:Window( Width, Height )
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
	show_border();	
	m_is_capturing = false;
}

int		Drawing2D::calc_metrics() 
{ 
}

void  Drawing2D::file_new()
{
	m_points_in_making.clear();	
	m_lines.clear();	
}

// chance to load resources, call functions which use fonts
int		Drawing2D::onCreate(  )
{
	place_views();
	Control::onCreate();
}

void Drawing2D::accept_poly_line( )	// append_new_captured
{
//	m_lines.push_back( m_points );	// accept
//	m_points.clear();					// start over!
}

void accept_line(void* mThis)
{
	void (*callback)(void*);
	Drawing2D* ptr = (Drawing2D*)mThis;
	ptr->accept_poly_line();
}

void	Drawing2D::place_views()
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

int Drawing2D::create_cmd_list ( struct stPath mlines )
{
	int size = m_lines[0].path_points.size();
	
	m_lines[0].commands.push_back( VG_MOVE_TO_ABS );
	for (int i=1; i<size; i++)
		m_lines[0].commands.push_back( VG_LINE_TO_ABS );

}

int Drawing2D::draw_poly_line ( int mIndex )
{
	int size = m_lines[mIndex].path_points.size();
	
	VGint numCmds		= size;
	VGint numCoords 	= size;
	VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD,
								VG_PATH_DATATYPE_F,
								1.0f, 0.0f, 		// scale,bias
								numCmds, numCoords,
								VG_PATH_CAPABILITY_ALL);
		
	VGubyte * commands = m_lines[mIndex].commands.data();			
	struct stPathPoint* coords   = m_lines[mIndex].path_points.data();

	vgAppendPathData(path, numCmds, commands, coords);
	vgDrawPath		(path, VG_STROKE_PATH			);
}

int Drawing2D::draw( )
{  
	int retval = Control::draw();
	for (int i=0; i < m_lines.size(); i++)
	{
		draw_poly_line(i);
	}
	return retval;
}

Control* Drawing2D::HitTest ( int x, int y 	)
{
	Control* retval = Control::HitTest( x,y);
	if (m_is_capturing) {
		Capture(x,y,0);
		printf("captured: %d,%d\n",x,y);
	}
	return retval;
}

int	Drawing2D::onClick(int x, int y, bool mouse_is_down) 
{ 
	//int result =  Control::onClick(x,y,mouse_is_down);
	if (mouse_is_down) {
		if (m_is_capturing) {
			printf("Drawing2D:  Done capturing!\n");
			append_new_captured();		
			m_is_capturing = false;
		} else {
			printf("Drawing2D:  Now capturing!\n");
			m_is_capturing = true;
			init_capture_vectors( 1 );
			Capture(x,y,0);
		}
	} else {
	}
}

void Drawing2D::init_capture_vectors( int mNumberFingersPressed	)
{
	struct stPath empty;
	m_points_in_making.clear();
	for (int i=0; i<mNumberFingersPressed; i++) {
		empty.finger_index = i;
		m_points_in_making.push_back( empty );
	}
}
Control* Drawing2D::Capture ( int x, int y, int finger 	)
{
	struct stPathPoint pt;
	pt.x = x;
	pt.y = y;

	m_points_in_making[finger].commands.push_back   ( VG_MOVE_TO_ABS );
	m_points_in_making[finger].path_points.push_back( pt );
	return NULL;
}
bool Drawing2D::append_new_captured	( )
{
	for (int i=0; i< m_points_in_making.size(); i++)
		m_lines.push_back( m_points_in_making[i] );		
}



void simple_path()
{
	VGubyte * commands = new VGubyte[10];
	VGfloat * coords   = new VGfloat[20];
	VGint numCmds		= 10;
	VGint numCoords 	= 10;
	VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD,
								VG_PATH_DATATYPE_F,
								1.0f, 0.0f, 		// scale,bias
								numCmds, numCoords,
								VG_PATH_CAPABILITY_ALL);

	commands[0] = VG_MOVE_TO_ABS;
	commands[1] = VG_LINE_TO_ABS;
	commands[2] = VG_LINE_TO_ABS;
	commands[3] = VG_LINE_TO_ABS;
	commands[4] = VG_LINE_TO_ABS;
	commands[5] = VG_LINE_TO_ABS;
	commands[6] = VG_LINE_TO_ABS;
	commands[7] = VG_LINE_TO_ABS;
	commands[8] = VG_LINE_TO_ABS;
	commands[9] = VG_LINE_TO_ABS;
	commands[10]= VG_LINE_TO_ABS;
	
	coords[0]   = 12.0;		coords[1] = 24.0;
	coords[2]   = 100.0;	coords[3] = 350.0;
	coords[4]   = 150.0;	coords[5] = 400.0;
	coords[6]   = 200.0;	coords[7] = 500.0;
	coords[8]   = 300.0;	coords[9] = 350.0;
	coords[10]  = 125.0;	coords[11] =200.0;
	coords[12]  = 900.0;	coords[13] =250.0;
	coords[14]  = 1080.0;	coords[15] =200.0;
	coords[16]  = 500.0;	coords[17] =100.0;
	coords[18]  = 12.0;		coords[19] =24.0;

	vgAppendPathData(path, numCmds, commands, coords);
	vgDrawPath		(path, VG_STROKE_PATH			);
}
