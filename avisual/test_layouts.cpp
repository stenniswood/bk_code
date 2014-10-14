// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"
#include "line_graph.hpp"
#include "dataset.hpp"
#include "histogram.hpp"
#include "line_graph.hpp"
#include "leveler.hpp"
#include "display.h"
#include "display_manager.hpp"
#include "button.hpp"
#include "listbox.hpp"
#include "checkbox.hpp"
#include "progressbar.hpp"
#include "text_view.hpp"
#include "scroll_bar.hpp"
#include "control.hpp"
#include "icon.hpp"
#include "visual_memory.h"
#include "listbox.hpp"
#include "tabular_listbox.hpp"
#include "window.hpp"
#include "frame_window.hpp"
#include <vector>
#include "radiobutton.hpp"
#include "test_layouts.hpp"
#include "directory_listbox.hpp"
#include "file_browser.hpp"
#include "image_gallery.hpp"
//#include "window_layouts.hpp"


static TextView 	ConnectionStatus( 50, 1230, 750, 700 );
static TextView 	LongText		( 50, 1230, 750, 500 );
static TabularListBox  tab_lb		( 600, 900, 750, 50 );

/********************* A sample Window ***********************************/
static FrameWindow	ParentWindowFrame(450, 1050, 500, 100);
static Window		ParentWindowF(450, 1050, 500, 100);
static Window		ViewWindow	 (0, 1080, 760, 0);
static TextView 	SampleText;
static ListBox  	AvailableClients;
static IconView		test_image;
static IconView		test_icon ( 50,200 );
/*************************************************************************/

static ScrollBar   vsb;
static ScrollBar   hsb(false);

////////////////////////////////////////////
static DataSet ds1(100);
static DataSet ds2(100);
static DataSet ds3(100);
static DataSet ds_tiltx(1000);
static DataSet ds_tilty(1000);

static Histogram hg ( 100, 300, 300, 100 );
static LineGraph lg1( 100, 300, 600, 400 );
static LineGraph lg2( 400, 600, 600, 400 );
static LineGraph lg3( 700, 900, 600, 400 );

static Leveler l1(1024, 0);
static Leveler l2(1024, 0);
static Leveler l3(1024, 0);
static Leveler l4(1024, 0);
static Leveler l5(1024, 0);
static Leveler l6(1024, 0);

// L, R, T, B
static Button   	MyButt	   ( 450, 600, 400, 350 );
static Button   	Okay	   (-1,-1);
static Button   	Cancel	   (-1,-1);

static ListBox  	MyList	   ( 20, 320, 700, 550  );
static ProgressBar  MyProgress ( 450, 650, 300, 275 );
static CheckBox 	MyCheck	   ( 300, 400, 400, 350 );

static RadioButton 	MyRadio1 ( -1, -1 );
static RadioButton 	MyRadio2 ( -1, -1 );
static RadioButton 	MyRadio3 ( -1, -1 );
static RadioButton 	MyRadio4 ( -1, -1 );

static vector<DirectoryListBox>  dir_lb;

//ButtonArrayMot MyArray	( 700, 1070, 350, 100);
static char 		ConnectionStatusText[255];

void print_test_list()
{
	printf("0 : init_simple_button_test\n");
	printf("1 : init_simple_textview_test\n");
	printf("2 : init_textview_test\n");
	printf("3 : init_progressbar_test\n");
	printf("4 : init_radio_button_test\n");
	printf("5 : init_check_button_test\n");
	printf("6 : init_sidebar_test\n");
	printf("7 : init_directory_lb_test\n");
	printf("8 : init_file_browser\n");
	printf("9 : pack_sample_window\n");
	printf("10 : init_frame_window\n");
	printf("11 : init_textfile_view\n");
	printf("12 : init_image_gallery\n");
	printf("20 : init_okay_cancel_dlg\n");
	printf("14 : init_\n");
	printf("15 : init_\n");
}
void load_test_screen(int number)
{
	printf("init_display() DisplayNum=%d\n", number);
	//number = 5;
	switch(number) 
	{	
	case 0:	init_simple_button_test();
			break;
	case 1:	init_simple_textview_test();
			break;
	case 2:	init_textview_test();
			break;
	case 3: init_progressbar_test();
			break;
	case 4: init_radio_button_test();
			break;
	case 5: init_check_button_test();
			break;
	case 6: init_sidebar_test();
			break;		
	case 7: init_directory_lb_test();
			break;		
	case 8: init_file_browser();
			break;
	case 9: pack_sample_window();
			break;		
	case 10: init_frame_window();
			break;		
	case 11: init_textfile_view();
			break;		
	case 12: init_image_gallery();
			break;		
	case 20: init_okay_cancel_dlg();
			break;
	default:	
			break;
	}
}

