// line graph OpenVG program
// Steve Tenniswood
#include <shapes.h>
#include <fontinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"

#include "dataset.hpp"
#include "display.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "display_manager.hpp"
#include "client_memory.hpp"
#include "client_list_control.hpp"
 


#define Debug 1


// Avisual display:



//TabularListBox  tab_lb(600, 20, 620, 5, -1);
static TabularListBox  adren_board_list;

/********************* A sample Window ****************/
// 1900 - 1000 = 900
Window		ParentWindow(450, 1050, 500, 100);
TextView 	SampleText;
ListBox  	AvailableClients;
IconView	test_image;
IconView	test_icon(50,200);
/*******************************************************/

ScrollBar   sb;
ScrollBar   hsb(false);

////////////////////////////////////////////
DataSet ds1;
DataSet ds2;
DataSet ds3;
DataSet ds_tiltx;
DataSet ds_tilty;


Histogram hg ( 100, 300, 300, 100 );
LineGraph lg1( 100, 300, 600, 400 );
LineGraph lg2( 400, 600, 600, 400 );
LineGraph lg3( 700, 900, 600, 400 );

Leveler l1(1024, 0);
Leveler l2(1024, 0);
Leveler l3(1024, 0);
Leveler l4(1024, 0);
Leveler l5(1024, 0);
Leveler l6(1024, 0);

// L, R, T, B
static ListBox  	MyBoardList( 20, 320, 700, 550  );
Button   	MyButt	   ( 450, 600, 400, 350 );
ListBox  	MyRobotList( 20, 320, 800, 500  );
ProgressBar MyProgress ( 450, 650, 300, 275 );
CheckBox 	MyCheck	   ( 300, 400, 400, 350 );
//ButtonArrayMot MyArray( 700, 1070, 350, 100);
static char ConnectionStatusText[128];
extern TextView 	ConnectionStatus;
extern TextView 	CmdText;

void set_headings()
{
	struct HeaderItemInfo hdr_info;

	// Column #1 : 
	hdr_info.text  = "Host name";
	hdr_info.width = -1;
	hdr_info.alignment = HEADER_ALIGN_LEFT;	
//	tab_lb.add_column( &hdr_info );
	
	// Column #2 :
	hdr_info.text  = "IP Address";
	hdr_info.width = -1;
	hdr_info.alignment = HEADER_ALIGN_LEFT;		
	//headings->push_back( hdr_info );
//	tab_lb.add_column( &hdr_info );

	// Column #3:
	//hdr_info = new struct HeaderItemInfo();
	hdr_info.text  = "Machine Type";
	hdr_info.width = -1;
	hdr_info.alignment = HEADER_ALIGN_LEFT;			
//	tab_lb.add_column( &hdr_info );
	//headings->push_back( hdr_info );
	//tab_lb.set_headings( headings );
	//printf("#1: %s\n", (*headings)[1].text.c_str() );	
}

void adren_board_list_set_headings()
{
	static struct HeaderItemInfo hdr_info;

	// Column #1 : 
	hdr_info.text  = "Model";
	hdr_info.width = 100;
	hdr_info.alignment = HEADER_ALIGN_LEFT;	
	adren_board_list.add_column( &hdr_info );

	// Column #2 :
	hdr_info.text  = "Instance";
	hdr_info.width = 100;
	hdr_info.alignment = HEADER_ALIGN_LEFT;		
	adren_board_list.add_column( &hdr_info );

	// Column #3:
	//hdr_info = new struct HeaderItemInfo();
	hdr_info.text  = "Firmware";
	hdr_info.width = 100;
	hdr_info.alignment = HEADER_ALIGN_LEFT;			
	adren_board_list.add_column( &hdr_info );	
}

