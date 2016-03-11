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
#include <vector>
#include "power_level.hpp"
#include "stereo_power.hpp"
#include "visual_memory.h"
#include "audio_memory.h"
#include "fft.h"
#include "audio_app.hpp"
#include "can_app.hpp"
#include "card_games.h"
#include "GyroViewI.hpp"

#include "load_cell_view.hpp"
#include "test_layouts.hpp"
#include "test_combo_layouts.hpp"
#include "test_game_layouts.hpp"
#include "test_graph_layouts.hpp"
//#include "can_msg_view.hpp"
#include "window_layouts.hpp"
 

#define Debug 0

// L, R, T, B
static TextView 		LongText		( 50, 700, 500, 50 );
static TabularListBox	tab_lb			( 600, 900, 400, 50  );

/********************* A sample Window ***********************************/
static FrameWindow	ParentWindowFrame(450, 1050, 500, 100);
static Window		ParentWindowF(450, 1050, 500, 100);

static TextView 	SampleText;
static ListBox  	AvailableClients;
static IconView		test_image;
static IconView		test_icon ( 50,200 ); 
/*************************************************************************/
static PowerLevel		 pl		(-1,-1);
static StereoPowerLevels spl  	(-1,-1);
static RadioButton 	MyRadio1 ( -1, -1 );
static RadioButton 	MyRadio2 ( -1, -1 );
static RadioButton 	MyRadio3 ( -1, -1 );
static RadioButton 	MyRadio4 ( -1, -1 );

static vector<DirectoryListBox>  dir_lb;
static char 		ConnectionStatusText[128];
//ButtonArrayMot MyArray	( 700, 1070, 350, 100);



void init_simple_button_test()
{
	/* Demonstrate different styles.  Image button? */
	MyButt.set_text( "Push me" );
	MyButt.set_state(true);
	MyButt.move_to ( 0.5*MainDisplay.screen_width, 200 );	
	if (Debug) MyButt.print_positions( );
	
	Button* test = new Button( -1, -1 ); 
	test->set_text( "Try me and see", true );
	test->set_position_right_of( &MyButt );

	// PowerLevel
	pl.move_to  		(100, 200 );
	pl.set_width_height ( 75, 150 );
	pl.set_max  		( 100.0   );
	pl.set_min  		(   0.0   );
	pl.set_level		(  75.0   );

	// Stereo Power Indicator	
	spl.set_position_right_of( &pl );
	spl.set_width_height( 75, 150);
	spl.set_max  		( 100.0 );
	spl.set_min  		(   0.0 );
	spl.set_level_left	(  75.0 );
	spl.set_level_right	(  75.0 ); 
	
	// Add to display manager:
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &MyButt );	
	MainDisplay.add_object( test );		
	MainDisplay.add_object( &pl  );
	MainDisplay.add_object( &spl );	
}


void init_textview_test()
{
	strcpy(ConnectionStatusText, "A moment of kindness between two animals was captured \
by a Tampa police officer just before a colleague rescued a stranded dog from the \
Hillsborough River. Residents in Tampas Seminole Heights neighborhood heard a strange \
noise coming from the river " );	
	// this past Friday night and then saw the dog, named White Boy, struggling to climb out of the river the next morning.
	LongText.set_width_height		( 0.75 * MainDisplay.screen_width, 0.5* MainDisplay.screen_height );
	LongText.set_text				( ConnectionStatusText );
	LongText.set_text_size			( 15.0		 );
	LongText.set_text_color			( 0xFFFF0000 );
	LongText.set_background_color	( 0xFFFfFf00 );
	LongText.center_vertical		( TRUE		 );
	LongText.center_horizontal		( TRUE		 );	

	float bottom = 0.5* MainDisplay.screen_height;
	TextView* leftText = new TextView( 50, 0.5 * MainDisplay.screen_width, bottom+150, bottom );
	leftText->set_alignment_horizontal( TEXTVIEW_ALIGN_LEFT );
	leftText->set_text("Left adjusted default."); 
	
	TextView* rightText = new TextView( 0.5*MainDisplay.screen_width, MainDisplay.screen_width-10, bottom+150, bottom );
	rightText->set_text("Right adjusted text is normal.");
	rightText->set_alignment_horizontal( TEXTVIEW_ALIGN_RIGHT );

	// Add to display manager:
	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &LongText );
	MainDisplay.add_object( leftText );	
	MainDisplay.add_object( rightText );		
}