void init_simple_button_test()
{
	MyButt.set_text( "Push me" );
	MyButt.set_state(true);

	Button* test = new Button( -1, -1 ); 
	test->set_text( "Try me and see" );
	test->copy_position_vert( &MyButt );
	test->set_position_right_of( &MyButt );
	
	// Add to display manager:
	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &MyButt );	
	MainDisplay.add_object( test );		
}

void init_simple_textview_test()
{
	strcpy(ConnectionStatusText, "No computers Found" );	

	ConnectionStatus.set_text				( ConnectionStatusText );
	ConnectionStatus.set_text_size			( 18.0		 );
	ConnectionStatus.set_text_color			( 0xFFFF0000 );
	ConnectionStatus.set_background_color	( 0xFFFfFf00 );
	ConnectionStatus.center_vertical		( TRUE		 );
	ConnectionStatus.center_horizontal		( TRUE		 );	

	// Add to display manager:
	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &ConnectionStatus );
}

void init_textview_test()
{
	strcpy(ConnectionStatusText, "A moment of kindness between two animals was captured by a Tampa police officer just before a colleague rescued a stranded dog from the Hillsborough River. Residents in Tampas Seminole Heights neighborhood heard a strange noise coming from the river " );	
	// this past Friday night and then saw the dog, named White Boy, struggling to climb out of the river the next morning.
	LongText.set_text				( ConnectionStatusText );
	LongText.set_text_size			( 15.0		 );
	LongText.set_text_color			( 0xFFFF0000 );
	LongText.set_background_color	( 0xFFFfFf00 );
	//LongText.center_vertical		( TRUE		 );
	//LongText.center_horizontal	( TRUE		 );	

	// Add to display manager:
	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &LongText );
}

void init_progressbar_test()
{
	MyProgress.set_percentage(100.0);		
	MyProgress.show_percentage();
	MyProgress.print_positions();
	
	ProgressBar* tmp = new ProgressBar();
	tmp->set_percentage	(50.0);
	tmp->copy_position_horiz( &MyProgress );
	//tmp->show_percentage();
	tmp->set_position_above ( &MyProgress );
	tmp->print_positions();

	// Add to display manager:
	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &MyProgress );
	MainDisplay.add_object( tmp		    );	

	tmp = new ProgressBar();
	tmp->copy_position_horiz( &MyProgress );
	tmp->set_position_below ( &MyProgress );
	tmp->set_percentage(20.0);

	tmp->print_positions();	
	MainDisplay.add_object( tmp		    );	
}

void init_radio_button_test()
{
	MyRadio1.move_to ( 200, 700 );
	MyRadio1.set_text("AM Radio");
	MyRadio2.set_text("FM Radio");
	MyRadio3.set_text("XM Radio");
	MyRadio4.set_text("Internet Radio");
				
	MyRadio2.set_position_below ( &MyRadio1 );
	MyRadio3.set_position_below ( &MyRadio2 );
	MyRadio4.set_position_below ( &MyRadio3 );
	
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

//CheckBox check1(50, 250, 300, 250);
//CheckBox check2(50, 250, 240, 190);
CheckBox check1(-1, -1);
CheckBox check2(-1, -1);

void init_check_button_test()
{
	check1.move_to(50, 250);

	check1.set_text("Select me");
	check2.set_text("Pick me too! ");
	check1.set_check();
	//check1.set_position_above( &check2 );
	check2.set_position_below( &check1 );
	check1.copy_position_horiz( &check2 );
		
	//check1.print_positions();
	//check2.print_positions();
	
	// Add to display manager:
	MainDisplay.remove_all_objects(		);	
	MainDisplay.add_object( &check1 );
	MainDisplay.add_object( &check2 );	
}

void init_sidebar_test()
{
	// Add to display manager:
	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &ConnectionStatus );
}

/*  There are several views in an file explorer.
	The first one, we'll have a succession of normal listboxes (not tabular)
    Each level should be a tabularListbox.
*/
void init_directory_lb_test()
{	
	printf( "\ninit_directory_lb_test() DirectoryListbox::\n" );
	DirectoryListBox* tmp_dir  = new DirectoryListBox();
	tmp_dir->set_width_height	 ( 200, 200 );		// height will get overwritten.
	tmp_dir->populate_directories( "/home/pi/", 1 );
	ParentWindowF.pack_control   ( tmp_dir, PACK_LEFT, PACK_FILL_PARENT );

	// widths of columns
	DirectoryListBox* tmp_dir2  = new DirectoryListBox();	
	tmp_dir2->set_odd_color   ( 0xFFFFFFFF 	);
	tmp_dir2->set_width_height( 200, 200 	);		// height will get overwritten.
	tmp_dir2->populate_files  ( "/home/pi/aorient/", 1 );

	tmp_dir2->set_position_right_of( tmp_dir, true, 0 );
	tmp_dir2->copy_position_vert   ( tmp_dir );	
	ParentWindowF.add_control	   ( tmp_dir2 );	

//	dir_lb.populate_files		( "/home/pi/", 1 );

	// Add to display manager:
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &ConnectionStatus );	
	MainDisplay.add_object( &ParentWindowF );	
}