void populate_listbox()
{
	// Now add some rows:	
	vector<string> *data = new vector<string>;	
	data->push_back("sjt-linux");
	data->push_back("192.168.1.142");
	data->push_back("Raspberry Pi");
	adren_board_list.add_row (data);
	
	data = new vector<string>;	
	data->push_back("dev1");
	data->push_back("192.168.1.141");
	data->push_back("Raspberry Pi");
	adren_board_list.add_row (data);

	data = new vector<string>;	
	data->push_back("sjtenniswood");
	data->push_back("192.168.1.140");
	data->push_back("Macbook Pro");
	adren_board_list.add_row (data);

	data = new vector<string>;	
	data->push_back("sjShadows computer");
	data->push_back("192.168.1.666");
	data->push_back("Cat attack");
	adren_board_list.add_row (data);

	data = new vector<string>;	
	data->push_back("merici drops");
	data->push_back("192.168.1.777");
	data->push_back("Windows PC");
//	adren_board_list.add_row (data);
	
	data = new vector<string>;	
	data->push_back("tv");
	data->push_back("192.168.1.144");
	data->push_back("Mac");
//	adren_board_list.add_row (data);
}




extern ClientList	AvailClients;
//static TabularListBox	tabb ( 100, 400, 750, 50  );

void init_avisual()
{
	char* str = new char[255];
	strcpy (str, "This is where the incoming text will go! What is the best way to count to the second line?  Okay. I don't know what else to say.  Fourier Transforms are amazingly fast! How can I say thanks for the things he has done for me?");
	strcpy (ConnectionStatusText, "Not connected");	

	ConnectionStatus.set_text				( ConnectionStatusText );
	ConnectionStatus.set_text_size			( 18.0		 );
	ConnectionStatus.set_text_color			( 0xFFFF0000 );
	ConnectionStatus.set_background_color	( 0xFFFfFf00 );
	ConnectionStatus.center_vertical		( TRUE		 );
	ConnectionStatus.center_horizontal		( TRUE		 );	

	CmdText.set_position  		( 50, 1230, 690, 500 );
	CmdText.set_text	  		( str  		 );
	CmdText.set_text_size 		( 25.0 		 );
	CmdText.set_text_color		( 0xFFFFFFFF );
	CmdText.set_background_color( 0xFF9f9f0f );

	// This should be hidden until asked for via voice.
//	AvailableClients.set_position( 10, 10, 400, 100 );
	AvailClients.move_to( 100, 100 );
	AvailClients.calc_metrics();
	AvailClients.set_headings();	
	AvailClients.update_available_client_list();
	

	//set_headings();
/*	if (Debug) printf("SET_HEADINGS() - \n");
	populate_listbox();
	if (Debug) printf("POPULATED LISTBOX () - \n");
	
	adren_board_list_set_headings();
	adren_board_list.set_width_height( 300,300 );
	adren_board_list.move_to( 10, 10 );
	
	//adren_board_list.copy_position_vert 				(&AvailableClients);
	//adren_board_list.set_text_size				    ( 12.0 );
	adren_board_list.adjust_height_for_num_visible_items( 8 );
	adren_board_list.move_to(100, 100);
	adren_board_list.calc_widths_from_text			    (   );
	adren_board_list.calc_column_positions_from_widths  (   );
	//adren_board_list.calc_metrics();
	if (Debug) adren_board_list.print_positions();

	//printf("CALC_METRICS() - DONE \n");
	//pack_sample_window();	
	//test_icon.set_position( 100, 200, 313, 200);
	test_icon.set_file		( (char*)"./resources/folder.jpg" );
	test_image.set_file		( (char*)"./resources/desert0.jpg" );
	test_image.set_position	( 200, 450, 313, 200);	

	sb.copy_position_vert   ( &test_image );
	sb.set_position_left_of ( &test_image );
	sb.set_max_value		( 11  );
	sb.set_min_value		( 0   );
	sb.scroll_to    		( 1   );
	sb.set_amount_visible	( 10  );
 
	hsb.copy_position_horiz (&test_image );
	hsb.set_position_below  ( &test_image );
	hsb.set_max_value		( 100 );
	hsb.set_min_value		( 0   );
	hsb.scroll_to    		( 20  );
	hsb.set_amount_visible	( 50 );
	
	l1.set_position 		( 500, 600, 300, 70 );
	l1.set_level_percent	( 50.0 );
*/
	// Add to display manager:
	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &ConnectionStatus );
	MainDisplay.add_object( &CmdText 	);
//	MainDisplay.add_object( &l1 		);
	MainDisplay.add_object( &AvailClients );
//	MainDisplay.add_object( &tabb );	
//	MainDisplay.add_object( &tabb );	
	printf("ADDING TABULAR LIST BOX  Avail Clients!!!!\n");
