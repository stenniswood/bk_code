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
#include "edit_box.hpp"
 

TextView 		ConnectionStatus( 50, 700, 500, 50 	);
ClientListPanel	AvailClients 	( 20, 700, 160, 50	);
TextView 		RobotResponse 	( 50, 700, 350, 200 );
EditBox 		ClientInputEdit( 50, 700, 450, 355 );

static char ConnectionStatusText[128];
static char CommandText[128];

void init_home_screen()
{
	RobotResponse.set_text			( "Not Connected" );		//ConnectionStatus
	if (ipc_memory_client)
		RobotResponse.set_text			( ipc_memory_client->Sentence );		//ConnectionStatus
	RobotResponse.set_text_size 		( 16.0 		 );
	RobotResponse.set_text_color		( 0xCFFF0000 );
	RobotResponse.set_background_color( 0xFFFFFf00 );
	
	// This should be hidden until asked for via voice.
	//AvailClients.move_to( 20, 75 );

	strcpy(CommandText, "connect to 192.168.2.14, Robot, show me the accuracy of your positioning. Show me a histogram for right leg positions.\
	 Lift your left leg.  Raise both arms.  Go to the bedroom and get my shoes." );	
	ClientInputEdit.set_text			( CommandText );
	ClientInputEdit.set_text_size 		( 16.0 );	

//	MainDisplay.add_object( &ConnectionStatus );
//	MainDisplay.add_object( &adren_board_list );

	// Add to display manager:
	MainDisplay.remove_all_objects(		  );
	MainDisplay.add_object( &ClientInputEdit   );
	MainDisplay.add_object( &RobotResponse 	  );
	MainDisplay.add_object( &AvailClients );
	MainDisplay.load_resources();
}


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
	//pack_sample_window();	
	*/