void init_progressbar_test()
{
	MyProgress.set_percentage(100.0);	
	MyProgress.move_to(100, 200);	
	MyProgress.show_percentage();
	MyProgress.print_positions();
	
	ProgressBar* tmp = new ProgressBar();
	tmp->set_percentage	(50.0);
	tmp->copy_position_horiz( &MyProgress );
	//tmp->show_percentage();
	tmp->set_position_above ( &MyProgress );

	ProgressBar* tmp2 = new ProgressBar();
	tmp2->set_position_above ( tmp );
	tmp2->copy_position_horiz( &MyProgress );
	tmp2->set_percentage(20.0);

	// Add to display manager:
	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &MyProgress );
	MainDisplay.add_object( tmp		    );	
	MainDisplay.add_object( tmp2	    );	
}

void init_radio_button_test()
{
	MyRadio1.move_to ( 200, 300 ); 
	MyRadio1.set_text("AM Radio");
	MyRadio2.set_text("FM Radio");
	MyRadio3.set_text("XM Radio", true);
	MyRadio4.set_text("Internet Radio", true);

	MyRadio2.set_position_below( &MyRadio1, true );
	MyRadio3.set_position_below( &MyRadio2, true );
	MyRadio4.set_position_below( &MyRadio3, true );

	MyRadio1.join_group( &MyRadio2 );
	MyRadio1.join_group( &MyRadio3 );
	MyRadio1.join_group( &MyRadio4 );
	MyRadio3.select();
	MyRadio2.select();
		
	//MyRadio1.onClick(5,5);

	// Need a group box:
	// Add to display manager:
	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &MyRadio1 );
	MainDisplay.add_object( &MyRadio2 );
	MainDisplay.add_object( &MyRadio3 );
	MainDisplay.add_object( &MyRadio4 );
}

CheckBox check1(-1, -1);
CheckBox check2(-1, -1);

void sample_callback( void* mPtr )
{
	TextView* text = (TextView*)mPtr;
	static int count = 0;
	char str[80];
	sprintf(str, "Thanks for checking me! %d", count++);
	text->set_text(str, true);
}

void init_check_button_test()
{
	float ty =  2*MainDisplay.screen_height/3;
	TextView* text = new TextView();
	text->set_text("waiting for callback...", true);	
	text->move_to( 10, ty);
	
	check1.move_to(50, 250);

	check1.set_text("Select me");
	check2.set_text("Pick me too! ");
	check1.set_on_click_listener( sample_callback, text );
	
	check1.set_check();

	// Aha architecture problem:  height of check1 is not determined until onCreate()
	// yet here we need it to place check2.  
	// Think of how to best do this after lunch.	
	//check1.copy_position_horiz( &check2 );
	
	// Add to display manager:
	MainDisplay.remove_all_objects(		);	
	MainDisplay.add_object( &check1 );
	MainDisplay.add_object( &check2 );	
	MainDisplay.add_object( text );
	check2.set_position_below( &check1, true, 50. );
}


void init_simple_text()
{
	MainDisplay.init_screen();
	MainDisplay.start_screen();

	printf("Simple text\n");
	float x = 50;
	float y = 450;

	vgSetf(VG_STROKE_LINE_WIDTH, 1.0);
	vgSeti(VG_STROKE_CAP_STYLE,  VG_CAP_BUTT);
	vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_MITER);
	//StrokeWidth	( 1.0 );
	Stroke_l	( 0xFFFFFF00 );
	Fill_l		( 0xFF0000FF );
	Text( x, y, "Sample Large thin", SerifTypeface, 64 );
	
	// To Answer the question - is there any fill in a font:
	y = 200;
	StrokeWidth	( 40.0 		 );
	Stroke_l	( 0xFFFFFF00 );
	Fill_l		( 0xFF0000FF );
	Text( x, y, "Sample Large thick", SerifTypeface, 64 );
	
	MainDisplay.end_screen();

	// Add to display manager:
	//while (1) {  };
}