//	MainDisplay.add_object( &test_icon  );
//	MainDisplay.add_object( &adren_board_list );
	MainDisplay.load_resources();
}

// L, R, T, B
//StickFigure  sf1( 100, 300, 300, 100 );
//StickLeg  sl2( 100, 300, 300, 100 );		// overlay

/* Shows a graph */
void init_tilt_objs()
{
	// Link the Dataseries
	hg.set_title 		("Accel ry"			);
	hg.set_xLabel		("Time (seconds)"	);
	hg.set_yLabel		("Rotate about y (degrees)");
	hg.add_data_series  ( &ds_tilty 		);

	lg2.add_data_series	( &ds_tilty 		);
	lg2.set_text	 	( "Rotate y", "Time (seconds)", "Angle (degrees)" );		
	lg2.set_max			( 1024 );
	lg2.set_min			( 0    );

	lg2.calc_scale	 	( 				);
	lg2.set_stroke_width( 1.0 			);
	lg2.set_title_size	( 18.0 			);

	lg3.add_data_series	( &ds3  );
	lg3.set_text		( "Rotate z", "Time (seconds)", "Angle (degrees)" );		
	lg3.set_max			( 1024  );
	lg3.set_min			( 0     );
	lg3.calc_scale		( 		);
	lg3.set_stroke_width( 1.0 	);	
	lg3.set_title_size	( 18.0  );	

	// Add to display manager:
	
	MainDisplay.remove_all_objects();	
	MainDisplay.add_object( &hg  );
	MainDisplay.add_object( &lg1 );
	MainDisplay.add_object( &lg2 );
//	MainDisplay.add_object( &lg3 );
}

void init_pot_objs()
{
	lg1.add_data_series( &ds1 );
	lg1.add_data_series( &ds2 );
	lg1.add_data_series( &ds3 );
	lg1.set_text     ( "Hip", "Time (seconds)", "Angle (degrees)" );	
	lg1.set_max(1024);
	lg1.set_min(0);
	lg1.calc_scale   (  );
	lg1.set_stroke_width( 1.0 );
	lg1.set_title_size  ( 16.0 );

	l1.set_position ( 500, 600, 300, 70 );
	l2.set_position_right_of( &l1, true );
	l3.set_position_right_of( &l2, true );
	l4.set_position_right_of( &l3, true );
	l5.set_position_right_of( &l4, true );
	l6.set_position_right_of( &l5, true );
	
	l1.set_level_percent(50.0);
	l2.set_level_percent(66.6);
	l3.set_level_percent(50.0);
	l4.set_level_percent(50.0);	
	
	// Add to display manager:
	MainDisplay.remove_all_objects();	
	//MainDisplay.add_object( &lg1 );
	//MainDisplay.add_object( &sf1 );	
	//MainDisplay.add_object( &sl2 );		
	/*MainDisplay.add_object( &l1 );	
	MainDisplay.add_object( &l2 );
	MainDisplay.add_object( &l3 );	
	MainDisplay.add_object( &l4 );
	MainDisplay.add_object( &l5 );
	MainDisplay.add_object( &l6 );	*/
}

void update_to_controls()
{
	//bl.PopulateListBox( &MyBoardList );
	// Now Want to put updated Leg info into graphical controls:
}

/* Initial Screen.  Shows Button array and Board List */
void init_control_test()
{
	if (Debug) printf("init_control_test() entered\n"		  );
	MyButt.set_text("Push Me Button"			  );
	MyRobotList.set_item(" No robots connected"    );
	MyBoardList.set_item(" no boards conencted"	  );

	if (Debug) printf("init_control_test() done with stick figure construction\n");

	MyProgress.set_percentage(80.0);	
	if (Debug) printf("init_control_test() setPercentage \n");	
	
	// Add to display manager:
	MainDisplay.remove_all_objects();	
	if (Debug) printf("init_control_test() remove_all_objects\n");	
	
//	MainDisplay.add_object( &MyBoardList );
	//MainDisplay.add_object( &sf1 );
	//MainDisplay.add_object( &sl2 );
	if (Debug) printf("init_control_test() done\n");	
	
//	MainDisplay.add_object( &MyRobotList );	// other robots available on wifi
	MainDisplay.add_object( &MyProgress );
	//MainDisplay.add_object( &MyArray	);
	
	if (Debug) printf("init_control_test() done\n");
}



