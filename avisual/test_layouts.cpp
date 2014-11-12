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
#include "bar_graph.hpp"
#include "bar_graph_stacked.hpp"
#include "scatter_plot.hpp"
#include "power_level.hpp"
#include "stereo_power.hpp"



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
static DataSet ds1;
static DataSet ds2;
static DataSet ds3;
static DataSet ds_tiltx;
static DataSet ds_tilty;

static Histogram 		hg ( 100, 300, 600, 400 );		// was ( 100, 300, 300, 100 );
static LineGraph 		lg1( 100, 300, 600, 400 );
static LineGraph 		lg2( 400, 600, 600, 400 );
static LineGraph 		lg3( 700, 900, 600, 400 );

static BarGraph  		bg ( 100, 500, 600, 400 );		// was ( 100, 300, 300, 100 );
static StackedBarGraph  sbg( 100, 300, 600, 400 );		// was ( 100, 300, 300, 100 );
static ScatterGraph  	sg ( 100, 300, 600, 400 );		// was ( 100, 300, 300, 100 );

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

static PowerLevel   pl		   (-1,-1);
static StereoPowerLevels spl   (-1,-1);

static ListBox  	MyList	   ( 20, 320, 700, 550  );
static ProgressBar  MyProgress ( 450, 650, 400, 375 );
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
	printf("2 : init_textview_test\n"	 );
	printf("3 : init_progressbar_test\n" );
	printf("4 : init_radio_button_test\n");
	printf("5 : init_check_button_test\n");
	printf("6 : init_sidebar_test\n"	 );
	printf("7 : init_directory_lb_test\n");
	printf("8 : init_file_browser\n"	 );
	printf("9 : pack_sample_window\n"	 );
	printf("10 : init_frame_window\n"	 );
	printf("11 : init_textfile_view\n"	 );
	printf("12 : init_image_gallery\n"	 );
	printf("13 : init_line_graph\n"		 );	
	printf("14 : init_histogram_graph\n" );	
	printf("15 : init_bar_graph\n"		 );	
	printf("16 : init_scatter_graph\n"	 );	
	
	printf("20 : init_okay_cancel_dlg\n" );
	printf("21 : init_combined_graph\n"	 );
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
	case 13: init_line_graph();
			break;		
	case 14: init_histogram_graph();
			break;
	case 15: init_bar_graph();
			break;
	case 16: init_scatter_graph();
			break;
	case 20: init_okay_cancel_dlg();
			break;
	case 21: init_combined_graph();
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
	test->copy_position_vert   ( &MyButt );
	test->set_position_right_of( &MyButt );

	// PowerLevel
	pl.move_to  		(100, 200);
	pl.set_width_height ( 75, 150 );
	pl.set_max  		( 100.0 );
	pl.set_min  		(   0.0 );
	pl.set_level		(  75.0 );

	// Stereo Power Indicator	
	spl.move_to  		(100, 200);
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
	//MyProgress.print_positions();
	
	ProgressBar* tmp = new ProgressBar();
	tmp->set_percentage	(50.0);
	tmp->move_to(100,500);
	tmp->copy_position_horiz( &MyProgress );
	//tmp->show_percentage();
	//tmp->set_position_above ( &MyProgress );
	//tmp->print_positions();

	ProgressBar* tmp2 = new ProgressBar();
	tmp2->move_to ( 100, 200 );
	tmp2->copy_position_horiz( &MyProgress );
	tmp2->set_percentage(20.0);
	//tmp2->print_positions();	

	// Add to display manager:
	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &MyProgress );
	MainDisplay.add_object( tmp		    );	
	MainDisplay.add_object( tmp2	    );	

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
	check2.set_position_below ( &check1 );
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
	tmp_dir->populate_directories( (char*)"/home/pi/", 1 );
	ParentWindowF.pack_control   ( tmp_dir, PACK_LEFT, PACK_FILL_PARENT );

	// widths of columns
	DirectoryListBox* tmp_dir2  = new DirectoryListBox();	
	tmp_dir2->set_odd_color   ( 0xFFFFFFFF 	);
	tmp_dir2->set_width_height( 200, 200 	);		// height will get overwritten.
	tmp_dir2->populate_files  ( (char*)"/home/pi/aorient/", 1 );

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

char textfilename[] = "Readme.txt"; 
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
	ig.set_directory( "/media/My Book/tv/PhonePics/" ); 

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &ig );
}