void init_simple_path()
{
	MainDisplay.init_screen();
	MainDisplay.start_screen();
	printf("Simple VG path example\n");
	
	// To Answer the question - is there any fill in a font:
	StrokeWidth	( 1.0 		 );
	Stroke_l	( 0xFFFF00FF );
	Fill_l		( 0xFF0000FF );
	//Text( x, y, "Sample Large thick", SerifTypeface, 42 );
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
	MainDisplay.end_screen();

	// Add to display manager:
	//while (1) {  };
}

void populate_simple_lb()
{
	AvailableClients.set_item(" Android Phone" );
	AvailableClients.set_item(" Robot One"  );	
	AvailableClients.set_item(" TV   " );
	AvailableClients.set_item(" RPI1 " );
	AvailableClients.set_item(" RPI2 " );
	AvailableClients.set_item(" RPI3 " );
	AvailableClients.set_item(" RPI4 " );
	AvailableClients.set_item(" RPI5 " );
	AvailableClients.set_item(" MAC1 " );
	AvailableClients.set_item(" MAC2 " );		
	AvailableClients.set_item(" MAC3 " );		
} 

void pack_sample_window()
{
	char* str = new char[255];
	strcpy (str, "Hello Dialog box!!");

	MyButt.set_text( "Push me" );
	MyProgress.set_percentage(80.0);

	AvailableClients.set_top_down	( false );
	AvailableClients.set_width_height( 200, 200 );
	AvailableClients.adjust_height_for_num_visible_items( 9 );
	populate_simple_lb				(		);
	AvailableClients.set_text_size	( 18.0  );

	Rectangle* rect = MainDisplay.get_useable_rect( );
	ParentWindowF.set_position( rect );
	if (Debug) ParentWindowF.print_positions	( );
	ParentWindowF.pack_control		( &MyButt, PACK_FILL_PARENT, PACKER_ALIGN_TOP  	);
	ParentWindowF.pack_below        ( &AvailableClients, &MyButt, PACK_FILL_PARENT  );
	ParentWindowF.pack_control		( &MyProgress, PACK_LEFT, PACKER_ALIGN_BOTTOM	);
	
//	ParentWindow.pack_control		( &SampleText, PACK_FILL_PARENT );
	test_image.set_file( (char*)"./resources/desert0.jpg" );
	test_icon.set_file ( (char*)"./resources/folder.jpg"  );

	//test_image.set_position_right_of( &AvailableClients );
	//ParentWindow.pack_control		( &test_image, PACK_FILL_PARENT );

	// Add to display manager:
	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &ParentWindowF );
	AvailableClients.print_info();
}

void init_frame_window()
{
	Rectangle* rect = MainDisplay.get_useable_rect( );
	ParentWindowFrame.move_to		  ( rect->get_left(), rect->get_bottom()  );
	ParentWindowFrame.set_width_height( rect->get_width()/2, rect->get_height()/2 );
	
	if (Debug) ParentWindowFrame.print_positions();
		
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &ParentWindowFrame );
}

char textfilename[] = "Readme.txt"; 
TextView tf;

void init_textfile_view()
{
	Rectangle* rect = MainDisplay.get_useable_rect( );
		
	tf.set_width_height	( rect->get_width(), rect->get_height() );
	tf.move_to  		( rect->get_left(), rect->get_bottom() );	
	tf.load_file		( textfilename );
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &tf );		
}

static HorizontalMenu hm(-1,-1);
VerticalMenu   vm   	(-1,-1);
VerticalMenu   simple   (-1,-1);
//VerticalMenu   molecules(-1,-1);
static VerticalMenu   games	(-1,-1);