FileBrowser  browser(800,600);
void init_file_browser()
{
	browser.set_base_path( "/home/pi/" );
	browser.move_to(100,100);	
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &browser );		
}
void init_frame_window()
{
	ParentWindowFrame.print_positions();
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &ParentWindowFrame );
}
char textfilename[] = "/home/pi/HelloX.c";

TextView tf;
void init_textfile_view()
{
	tf.set_width_height	( 600, 600 );
	tf.move_to  		( 100, 100 );	
	tf.load_file		( textfilename );
		
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &tf );		
}


ImageGallery ig;
void init_image_gallery()
{
	ig.set_position(10,1200, 700, 10);
	ig.onCreate    (); 
	//ig.set_directory( "/home/pi/abkInstant/media/" );
	ig.set_directory( "/media/My\ Book/tv/PhonePics/" );

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &ig );
}
void init_okay_cancel_dlg()
{	
	MyRadio1.set_text("AM Radio");
	MyRadio2.set_text("FM Radio");
	MyRadio3.set_text("XM Radio");
	MyRadio4.set_text("Internet Radio");
				
/*	MyRadio2.set_position_below ( &MyRadio1 );
	MyRadio3.set_position_below ( &MyRadio2 );
	MyRadio4.set_position_below ( &MyRadio3 ); */
	
	MyRadio1.join_group( &MyRadio2 );
	MyRadio1.join_group( &MyRadio3 );
	MyRadio1.join_group( &MyRadio4 );
	MyRadio1.expand_group_widths();		
	MyRadio3.select();
	MyRadio2.select();

	check1.set_text("V6 Engine");
	check2.set_text("All wheel drive");
	check1.set_check();
	//check1.set_position_above( &check2 );
	//check2.set_position_below( &check1 );
	//check1.copy_position_horiz( &check2 );

	Okay.set_text("Okay");
	Cancel.set_text("Cancel");
	
	SampleText.set_width_height(-1,-1);
	SampleText.set_text("My Car dialog\n");
	//SampleText.center_vertical(TRUE);
	ParentWindowF.pack_control( &SampleText, PACK_FILL_PARENT, PACKER_ALIGN_TOP );
	ParentWindowF.pack_control( &MyRadio1, PACK_RIGHT, PACKER_ALIGN_TOP );
	ParentWindowF.pack_below  ( &MyRadio2, &MyRadio1, PACK_COPY_HORIZ );
	ParentWindowF.pack_below  ( &MyRadio3, &MyRadio2, PACK_COPY_HORIZ );
	ParentWindowF.pack_below  ( &MyRadio4, &MyRadio3, PACK_COPY_HORIZ );

	ParentWindowF.pack_control( &check1, PACK_LEFT, PACKER_ALIGN_TOP  	);
	ParentWindowF.pack_below  ( &check2, &check1, PACK_COPY_HORIZ );

	ParentWindowF.pack_control( &test_icon , PACK_LEFT	 , 	PACKER_ALIGN_BOTTOM );
	ParentWindowF.pack_control( &Okay	   , PACK_RIGHT  ,  PACKER_ALIGN_BOTTOM );
	ParentWindowF.pack_control( &Cancel	   , PACK_RIGHT  ,  PACKER_ALIGN_BOTTOM );
	
	// Add to display manager:
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object		  ( &ParentWindowF );	
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

	/*	strcpy(ConnectionStatusText, "A moment of kindness between two animals was captured " );		
	SampleText.set_text			( ConnectionStatusText );
	SampleText.move_to			( 10, 500  );
	SampleText.calc_margins		( 0.1      );
	SampleText.set_width_height ( 300, 200 );
	SampleText.center_vertical	( TRUE		 );
	SampleText.center_horizontal( TRUE		 );	 
	ParentWindowF.pack_control	( &SampleText, PACK_FILL_PARENT, PACK_FILL_PARENT );*/

	AvailableClients.set_width_height( 200, 200 );
	AvailableClients.adjust_height_for_num_visible_items( 9 );
	populate_simple_lb();
	AvailableClients.set_text_size( 18.0 );	

	ParentWindowF.print_positions();

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
}


