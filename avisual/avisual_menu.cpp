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

#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "avisual_menu.hpp"
#include "test_layouts.hpp"
#include "draw_app.hpp"
#include "draw_app2.hpp"
#include "audio_app.hpp"
#include "test_combo_layouts.hpp"
#include "power.hpp"
#include "blackjack_app.hpp"
#include "file_editor_app.hpp"
#include "video_app.hpp"
#include "window_layouts.hpp"
#include "robot_app.hpp"
#include "datalog_graph.hpp"


HorizontalMenu hm		(-1,-1);
VerticalMenu   view     (-1,-1);
VerticalMenu   atoms    (-1,-1);
VerticalMenu   molecules(-1,-1);
VerticalMenu   apps		(-1,-1);
VerticalMenu   games	(-1,-1);
VerticalMenu   graphs	(-1,-1);
VerticalMenu   power_menu(-1,-1);		// power switch pushed.

#define Debug 0
#define ifprintf if (Debug) printf

int show_view_screens (void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{	
	case 0:	init_datalog_graph	();		break;
	case 1:	init_listbox		();		break;
	case 2:	init_tab_listbox	();		break;
	case 3:	init_grid_test		();		break;
	case 4:	init_motor_gui_test	();		break;
	default: break;
	}	
}

void init_view_menu()
{
	dprintf("init_view_menu()\n");
	view.add_simple_command( "Data log graph"		);
	view.add_simple_command( "ListBox demo" 		);
	view.add_simple_command( "Tabular ListBox demo"	);
	view.add_simple_command( "Grid test demo"		);	
	view.add_simple_command( "Motor control demo"	);		
	view.add_callback_all_items( show_view_screens );	
}


int show_atom_screens (void* menuPtr, int mMenuIndex, Application* mApp )
{
	//ifprintf("init_display() DisplayNum=%d\n", number);
	switch(mMenuIndex) 
	{	
	case 0:	init_simple_button_test	();		break;
	case 1:	init_gyro_view		    ();		break;
	case 2:	init_textview_test		();		break;
	case 3: init_progressbar_test	();		break;
	case 4: init_radio_button_test	();		break;
	case 5: init_check_button_test	();		break;
//	case 6: init_simple_text		();		break;
	case 6: init_loadcell_view		();		break;
	case 7: init_simple_path		();		break;
	case 8: pack_sample_window		();		break;
	case 9: init_frame_window		();		break;		
	case 10: init_textfile_view		();		break;		
//	case 11: init_horiz_menu		();		break;			
//	case 12: init_vert_menu			();		break;
	case 11: init_combo_menu		();		break;
	case 12: init_spinner_menu		();		break;
	case 13: init_listbox			();		break;	
	case 14: init_tab_listbox		();		break;			
	case 15: /*init_CAN_msg_view	();*/		break;
	default: break;
	}
}


void init_atom_menu()
{
	if (Debug) printf("init_atom_menu()\n");
	atoms.add_simple_command( "init_simple_button_test" );
	if (Debug) printf("init_atom_menu() 1\n");
	atoms.add_simple_command( "init_gyro_view" );
	atoms.add_simple_command( "init_textview_test"		);
	atoms.add_simple_command( "init_progressbar_test"   );
	atoms.add_simple_command( "init_radio_button_test"  );
	atoms.add_simple_command( "init_check_button_test"	);
	//atoms.add_simple_command( "init_simple_text"		);
	atoms.add_simple_command( "init_loadcell_view"		);	
	atoms.add_simple_command( "init_simple_path"		);
	atoms.add_simple_command( "pack_sample_window"		);
	atoms.add_simple_command( "init_frame_window"		);
	atoms.add_simple_command( "init_textfile_view"		);	
	//atoms.add_simple_command( "init_horiz_menu"			);
	//atoms.add_simple_command( "init_vert_menu"			);
	atoms.add_simple_command( "init_combo_menu"			);
	atoms.add_simple_command( "init_spinner_menu"		);
	//atoms.add_simple_command( "init_listbox"			);
	//atoms.add_simple_command( "init_tab_listbox"		);
	atoms.add_simple_command( "init_CAN_msg_view"		);
	atoms.add_callback_all_items( show_atom_screens );
	
}

