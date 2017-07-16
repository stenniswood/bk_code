/* Rember to Build Adrenaline .a libraries.  See bk_code/core/Adrenaline/

*/
#include <shapes.h>
#include <fontinfo.h>
#include <sys/time.h>
#include <string>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include "bk_system_defs.h"

#include "leds.h"
#include "vector_math.h"
#include "mouse_oop.hpp"
#include "touch_gesture.hpp"
#include "keyboard_dev.hpp"
#include "test_layouts.hpp"


#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include "bcm_host.h"
#include "ilclient.h"

//#include "client_memory.hpp"
//#include "display_manager.hpp"

#include "mouse_oop.hpp"

//#include "home_screen.hpp"
//#include "ipc_mem_bkinstant.h"


/***********************************************************************/
/*
	What's displayed here is going to be under the control of bkInstant.
	We'll start by showing the incoming text.
*/
/***********************************************************************/
int DisplayNum=-1;
#define Debug 1

/***********************************************************************/
void help()
{
	printf("Visual Hello - Sample to demonstrate Adrenalina Windowing environment. \n");
	printf(" 	\n");
	printf(" 	\n");
}

void print_args(int argc, char *argv[])
{
	printf("ARGS: %d", argc);
	for (int i=0; i< argc; i++)
	{
		printf(" %s ", argv[i] );
	}
	printf("\n");
}

Mouse mouse;

int icount=0;
int Last_Retrieved_Number=0;
int UpdateDisplaySemaphore=1;

/*void gui_interface()
{
//	static int left_mouse_button_prev  = 0;
	static int right_mouse_button_prev = 0;

	// HANDLE MOUSE EVENTS :
	int result = mouse.time_slice();
	MainDisplay.end_screen(); 

	int child_ctrl;
	Control* object_clicked = NULL; 
	int x = round(mouse.x);
	int y = round(mouse.y);
	if (MainDisplay.mouse_capturing==true)
	{
		MainDisplay.relay_mouse();
	}
	else if (result == LEFT_BUTTON_DOWN)
	{
		// Mouse handle_event() ensures only UNHANDLED events (no held down button)!
		Dprintf(" Left button clicked!  mousexy=(%d,%d)\n", x,y );
		object_clicked = MainDisplay.HitTest( x, y );
		if (object_clicked)
		{ 
			child_ctrl = object_clicked->onClick( x, y );
			Dprintf("clicked an object %s %p  child=%d!\n", object_clicked->class_name, object_clicked, child_ctrl);
			UpdateDisplaySemaphore=1;
		}
	} 
	else if (result == RIGHT_BUTTON_DOWN)
	{
		if (right_mouse_button_prev == 0)
		{
			right_mouse_button_prev = result;
		}
	}
	else
	{
		//object_clicked = MainDisplay.HitTest( x, y );
		//if (object_clicked)
		{
			//object_clicked->onHover( x, y );
			//if (object_clicked->invalidated==true)
			//	UpdateDisplaySemaphore=1;
		}
	}

	// Scan for objects which have been invalidated and need redrawing.
	BOOL invalid = MainDisplay.any_invalid_children();
	// if the invalid child is now hiden, or moved, redraw everything underneath it!
	if (invalid)
	{	
		UpdateDisplaySemaphore = 1;
		//printf("Test for invalid children.  Found! \n");	
	}

	MainDisplay.draw_invalid_children();
	//printf("MainDisplay.draw_invalid_children done. \n");
	MainDisplay.update_invalidated();		
	//printf("MainDisplay.update_invalidated. \n");

//	if (cli_is_new_client())
	{
//		Last_Retrieved_Number = ipc_memory_client->NumberClients;
//		UpdateDisplaySemaphore = 1;
	}
    ////////////////////////////////////////
	if (UpdateDisplaySemaphore)
	{
		UpdateDisplaySemaphore = 0;
		//update_to_controls();
		// hide mouse: (mouse has the old image stored in it's buffer)
		// Only draw the invalidated() items.
		
		MainDisplay.draw();		
		//printf("MainDisplay.draw() Done.\n");
		MainDisplay.update_invalidated();
		//printf("MainDisplay.update_invalidated() Done.\n");
				
		// so now just resave the buffer with new contents:
//		mouse.save_mouse();
		//printf("done with save_mouse()\n");
	}
} */

char shared_mem_ids_base_path[20];


// Where does it evalutate Menu's and call their callbacks?

int main( int argc, char *argv[] )
{
	printf("======= main() ==============\n");
	DisplayNum = 4;

	if (argc>1)
	{
		printf("======= Checking command line arguments ==============\n");	
		print_args( argc, argv );

		if ((strcmp(argv[1], "help") == 0))
		{
			print_test_list();
			help();			
			return 0;
		} else {
			DisplayNum = atoi(argv[1]);
			if (Debug) printf("Loading Test Screen # %d\n", DisplayNum);
			//load_test_screen(DisplayNum); 
		} 
	} 
//	else 
//		init_simple_button_test();
//		init_home_screen();		// opening screen!	

	printf("================= init_home_screen() ==========================\n");	
	UpdateDisplaySemaphore=1;
	//MainDisplay.onCreate();
	dev_keyboard_init();	
	mouse.Initialize(); 
	
	init_screen();
	start_screen();
	onesimus();
	end_screen();
		
	printf("================= Main Loop ==========================\n");	
	while (1)
	{	
//		abkInstant_interface();
//		ethernet_interface();
	//	gui_interface();
//		can_interface();

//		MainDisplay.idle_tasks();		

		// Want to be able to open the screen leaving everything as is.
		// then do an end again to render the mouse.
	}
}
