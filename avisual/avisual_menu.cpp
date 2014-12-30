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

//#include <string.h>
#include <fontinfo.h>
#include <shapes.h>
#include "Graphbase.hpp"
#include "display.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "avisual_menu.hpp"
#include "test_layouts.hpp"
#include "draw_app.hpp"
#include "test_combo_layouts.hpp"
#include "power.hpp"


HorizontalMenu hm		(-1,-1);
VerticalMenu   atoms    (-1,-1);
VerticalMenu   molecules(-1,-1);
VerticalMenu   games	(-1,-1);
VerticalMenu   graphs	(-1,-1);

VerticalMenu   power_menu	(-1,-1);		// power switch pushed.


#define ifprintf printf
//#define ifprintf //printf

int show_atom_screens (void* menuPtr, int mMenuIndex )
{
	//ifprintf("init_display() DisplayNum=%d\n", number);
	switch(mMenuIndex) 
	{	
	case 0:	init_simple_button_test	();		break;
	case 1:	init_simple_textview_test();	break;
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
	case 17: init_CAN_msg_view		();		break;
	default: break;
	}
}

void init_atom_menu()
{
	ifprintf("init_atom_menu()\n");
	atoms.add_simple_command( "init_simple_button_test" );
	ifprintf("init_atom_menu() 1\n");
	atoms.add_simple_command( "init_simple_textview_test" );
	atoms.add_simple_command( "init_textview_test"		);
	atoms.add_simple_command( "init_progressbar_test"   );
	atoms.add_simple_command( "init_radio_button_test"  );
	atoms.add_simple_command( "init_check_button_test"	);
	atoms.add_simple_command( "init_simple_text"		);
	atoms.add_simple_command( "init_simple_path"		);
	atoms.add_simple_command( "pack_sample_window"		);
	atoms.add_simple_command( "init_frame_window"		);
	atoms.add_simple_command( "init_textfile_view"		);	
	atoms.add_simple_command( "init_horiz_menu"			);
	atoms.add_simple_command( "init_vert_menu"			);
	atoms.add_simple_command( "init_combo_menu"			);
	atoms.add_simple_command( "init_spinner_menu"		);
	atoms.add_simple_command( "init_listbox"			);
	atoms.add_simple_command( "init_tab_listbox"		);
	atoms.add_simple_command( "init_CAN_msg_view"		);

	atoms.add_callback( 0, show_atom_screens  );
	atoms.add_callback( 1, show_atom_screens  );
	atoms.add_callback( 2, show_atom_screens  );
	atoms.add_callback( 3, show_atom_screens  );
	atoms.add_callback( 4, show_atom_screens  );
	atoms.add_callback( 5, show_atom_screens  );
	atoms.add_callback( 6, show_atom_screens  );
	atoms.add_callback( 7, show_atom_screens  );
	atoms.add_callback( 8, show_atom_screens  );			
	atoms.add_callback( 9, show_atom_screens  );			
	atoms.add_callback( 10, show_atom_screens  );			
	atoms.add_callback( 11, show_atom_screens  );						
	atoms.add_callback( 12, show_atom_screens  );						
	atoms.add_callback( 13, show_atom_screens  );						
	atoms.add_callback( 14, show_atom_screens  );
	atoms.add_callback( 15, show_atom_screens  );
	atoms.add_callback( 16, show_atom_screens  );						
	atoms.add_callback( 17, show_atom_screens  );
}

int show_molecule_screens( void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex) 
	{
	case 0: init_image_gallery		();		break;		
	case 1: init_okay_cancel_dlg	();		break;
	case 2: init_audio_view			();		break;
	case 3: init_directory_lb_test	();		break;
	case 4: init_file_browser		();		break;			
	case 5: init_CAN_app			();		break;
	case 6: init_drawing_app		();		break;	
	case 7: init_camera_app			();		break;		
	default: 	break;
	}
}

void init_molecule_menu()
{
	molecules.add_simple_command( "Image Gallery" 	);
	molecules.add_simple_command( "Okay Cancel Dialog" );
	molecules.add_simple_command( "Audio Amp" 		);
	molecules.add_simple_command( "Directory lb" 	);
	molecules.add_simple_command( "File Browser" 	);				
	molecules.add_simple_command( "My guts (CAN)" 	);
	molecules.add_simple_command( "Drawing" 		);
	molecules.add_simple_command( "Camera" 		);


	molecules.add_callback( 0,  show_molecule_screens  );
	molecules.add_callback( 1,  show_molecule_screens  );	
	molecules.add_callback( 2,  show_molecule_screens  );	
	molecules.add_callback( 3,  show_molecule_screens  );	
	molecules.add_callback( 4,  show_molecule_screens  );	
	molecules.add_callback( 5,  show_molecule_screens  );	
	molecules.add_callback( 6,  show_molecule_screens  );

}