int show_molecule_screens( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: init_image_gallery		();		break;		
	case 1: init_okay_cancel_dlg	();		break;
	case 3: init_directory_lb_test	();		break;
	case 4: init_file_browser		();		break;			
	default: 	break;
	}
}
void init_molecule_menu()
{
	molecules.add_simple_command( "Image Gallery" 	);
	molecules.add_simple_command( "Okay Cancel Dialog" );
	molecules.add_simple_command( "Directory lb" 	);
	molecules.add_simple_command( "File Browser" 	);				
	molecules.add_callback_all_items( show_molecule_screens );
}

/*
	All apps are currently invoked by a pull down menu called "Apps".  
	This begins each app when selected.
*/
int handle_apps_screens( void* menuPtr, int mMenuIndex, Application* mApp )
{
	Application* tmp_app=NULL;
	printf("handle_apps_screens()\n");
	switch(mMenuIndex) 
	{
	case 0: init_image_gallery		();		break;		
	case 1: if (audio_app==NULL)
				audio_app = new AudioApp();
			MainDisplay.start_app( audio_app );
			break;
	case 2: init_file_browser		();		break;
	case 3: init_CAN_app();
			MainDisplay.start_app( can_app );	
			break;
	case 4: init_drawing_app		();		break;	
	case 5: init_file_editor_app	();		break;
	case 6: init_pot_objs			();		break;	
	case 7: init_Video_app			();		break;
	case 8: init_robot_app();		printf(" --- init_robot_panel --- \n");  break;
	default: 	break;
	}
	printf("handle_apps_screens() done\n");
}

void init_apps_menu()
{
	apps.add_simple_command( "Image Gallery" 	);
	apps.add_simple_command( "Audio Master" 	);
	apps.add_simple_command( "File Browser" 	);				
	apps.add_simple_command( "CAN App" 		);
	apps.add_simple_command( "Drawing" 		);
	apps.add_simple_command( "File Editor" 	);
	apps.add_simple_command( "Potentiometer");	// simple app showing Big Motor board potentiometer/encoder results.
	apps.add_simple_command( "Camera" 		);
	apps.add_simple_command( "Robot" 		);
	apps.add_callback_all_items( handle_apps_screens );
}

int show_game_screens( void* menuPtr, int mMenuIndex, Application* mApp )
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
	games.set_state( MENU_STATE_GRAYED, 4 );
	games.add_callback_all_items( show_game_screens  );
}

int show_graph_screens( void* menuPtr, int mMenuIndex, Application* mApp )
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
	graphs.add_callback_all_items( show_graph_screens  );
}

HorizontalMenu system_hmenu;

void init_system_hmenu( )
{
	if (Debug) printf("init_system_hmenu 1\n");
	init_view_menu		(  );
	init_atom_menu		(  );
	init_molecule_menu  (  );	
	init_apps_menu		(  );
	init_game_menu 		(  );	
	init_graph_menu 	(  );
	if (Debug) printf("init_system_hmenu 5\n");

	system_hmenu.m_entries.clear();
	system_hmenu.add_entry_text( "File" );
	system_hmenu.add_entry_text( "Edit" );	
	system_hmenu.add_sub_menu( "View",      &view 	);
	system_hmenu.add_sub_menu( "Atoms",     &atoms 	);
	system_hmenu.add_sub_menu( "Molecules", &molecules );
	system_hmenu.add_sub_menu( "Apps", 		&apps 	);
	system_hmenu.add_sub_menu( "Games",     &games  );
	system_hmenu.add_sub_menu( "Graphs",    &graphs );		
	if (Debug) printf("init_avisual_menu done\n");
}


int handle_power_action( void* menuPtr, int mMenuIndex, Application* mApp )
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
	power_menu.add_callback_all_items( handle_power_action );
}


//vector<IconView*>  soft_buttons;

void init_default_sidebar( SideBar* mSB )
{
	if (Debug) printf("init_default_sidebar( ) \n");
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

	if (Debug) printf("init_default_sidebar( SideBar* mSB )\n");
} 