void fill_fake_data1()
{
	ds1.add( 1. );	ds1.add( 2. );	ds1.add( 3. );	ds1.add( 4. );
	ds1.add( 5. );	ds1.add( 6. );	ds1.add( 7. );	ds1.add( 8. );
	ds1.add( 9. );	ds1.add( 10. );	ds1.add( 11. );	ds1.add( 12. );
	ds1.add( 13. );	ds1.add( 14. );	ds1.add( 15. );	ds1.add( 16. );
	ds1.add( 17. );	ds1.add( 18. );	ds1.add( 19. );	ds1.add( 20. );	
	ds1.add( 21. );	ds1.add( 22. );	ds1.add( 23. );	ds1.add( 24. );
	ds1.add( 25. );	ds1.add( 26. );	ds1.add( 27. );	ds1.add( 28. );
	ds1.add( 29. );	ds1.add( 30. );	ds1.add( 31. );	ds1.add( 32. );						
}
void fill_fake_data2()
{
	ds2.add( 1. );	ds2.add( 3. );	ds2.add( 5. );	ds2.add( 7. );
	ds2.add( 9. );	ds2.add( 11. );	ds2.add( 13. );	ds2.add( 15. );
	ds2.add( 17. );	ds2.add( 19. );	ds2.add( 21. );	ds2.add( 23. );
	ds2.add( 25. );	ds2.add( 27. );	ds2.add( 29. );	ds2.add( 31. );
	ds2.add( 33. );	ds2.add( 35. );	ds2.add( 37. );	ds2.add( 39. );	
	ds2.add( 41. );	ds2.add( 43. );	ds2.add( 45. );	ds2.add( 47. );
	ds2.add( 49. );	ds2.add( 51. );	ds2.add( 53. );	ds2.add( 55. );
	ds2.add( 57. );	ds2.add( 59. );	ds2.add( 61. );	ds2.add( 63. );
}
void fill_fake_data3()
{
	ds3.add( 10. );	ds3.add( 10. );	ds3.add( 10. );		ds3.add( 10. );
	ds3.add( 11. );	ds3.add( 11. );	ds3.add( 11. );		ds3.add( 10. );
	ds3.add( 9. );	ds3.add( 9. );	ds3.add( 9. );		ds3.add( 9.5 );
	ds3.add( 8. );	ds3.add( 8. );	ds3.add( 8.75 );	ds3.add( 8.5 );
	ds3.add( 12. );	ds3.add( 12. );	ds3.add( 11.5 );	ds3.add( 11.25 );	
	ds3.add( 14. );	ds3.add( 15.5 ); ds3.add( 13.5 );	ds3.add( 12.2 );
	ds3.add( 6. );	ds3.add( 6.1 );	ds3.add( 6.9 );		ds3.add( 9.5 );
	ds3.add( 5. );	ds3.add( 16.2 ); ds3.add( 8.78 );	ds3.add( 8.88 );
}

const char chart1_title[] = "Sample X Line Graph";
const char chart2_title[] = "Sample Y Line Graph";

void init_line_graph()
{
	printf("Line Graph Init\n");
	fill_fake_data1			(	);
	fill_fake_data2			(	);
	fill_fake_data3			(	);	

	lg1.set_title 			( (char*) chart1_title );
	lg1.set_xLabel			( (char*) "Time" 				);
	lg1.set_yLabel			( (char*) "X Degree/second" 	);	
	//lg1.draw_title			(      );
	lg1.set_max				( 64.0 );
	lg1.set_min				( 0.0  );
	
	lg1.add_data_series		( &ds1 							);
	lg1.add_data_series		( &ds3 							);	
	lg1.calc_scale			( 								);
	lg1.set_horizontal_lines( 5 							);
	lg1.set_vertical_lines	( 5 							);

	lg2.set_title 			( (char*) chart2_title 			);
	lg2.set_xLabel			( (char*) "Time" 				);
	lg2.set_yLabel			( (char*) "Y Degree/second" 	);
	lg2.add_data_series		( &ds2 							);
	lg2.set_max				( 64.0 );
	lg2.set_min				( 0.0 );
	lg2.calc_scale			( );
	lg2.set_horizontal_lines( 5		);
	lg2.set_vertical_lines  ( 5 	); 
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &lg1 );
	MainDisplay.add_object( &lg2 );
}
void init_histogram_graph()
{
	printf("Histogram Graph Init\n");
	fill_fake_data3			(	);
	
	hg.set_title 			( (char*) chart1_title );
	hg.set_xLabel			( (char*) "Angle" 		);
	hg.set_yLabel			( (char*) "Samples" 	);	
	hg.add_data_series		( &ds3 );

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( &hg );
}
void init_bar_graph	 	()
{
	fill_fake_data3();
	bg.set_title 			( (char*) "Bar Chart" );
	bg.set_xLabel			( (char*) "Month" 		);
	bg.set_yLabel			( (char*) "Apples Sold" 	);	
	bg.add_data_series		( &ds3 );

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( &bg );
}
void init_scatter_graph	 	()
{

}
void init_combined_graph()
{
	printf("Line Graph Init\n");
	fill_fake_data1			(	);
	fill_fake_data2			(	);
	fill_fake_data3			(	);	

	lg1.set_title 			( (char*) chart1_title 			);
	lg1.set_xLabel			( (char*) "Time" 				);
	lg1.set_yLabel			( (char*) "X Degree/second" 	);	
	lg1.set_max				( 64.0 );
	lg1.set_min				( 0.0  );	
	lg1.add_data_series		( &ds1 							);
	lg1.add_data_series		( &ds3 							);	
	lg1.calc_scale			( 								);
	lg1.set_horizontal_lines( 5 							);
	lg1.set_vertical_lines	( 5 							);

	lg2.set_title 			( (char*) chart2_title 			);
	lg2.set_xLabel			( (char*) "Time" 				);
	lg2.set_yLabel			( (char*) "Y Degree/second" 	);
	lg2.add_data_series		( &ds2 							);
	lg2.set_max				( 64.0 );
	lg2.set_min				( 0.0 );
	lg2.calc_scale			( );
	lg2.set_horizontal_lines( 5		);
	lg2.set_vertical_lines  ( 5 	); 
	
	bg.move_to				( 675, 400 );
	bg.set_title 			( (char*) "Bar Chart" );
	bg.set_xLabel			( (char*) "Month" 		);
	bg.set_yLabel			( (char*) "Apples Sold" 	);	
	bg.add_data_series		( &ds3 );


	hg.move_to				( 100,100 );
	hg.set_background_color ( 0xFF202020 );
	hg.set_title 			( (char*) chart1_title );
	hg.set_xLabel			( (char*) "Angle" 		);
	hg.set_yLabel			( (char*) "Samples" 	);	
	hg.add_data_series		( &ds3 );

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &lg1 );
	MainDisplay.add_object( &lg2 );
	MainDisplay.add_object( &bg );
	MainDisplay.add_object( &hg );
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


