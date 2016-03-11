/*************************************************************************
NAME		:  test_layouts.cpp
DESCRIPTION	:  

	This file is used to test the primitive objects in the Adrenaline
	windowing package.

	For combinations of objects see:  test_combo_layouts.cpp
	
DATE	: November 20, 2014
AUTHOR	: Steve Tenniswood
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "display.h"
#include "display_manager.hpp"
#include "frame_window.hpp"
#include "visual_memory.h"
#include "audio_memory.h"
#include "fft.h"
#include "can_app.hpp"
#include "control.hpp"
#include "rectangle.hpp"
#include "line_graph.hpp"
#include "can_sql_logger.hpp"
#include "calendar.hpp"
#include "client_list_control.hpp"
#include "home_screen.hpp"
#include "window_layouts.hpp"

#define Debug 1
 
LineGraph lg;
//Histogram hg;		uses the one in window_layouts.cpp
//FrameWindow graph_win;
FrameWindow dlg;

// DIALOG CHILD CONTROLS:
RadioButton rb_gyro(-1,-1);
RadioButton rb_loadcell(-1,-1);
RadioButton rb_body_position(-1,-1);
RadioButton rb_gps(-1,-1);	
RadioButton rb_reading;	
Calendar start_date;				// Change to date picker
Calendar end_date;
Button Okay;
Button Cancel;
RadioButton rb_report_linegraph (-1,-1);
RadioButton rb_report_histogram (-1,-1);

DataSet ds[8];
struct tm start_time_bd;
struct tm end_time_bd;

SQL_Logger dlog;

void fill_legend_info(int mReadType )
{
	printf("fill_legend_info() %d\n", mReadType); 
	switch(mReadType)
	{
	case 0:	ds[0].name = "tilt";
			ds[1].name = "pitch";
			ds[2].name = "heading";
			break;
	case 1: ds[0].name = "Left TL";
			ds[1].name = "Left TR";
			ds[2].name = "Left HL";
			ds[3].name = "Left HR";
			ds[4].name = "Right TL";
			ds[5].name = "Right TR";
			ds[6].name = "Right HL";
			ds[7].name = "Right HR";						
			break;
	case 2: ds[0].name = "Longitude";
			ds[1].name = "Latitude";
			break;
	default: ds[0].name = "Reading 1";
			ds[1].name = "Reading 2";
			ds[2].name = "Reading 3";
			ds[3].name = "Reading 4";
			ds[4].name = "Reading 5";
			ds[5].name = "Reading 6";
			ds[6].name = "Reading 7";
			ds[7].name = "Reading 8";	
			break;
	};
	printf("fill_legend_info() done\n");
}

void fill_graph_with_data(string mread_type)
{
	// MAKE A LEGEND:
	int selected_index    = rb_gyro.get_selected_index();	// traverses the whole group
	fill_legend_info(selected_index);		
	int counter = 0;
	
	// SEARCH SQL:
	dlog.find_reading( mread_type, start_time_bd, end_time_bd );		
	dlog.goto_first_row();
	
	// CONSTRUCT DATA SERIES:
	while (dlog.goto_next_row())
	{
		dlog.extract_result();
		for (int d=0; d<8; d++)
			ds[d].add( dlog.readings[d] ); 
		counter++;
	}	
	printf("fill_graph_with_data()=%d\n", counter);

}
void reset_all_data()
{
	lg.reset_data();
	for (int i=0; i<8; i++)
		ds[i].reset();
}
void setup_histogram(string read_type, Rectangle* rect)
{
	string yLabel;	
	hg.set_text  	 	( read_type.c_str(), "time" );
	hg.move_to			( rect->get_left(), rect->get_bottom() );
	hg.set_width_height	( rect->get_width(), rect->get_height() );

	fill_graph_with_data(read_type);
	// ADD DATA SERIES TO GRAPH :
	for (int d=0; d<8; d++)	
		hg.add_data_series( &(ds[d]) );

	//hg.set_auto_scale( true );
	hg.calc_scale();
	hg.Invalidate();
}
void setup_linegraph(string read_type, Rectangle* rect)
{
	string yLabel;	
	lg.set_text  	 	( read_type.c_str(), "time" );
	lg.move_to			( rect->get_left(), rect->get_bottom() );
	lg.set_width_height	( rect->get_width(), rect->get_height() );

	fill_graph_with_data(read_type);
	// ADD DATA SERIES TO GRAPH :
	for (int d=0; d<8; d++)	
		lg.add_data_series( &(ds[d]) );

	lg.set_auto_scale( true );
	lg.calc_scale();
	printf("okay_callback() calc_scale done\n" );		
	lg.Invalidate();

}
void okay_callback( void* mPtr )
{
	dprintf("okay_callback() \n" ); 
	reset_all_data();
	// grab the info:
	// Which radio button selected?
	int report_index = rb_report_linegraph.get_selected_index();
	//RadioButton* report_selected = rb_report_linegraph.get_selected();			// traverses the whole group

	RadioButton* selected = rb_gyro.get_selected();			// traverses the whole group
	string read_type = selected->get_text();
	dprintf(" %s\n", read_type.c_str()); 
		
	// Start date?
	// end date?
	start_time_bd.tm_mon  = 2;
	start_time_bd.tm_mday = 1;
	start_time_bd.tm_year = 2016-1900;
	
	end_time_bd.tm_mon  = 3;
	end_time_bd.tm_mday = 30;
	end_time_bd.tm_year = 2016-1900;	

	// Layout the Graph:
	Rectangle* rect = MainDisplay.get_useable_rect( );
	rect->shrink(0.1);	
	if (Debug) { printf("okay_callback() rect=\n" );	rect->print_positions();	}

	MainDisplay.remove_all_objects(	);	
	switch (report_index)
	{
	case 0 : setup_linegraph(read_type, rect);
			MainDisplay.add_object( &lg );
			break;
	case 1 : setup_histogram(read_type, rect);
			MainDisplay.add_object( &hg );
			break;
	default:
			break;	
	}
	dprintf("okay_callback() done\n");
}

void cancel_callback( void* mPtr )
{
	init_home_screen();
	dprintf("cancel_callback():\n");
}

void init_datalog_graph()
{
	static bool first_time=true;
	if (first_time) {
		first_time=false;
		Cancel.set_text("Cancel", true );
		Okay.set_text  ("Okay",   true );
		Okay.set_on_click_listener  ( okay_callback, NULL   );
		Cancel.set_on_click_listener( cancel_callback, NULL );	

		rb_gyro.set_text		 ("Gyro", 			true);
		rb_loadcell.set_text	 ("Load Cell", 		true);
		rb_body_position.set_text("Body Positions", true);
		rb_gps.set_text			 ("GPS", 			true);
		rb_reading.set_text		 ("General Readings", true);	
		rb_gyro.join_group	( &rb_loadcell );
		rb_gyro.join_group	( &rb_body_position );
		rb_gyro.join_group	( &rb_gps );
		rb_gyro.join_group	( &rb_reading );	
	
		rb_report_linegraph.set_text( "Line Graph", true);
		rb_report_histogram.set_text( "Histogram", true);
		rb_report_linegraph.join_group	( &rb_report_histogram );	
		
		
		dlg.add_control( &Okay ); 
		dlg.add_control( &Cancel );	
		dlg.add_control( &rb_gyro );
		dlg.add_control( &rb_loadcell );
		dlg.add_control( &rb_body_position );
		dlg.add_control( &rb_gps );
		dlg.add_control( &rb_reading );	
		
		dlg.add_control( &rb_report_linegraph );	
		dlg.add_control( &rb_report_histogram );			
		//dlg.add_object( &start_date );
		//dlg.add_object( &end_date );			

		Rectangle* rect = MainDisplay.get_useable_rect( );
		rect->shrink(0.3);
		rect->print_positions();
		
		float w = rect->get_width();
		dlg.set_width_height( rect->get_width(), rect->get_height() );
		dlg.move_to ( rect->get_left(), rect->get_bottom() );

		float padding = 12 * 1.3;
		rb_gyro.move_to 		 			( 250, 300 );
		rb_loadcell.set_position_below		( &rb_gyro, 	true, padding );
		rb_body_position.set_position_below ( &rb_loadcell, true, padding );
		rb_gps.set_position_below			( &rb_body_position, true, padding );	
		rb_reading.set_position_below		( &rb_gps, 		true, padding );			
		rb_loadcell.select();
			
		float left = rect->get_left() + 2*rect->get_width() / 3.;
		rb_report_linegraph.move_to 		  ( left, 300 );
		rb_report_histogram.set_position_below( &rb_report_linegraph, true, padding );
		rb_report_linegraph.select();
		
		Cancel.move_to ( rect->get_left()+10, rect->get_bottom()+10 );
		Okay.move_to ( rect->get_right()-Okay.get_width()-20, rect->get_bottom()+10 );		
	}
	dprintf( "graph_info_dialogbox()\n\n" );
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &dlg );
	dprintf( "graph_info_dialogbox() DONE.\n\n" );
}




class GridFrameWindow : public FrameWindow
{
public:
	GridFrameWindow();
	void			setupgrid();
	virtual int		draw();
	virtual int			onClick		  ( int x, int y, bool mouse_is_down=true );	
	Grid grid;
};
GridFrameWindow::GridFrameWindow()
{
}
void GridFrameWindow::setupgrid() 
{
	Rectangle* rect = MainDisplay.get_useable_rect( );
	rect->shrink(0.25);
	grid.set_boundary ( rect->get_left(), rect->get_right(), rect->get_top()-50, rect->get_bottom() );
	grid.create( 10, 10 );
	grid.draw();
}
int GridFrameWindow::draw()
{
	FrameWindow::draw();
	grid.draw();	
}

int	GridFrameWindow::onClick( int x, int y, bool mouse_is_down=true )
{
	grid.hit_test( x,y );
}

GridFrameWindow grid_test;
void init_grid_test()
{
	Rectangle* rect = MainDisplay.get_useable_rect( );
	rect->shrink(0.2);
	
	float w = rect->get_width();
	grid_test.set_width_height( rect->get_width(), rect->get_height() );
	grid_test.move_to 		  ( rect->get_left(), rect->get_bottom() );
	grid_test.setupgrid();
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &grid_test );	
}


