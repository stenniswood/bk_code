/*************************************************************************
NAME	:	test_layouts.cpp
DESCRIPTION:

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
#include <vector>
#include <string>
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"
#include "dataset.hpp"
#include "display.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"

#include "display_manager.hpp"
#include "frame_window.hpp"

#include "test_layouts.hpp"
#include "visual_memory.h"
#include "audio_memory.h"
#include "fft.h"
#include "test_combo_layouts.hpp"


static TextView 		ConnectionStatus( 50, 1230, 750, 700 );
static TextView 		LongText		( 50, 1230, 750, 500 );
//static TabularListBox	tab_lb			( 600, 900, 750, 50  );

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

static Leveler l1(1024, 0);
static Leveler l2(1024, 0);
static Leveler l3(1024, 0);
static Leveler l4(1024, 0);
static Leveler l5(1024, 0);
static Leveler l6(1024, 0);

// L, R, T, B
static Button   	Okay	   (-1,-1);
static Button   	Cancel	   (-1,-1);

static PowerLevel		 pl		(-1,-1);
static StereoPowerLevels spl  	(-1,-1);

static ListBox  	MyList	   ( 20, 320, 700, 550  );
static ProgressBar  MyProgress ( 450, 650, 400, 375 );
static CheckBox 	MyCheck	   ( 300, 400, 400, 350 );

static RadioButton 	MyRadio1 ( -1, -1 );
static RadioButton 	MyRadio2 ( -1, -1 );
static RadioButton 	MyRadio3 ( -1, -1 );
static RadioButton 	MyRadio4 ( -1, -1 );

static CheckBox 	audio_source1(-1, -1);
static CheckBox 	audio_source2(-1, -1);

static vector<DirectoryListBox>  dir_lb;

//ButtonArrayMot MyArray	( 700, 1070, 350, 100);
static char 		ConnectionStatusText[255];

void print_test_combo_list()
{
	printf("10 : init_sidebar_test\n"	 );
	printf("11 : init_directory_lb_test\n"	 );
	printf("12 : init_file_browser\n"	 );
	printf("13 : init_image_gallery\n"		 );	
	printf("14 : init_okay_cancel_dlg\n" );	
}

void load_test_combo_screen(int number)
{
	printf("init_display() DisplayNum=%d\n", number);
	//number = 5;
	switch(number) 
	{	
	case 10: init_sidebar_test();
			break;		
	case 11: init_directory_lb_test();
			break;		
	case 12: init_file_browser();
			break;		
	case 13: init_image_gallery();
			break;		
	case 14: init_okay_cancel_dlg();
			break;
	default: //load_combo_test_screen( number );
			break;
	}
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
	tmp_dir->populate_directories( (char*)"/home/pi/", 1 );
	ParentWindowF.pack_control   ( tmp_dir, PACK_LEFT, PACK_FILL_PARENT );

	// widths of columns
	DirectoryListBox* tmp_dir2  = new DirectoryListBox();	
	//tmp_dir2->set_odd_color   ( 0xFFFFFFFF 	);
	tmp_dir2->set_width_height( 200, 200 	);		// height will get overwritten.
	tmp_dir2->populate_files  ( (char*)"/home/pi/aorient/", 1 );
	
	tmp_dir2->set_position_right_of( tmp_dir, true, 0 );
	tmp_dir2->copy_position_vert   ( tmp_dir );	
	ParentWindowF.add_control	   ( tmp_dir2 );	

	// Add to display manager:
	MainDisplay.remove_all_objects(	);
//	MainDisplay.add_object( &ConnectionStatus );	
	MainDisplay.add_object( &ParentWindowF );	
}

FileBrowser  browser(1000,600);
void init_file_browser()
{
	browser.set_base_path( "/home/pi/" );
	browser.move_to(100,100);

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &browser );		
}

ImageGallery ig;
void init_image_gallery()
{
	Rectangle* rect = MainDisplay.get_useable_rect();
	ig.set_position(rect->get_left(), rect->get_width(), rect->get_top(), rect->get_bottom() );
	ig.onCreate    ();
	ig.set_directory( "/home/pi/bk_code/abkInstant/media/" );
	//ig.set_directory( "/media/My Book/tv/PhonePics/" ); 

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

	audio_source1.set_text("V6 Engine");
	audio_source2.set_text("All wheel drive");
	audio_source1.set_check();
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

	ParentWindowF.pack_control( &audio_source1, PACK_LEFT, PACKER_ALIGN_TOP  	);
	ParentWindowF.pack_below  ( &audio_source2, &audio_source1, PACK_COPY_HORIZ );

	ParentWindowF.pack_control( &test_icon , PACK_LEFT	 , 	PACKER_ALIGN_BOTTOM );
	ParentWindowF.pack_control( &Okay	   , PACK_RIGHT  ,  PACKER_ALIGN_BOTTOM );
	ParentWindowF.pack_control( &Cancel	   , PACK_RIGHT  ,  PACKER_ALIGN_BOTTOM );

	// Add to display manager:
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object		  ( &ParentWindowF );	
}



void init_drawing_app()
{

}