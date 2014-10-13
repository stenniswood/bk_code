// line graph OpenVG program
// Steve Tenniswood
#include <shapes.h>
#include <fontinfo.h>
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
#include <vector>
#include "radiobutton.hpp"
#include "test_layouts.hpp"
#include "directory_listbox.hpp"



//static FrameWindow	ParentWindow(450, 1050, 500, 100);
static Window		ParentFrame(450, 1050, 500, 100);
//static vector<ListBox>  Levels;

/* Start with just 1 level 
*/
Window* create_file_browser( char* mBasePath )
{
	printf( "\nDirectoryListbox::\n" );
	DirectoryListBox* tmp_dir  = new DirectoryListBox();
	tmp_dir->set_width_height	 ( 200, 200 );		// height will get overwritten.
	tmp_dir->print_positions	 ( 			);
	tmp_dir->setup_headings		 ( 			);		// should place start_x!
	tmp_dir->populate_directories( "/home/pi/", 1 );
	tmp_dir->print_tab_info		 ( 			);
	tmp_dir->print_client_rect();
	ParentWindow.pack_control   ( tmp_dir, PACK_LEFT, PACK_FILL_PARENT );
	tmp_dir->print_client_rect();
	
/*	DirectoryListBox* tmp_dir2 = new DirectoryListBox();
	tmp_dir2->print_positions	  ( );
	tmp_dir2->setup_headings		  ( );
	tmp_dir2->populate_directories ( "/home/pi/aorient", 1 );
	tmp_dir2->print_tab_info		  ( );
	tmp_dir2->set_odd_color 		  ( 0xCFCFCFFF );
	tmp_dir2->set_even_color		  ( 0xCFFFFFFF );
	tmp_dir2->set_width_height     ( 200, 200 );		// height will get overwritten.
	tmp_dir2->set_position_right_of( tmp_dir );
	tmp_dir2->copy_position_vert	  ( tmp_dir );	
	ParentWindowF.add_control	  ( tmp_dir2 );	*/
	//dir_lb.push_back		    ( *tmp_dir   );

	
//	dir_lb.populate_files		( "/home/pi/", 1 );

	// Add to display manager:
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &ParentWindowF );

	return &ParentWindow;
}