int menu_callback(void* menuPtr, int mMenuIndex, Application* mApp )
{
	printf("menu_callback( %4x, %d )\n", menuPtr, mMenuIndex );
//	load_test_screen( mMenuIndex );
}

void init_horiz_menu		()
{
	//stHorizMenuInfo ptr = new struct stHorizMenuInfo;
	//ptr->text = "File";	
	hm.set_width_height(400, 30);
	hm.move_to( 100, 700 );
	hm.add_entry_text( "File" );
	hm.add_entry_text( "Edit" );	
	hm.add_entry_text( "View" );	
	hm.add_entry_text( "Window" );
	if (Debug) hm.print_positions( );
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &hm );
}
void init_vert_menu			()
{
	printf("init_vert_menu\n");	
	vm.set_width_height	( 200, 200 );
	vm.move_to			( 100, 500 );
	vm.add_simple_command( "File Browser" 	);
	vm.add_simple_command( "Audio Amp" 		);	
	vm.add_simple_command( "Black Jack" 	);		
	vm.add_simple_command( "Hearts" 		);			
	vm.add_simple_command( "Image Gallery"	);
	vm.calc_metrics();
	
	if (Debug) printf("init_vert_menu\n");
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &vm );
}

void init_combo_menu()
{

	//simple.move_to			 ( 100, 500 		);	
	simple.add_simple_command( "init_simple_button_test" );
	simple.add_simple_command( "init_gyro_view" );
	simple.add_simple_command( "init_textview_test"		);
	simple.add_simple_command( "init_progressbar_test"  );
	simple.add_simple_command( "init_radio_button_test" );
	simple.add_simple_command( "init_check_button_test"	);
	simple.add_simple_command( "init_simple_text"		);
	simple.add_simple_command( "init_simple_path"		);
	simple.add_simple_command( "pack_sample_window"		);
	simple.add_simple_command( "init_frame_window"		);
	simple.add_simple_command( "init_textfile_view"		);	
	simple.add_simple_command( "init_spinner_menu"		);		
	simple.add_simple_command( "init_CAN_msg_view"		);	
	simple.add_callback_all_items( menu_callback  );

	Rectangle* rect = MainDisplay.get_useable_rect( );
	float menu_bottom = 2.0*rect->get_height()/3.0;
	hm.move_to( rect->get_left()+10, menu_bottom );	
	hm.set_width_height(rect->get_width()-10, 30 );
	//hm.move_to( 100, 768-36 );
	hm.add_entry_text( "File" );
	hm.add_entry_text( "Edit" );	
	hm.add_entry_text( "View" );	
	hm.add_sub_menu( "Atoms", &simple );
	//hm.add_sub_menu( "Molecules", &molecules );	
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &hm );
}

SpinnerControl spin(200, 36);
SpinnerControl spin2(200, 36);

void init_spinner_menu()
{
	spin.move_to  ( 300, 300 );
	spin.set_value( 350  );
	spin.set_max  ( 1000 );
	spin.set_min  (    0 );
	spin.set_text_color(0xFFFF0000);
 
	spin2.move_to  ( 300, 400 );
	spin2.set_value( 400  );
	spin2.set_max  ( 1000 );
	spin2.set_min  (    0 );
	spin2.set_text_color(0xFFFF0000);

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &spin  );
	MainDisplay.add_object( &spin2 );
}

void init_gyro_view()
{
	GyroView* view = new GyroView(100, 500, 450, 50);
	view->set_roll_angle_deg ( 30 );
	view->set_pitch_angle_deg( 30 ); 
	view->set_heading_angle_deg( 30 ); 	
	//view->print_info();
		
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( view  );
}