int show_game_screens( void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex) 
	{
	// GAMES start at 30!
	case 0: init_cards_only	();		break;
	case 1: init_blackjack	();		break;
	case 2: init_hearts		();		break;
	case 3: init_reversi	();		break;
	case 4: init_battleships();		break;	
	default: break;
	}
}

void init_game_menu()
{
	games.add_simple_command( "Show deck" 	);
	games.add_simple_command( "Twenty One" 	);
	games.add_simple_command( "Hearts" 		);
	games.add_simple_command( "Reversi" 	);
	games.add_simple_command( "Battleships" );

	games.add_callback( 0,  show_game_screens  );
	games.add_callback( 1,  show_game_screens  );	
	games.add_callback( 2,  show_game_screens  );	
	games.add_callback( 3,  show_game_screens  );	
	games.add_callback( 4,  show_game_screens  );
}

int show_graph_screens( void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex) 
	{
	case 0: init_line_graph		();		break;		
	case 1: init_histogram_graph();		break;
	case 2: init_bar_graph		();		break;
	case 3: init_scatter_graph	();		break;
	case 4: init_combined_graph	();		break;
	default: break;
	}
}

void init_graph_menu()
{
	graphs.add_simple_command( "Line Graph" 	);
	graphs.add_simple_command( "Histogram" 		);
	graphs.add_simple_command( "Bar     graph" );
	graphs.add_simple_command( "Scatter graph" );
	graphs.add_simple_command( "Combo   graph" );

	graphs.add_callback( 0,  show_graph_screens  );
	graphs.add_callback( 1,  show_graph_screens  );	
	graphs.add_callback( 2,  show_graph_screens  );	
	graphs.add_callback( 3,  show_graph_screens  );	
	graphs.add_callback( 4,  show_graph_screens  );
}

HorizontalMenu system_hmenu;

void init_system_hmenu( )
{
	ifprintf("init_system_hmenu 1\n");
	init_atom_menu		(  );
	init_molecule_menu  (  );	
	init_game_menu 		(  );	
	init_graph_menu 	(  );
	ifprintf("init_system_hmenu 5\n");

	system_hmenu.m_entries.clear();
	system_hmenu.add_entry_text( "File" );
	system_hmenu.add_entry_text( "Edit" );	
	system_hmenu.add_entry_text( "View" );	
	system_hmenu.add_sub_menu( "Atoms",     &atoms 	  );
	system_hmenu.add_sub_menu( "Molecules", &molecules );
	system_hmenu.add_sub_menu( "Games",     &games  );
	system_hmenu.add_sub_menu( "Graphs",    &graphs );		
	ifprintf("init_avisual_menu done\n");
}


int handle_power_action( void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex) 
	{
	case 0: init_power_off		();		break;		
	case 1: init_power_restart  ();		break;
	case 2: init_power_sleep	();		break;
	case 3: init_logoff_user	();		break;
	default: break;
	}
}

void init_power_menu()
{
	power_menu.add_simple_command( "Restart" 	);
	power_menu.add_simple_command( "Log out" 	);
	power_menu.add_simple_command( "Sleep" 		);
	power_menu.add_simple_command( "Power Off"  );

	power_menu.add_callback( 0,  handle_power_action  );
	power_menu.add_callback( 1,  handle_power_action  );	
	power_menu.add_callback( 2,  handle_power_action  );	
	power_menu.add_callback( 3,  handle_power_action  );
}


//vector<IconView*>  soft_buttons;

void init_default_sidebar( SideBar* mSB )
{
	ifprintf("init_default_sidebar( ) \n");
	IconView* iv = new IconView( 64., 64. );
	iv->set_file( "/home/pi/bk_code/avisual/resources/calendar.jpg");
	iv->set_width_height( 64., 64.);
	mSB->add_control( iv );
	
	iv = new IconView( 64., 64. );
	iv->set_file( "/home/pi/bk_code/avisual/resources/settings.jpg");
	iv->set_width_height( 64., 64.);
	mSB->add_control( iv );

	iv = new IconView( 64., 64. );
	iv->set_file( "/home/pi/bk_code/avisual/resources/folder2.jpg");
	iv->set_width_height( 64., 64.);
	mSB->add_control( iv );
	
	iv = new IconView( 64., 64. );
	iv->set_file( "/home/pi/bk_code/avisual/resources/cmd_line.jpg");
	iv->set_width_height( 64., 64.);
	mSB->add_control( iv );
	
	iv = new IconView( 64., 64. );
	iv->set_file( "/home/pi/bk_code/avisual/resources/calendar.jpg");
	iv->set_width_height( 64., 64.);
	mSB->add_control( iv );
	
	iv = new IconView( 64., 64. );
	iv->set_file( "/home/pi/bk_code/avisual/resources/calendar.jpg");
	iv->set_width_height( 64., 64.);
	mSB->add_control( iv );

	ifprintf("init_default_sidebar( SideBar* mSB )\n");
} 


