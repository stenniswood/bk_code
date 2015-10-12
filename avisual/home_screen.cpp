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
 

TextView 	ConnectionStatus( 50, 700, 500, 50 );
ClientList	AvailClients (20, 650, 160, 50);
TextView 	CmdText( 50, 700, 350, 200 );
static char ConnectionStatusText[128];
static char CommandText[128];

void init_home_screen()
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

	strcpy(CommandText, "Robot, show me the accuracy of your positioning. Show me a histogram for right leg positions.\
	 Lift your left leg.  Raise both arms.  Go to the bedroom and get my shoes." );	
	CmdText.set_text			( CommandText );
	CmdText.set_text_size 		( 16.0 		 );
	CmdText.set_text_color		( 0x9FFF0000 );
	CmdText.set_background_color( 0x7FFfFf00 );
	
	// This should be hidden until asked for via voice.
	AvailClients.move_to( 20, 75 );
	AvailClients.calc_metrics();
	AvailClients.set_headings();	
	AvailClients.update_available_client_list();
	
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
	// Add to display manager:
	MainDisplay.remove_all_objects(		);
//	MainDisplay.add_object( &ConnectionStatus );
	MainDisplay.add_object( &CmdText 	);
	MainDisplay.add_object( &AvailClients );
//	MainDisplay.add_object( &adren_board_list );
	MainDisplay.load_resources();
}