void init_loadcell_view()
{
	int w = MainDisplay.screen_width /2. ;
	LoadCellView* left  = new LoadCellView(50, w, 400, 50    );
	LoadCellView* right = new LoadCellView(w+20, w*2, 400, 50);

	// 200 lbs / 4 = 50lbs nominal
	left->set_sensor_value( 0, 75 ); 	// heel left
	left->set_sensor_value( 1, 65 );
	left->set_sensor_value( 2, -25 ); 	// toe left
	left->set_sensor_value( 3, -30 );

	right->set_sensor_value( 0, 50 );
	right->set_sensor_value( 1, 60 );
	right->set_sensor_value( 2, 25 ); 
	right->set_sensor_value( 3, 20 ); 	

		
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( left  );
	MainDisplay.add_object( right  );	
}

ScrollBar My_sb;
ScrollBar My_sb2;
void init_scrollbar()
{
	My_sb.set_width_height	( 200, 200 );
	My_sb.move_to		 	( 400, 400 );
	My_sb.set_max_value		( 100. );
	My_sb.set_min_value		( 0.   );
	My_sb.set_amount_visible( 25.  );
	My_sb.print_scroll_info	(      );

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &My_sb );
}

void init_listbox()
{
	Rectangle* rect = MainDisplay.get_useable_rect( );
	//AvailableClients.set_top_down	( false );
	AvailableClients.set_width_height( 200, 200 );

	AvailableClients.adjust_height_for_num_visible_items( 9 );
	populate_simple_lb				 (		);
	AvailableClients.set_text_size	 ( 18.0 );
	// Should set the bottom of the list box at the below coordinate!
	AvailableClients.move_to		 ( rect->get_left(), rect->get_bottom() );

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &AvailableClients );
}

void init_tab_listbox()
{
	printf("\ninit_tab_listbox\n");
	static struct HeaderItemInfo hdr_info;
	if (tab_lb.is_created()==false)
	{
		hdr_info.start_x  = 10.;
		hdr_info.end_x	  = 10.;
		hdr_info.text 	  = "Name";
		hdr_info.width	  = -1;
		hdr_info.alignment= HEADER_ALIGN_LEFT;	// left,center,right
		tab_lb.add_column( &hdr_info );
	
		printf("\ninit_tab_listbox : added first column \n");
		hdr_info.text 	 = "Advisor";
		//tab_lb.add_column( &hdr_info );
	
		hdr_info.text 	 = "Expected Time";	
		tab_lb.add_column( &hdr_info );
		printf("\ninit_tab_listbox : added 3 columns \n");
	}
	tab_lb.move_to( 100, 50 );
	printf("\ninit_tab_listbox : moved_to \n");	
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &tab_lb );
}

void init_dropdown_menu()
{
	
}

void init_motor_gui_test()
{

}

/*CANMessageView 			 msg_view;
void fill_data( byte* mdata, byte last)
{
	mdata[0] = 0x12;
	mdata[1] = 0x34;
	mdata[2] = 0x56;
	mdata[3] = 0x78;	
	mdata[4] = 0xAB;
	mdata[5] = 0xCD;
	mdata[6] = 0xEF;
	mdata[7] = last;
}
void fill_msgs(  )
{
	struct sCAN can;
	for (int i=0; i<100; i++)
	{
		can.id.group.id 	  = 0x0040+i;
		can.id.group.instance = 0x20  +i;
		can.header.DLC 		  = 8;
		fill_data( can.data, 0x10 + i );
		msg_view.add_message( &can );
	}
}
void init_CAN_msg_view( )
{
	msg_view.move_to  		 ( 200, 200   );
	msg_view.set_width_height( 600, 400   );	
	msg_view.set_text_color	 ( 0xFFFF0000 );
	printf("CAN_msg_view: object constructed and positioned.\n");
	
	fill_msgs();
	printf("CAN_msg_view: CAN msgs added.\n");

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &msg_view );
}*/
//static ListBox  	MyList	   ( 20, 320, 700, 550  );
//static ProgressBar  MyProgress ( 450, 650, 400, 375 );
//static CheckBox 	MyCheck	   ( 300, 400, 400, 350 );


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void print_test_list()
{
	printf("0 : init_simple_button_test  \n");	// Simple
	//printf("1 : init_simple_textview_test\n");		
	printf("2 : init_textview_test\n"	 );
	printf("3 : init_progressbar_test\n" );
	printf("4 : init_radio_button_test\n");
	printf("5 : init_check_button_test\n");
	printf("6 : init_simple_text\n" 	 );
	printf("7 : init_simple_path\n" 	 );
	printf("8 : pack_sample_window\n"	 );
	printf("9 : init_frame_window \n"	 );		// Combined
	printf("10 : init_textfile_view\n"	 );
	printf("11 : init_horiz_menu\n"	 	 );
	printf("12 : init_vert_menu\n"	 	 );
	printf("13 : init_combo_menu\n"	 	 );
	printf("14 : init_spinner_menu\n"	 );
	printf("15 : init_listbox_menu\n"	 );
	printf("16 : init_tab_listbox_menu\n");	
	printf("17 : init_scrollbar()\n"	 );	
	printf("18 : empty\n"	 );

	// test_combo_layouts.hpp
	printf("21 : init_image_gallery\n"	 );
	printf("22 : init_okay_cancel_dlg\n" );
	printf("23 : init_audio_view \n"	 );
	printf("24 : init_directory_lb_test\n");
	printf("25 : init_file_browser\n"	 );
	printf("26 : init_sidebar_test\n"	 );
	
	// test_graph_layouts.hpp :
	printf("30 : init_line_graph\n"		 );		// GRAPHS
	printf("31 : init_histogram_graph\n" );	
	printf("32 : init_bar_graph\n"		 );	
	printf("33 : init_scatter_graph\n"	 );	
	printf("34 : init_combined_graph\n"	 );	

	// test_game_layouts.hpp
	printf("40 : init_cards_only\n"	);			// GAMES:
	printf("41 : init_cards\n"	 	);	
	printf("42 : init_blackjack\n"	);	
	printf("43 : init_hearts\n"		);
	printf("44 : init_reversi\n"	);			
	printf("45 : init_battleships\n");
	printf("\n"	);	
}


/*void load_test_screen(int number)
{
	printf("init_display() DisplayNum=%d\n", number);
	//number = 5;
	switch(number) 
	{	
	case 0:	init_simple_button_test	();		break;
	case 1:	init_gyro_view			();		break;
	case 2:	init_textview_test		();		break;
	case 3: init_progressbar_test	();		break;
	case 4: init_radio_button_test	();		break;
	case 5: init_check_button_test	();		break;
	case 6: init_simple_text		();		break;
	case 7: init_simple_path		();		break;
	case 8: pack_sample_window		();		break;
	case 9: init_frame_window		();		break;		
	case 10: init_textfile_view		();		break;		
	case 11: init_horiz_menu		();		break;			
	case 12: init_vert_menu			();		break;
	case 13: init_combo_menu		();		break;
	case 14: init_spinner_menu		();		break;
	case 15: init_listbox			();		break;
	case 16: init_tab_listbox		();		break;
	case 17: init_scrollbar			();		break;
				
	case 18: *init_CAN_msg_view		();*		break;

	case 21: init_image_gallery		();		break;		
	case 22: init_okay_cancel_dlg	();		break;
	
	case 24: init_directory_lb_test	();		break;
	case 25: init_file_browser		();		break;			
	case 26: init_sidebar_test		();		break;
	
	case 30: init_line_graph		();		break;		
	case 31: init_histogram_graph	();		break;
	case 32: init_bar_graph			();		break;
	case 33: init_scatter_graph		();		break;
	case 34: init_combined_graph	();		break;

	// GAMES start at 30!
	case 40: init_cards_only		();		break;
	case 41: init_card				();		break;				
	case 42: init_blackjack			();		break;
	case 43: init_hearts			();		break;
	case 44: init_reversi			();		break;
	case 45: init_battleships		();		break;	
 
	case 80: init_pot_objs(); 		break;
	case 81: init_tilt_objs(); 		break;
	case 82: init_control_test(); 	break;
	
	default: 	break;
	}
} */