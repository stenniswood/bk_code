#include <shapes.h>
#include <fontinfo.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <pthread.h>
#include "bk_system_defs.h"
#include "pican_defines.h"
#include "can_txbuff.h"
#include "packer.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"

#include "can_id_list.h"
#include "cmd_process.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "catagorization.h"

#include "window_layouts.hpp"
#include "callbacks.hpp"
#include "vector_file.hpp"
#include "mouse.h"
#include "test_layouts.hpp"

#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include "bcm_host.h"
#include "ilclient.h"
#include "AUDIO_device.h"

#include "audio_app.hpp"
#include "client_memory.hpp"
#include "visual_memory.h"
#include "audio_memory.h"
#include "CAN_memory.h"
#include "client_list_control.hpp"
#include "home_screen.hpp"







/*
	What's displayed here is going to be under the control of bkInstant.
	We'll start by showing the incoming text.
*/
/***********************************************************************/
extern int DisplayNum;
#define Debug 0

pthread_t udp_thread;  
void create_threads()
{	
	const char *message1 = "every second";	
	// unknown yet :
}
/***********************************************************************/
int bkInstant_connected = FALSE;
int can_connected = FALSE;

void init_ipc( const char* mVectorFileName )
{
	// Ready-to-send, Receive buffer full 
	create_threads();
	
	bkInstant_connected = connect_shared_client_memory( FALSE );
	printf("init_ipc %d \n", bkInstant_connected );
	
	//bkInstant_connected = connect_shared_abkInstant_memory(); 

	// AUDIO : 
	if (aud_allocate_memory() == -1)
	{
		printf("Cannot allocate memory.\n");
		return;
	}
	aud_attach_memory();
	
	can_connect_shared_memory(FALSE);
}

void help()
{
	printf("A Visual - this task is the adrenaline general purpose \n"			);
	printf("           graphic interface.  It connects to bkInstant\n"			);
	printf("    and displays the results of user requests. Video input\n"		);
	printf("    graphs realtime, images, files, and incoming announcements.\n"	);
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


int icount=0;
int Last_Retrieved_Number=0;

void gui_interface()
{
	static int left_mouse_button_prev  = 0;
	static int right_mouse_button_prev = 0;

	// HANDLE MOUSE EVENTS :
	int result = mouse_timeslice();
	MainDisplay.end_screen();			// 

	Control* object_clicked = NULL;
	int x = round(mouse.x);
	int y = round(mouse.y);
	if (result == LEFT_BUTTON_DOWN)
	{
		//if (left_mouse_button_prev == 0)
		{	
			//printf(" Left button clicked!  mousex=(%d,%d)\n", x,y);
			object_clicked = MainDisplay.HitTest( x, y );
			if (object_clicked)
			{
				//printf("clicked an object %x!\n", object_clicked);
				int num = object_clicked->onClick( x, y );
				UpdateDisplaySemaphore=1;
				//printf("clicked an object - called onClick() DONE\n");				
			}  
			left_mouse_button_prev = result;
		}
	} 
	else if (result == RIGHT_BUTTON_DOWN)
	{
	}
	else
	{
		object_clicked = MainDisplay.HitTest( x, y );
		//if (object_clicked)
		{
			//object_clicked->onHover( x, y );
			//if (object_clicked->invalidated==true)
			//	UpdateDisplaySemaphore=1;
		}
	}

	// Scan for objects which have been invalidated and need redrawing.
	UpdateDisplaySemaphore = MainDisplay.any_invalid_children();
	//printf("any_invalid_children= %d\n", UpdateDisplaySemaphore );			

	if (ipc_memory_client)
		if (cli_is_new_update())
		{
			Last_Retrieved_Number = ipc_memory_client->NumberClients;
			UpdateDisplaySemaphore = 1;
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
		save_mouse();		
		//printf("done with save_mouse()\n");		
	}
}

// Where does it evalutate Menu's and call their callbacks?



void sequencer_interface()
{	/* select script, play, loop, link, etc */ }

void ethernet_interface()		/* wifi comms */
{
	if ( cli_is_new_update() )
	{	
		if (Debug) printf("New sentence : %s\n", get_sentence());
		CmdText.set_text( get_sentence() );		
		AvailClients.update_available_client_list();
		AvailClients.Invalidate();		
		UpdateDisplaySemaphore=1;			
	}
	if ( cli_is_new_connection_status() )
	{	
		if (Debug) printf("New status : %s\n", get_connection_status());	
		ConnectionStatus.set_text( get_connection_status() );		
		UpdateDisplaySemaphore=1;
	}
}

void voice_interface()
{	/* NLP/Synthesis */ }
void behavior_interface()
{	
//	if (Classify)
//		classify_timeslice_100ms();
	/* Tasks, User Responses */
}

extern Wave dWave;

/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.		*/
int main( int argc, char *argv[] )
{
	printf("======= main() ==============\n");		
	init_ipc("simple_walk.csv");
	DisplayNum = 4;

	printf("======= Checking command line arguments ==============\n");	
	print_args( argc, argv );
	if (argc>1)
	{
		if ((strcmp(argv[1], "help") == 0))
		{
			print_test_list();
			help();			
			return 0;
		} else {
			DisplayNum = atoi(argv[1]);
			if (Debug) printf("Loading Test Screen # %d\n", DisplayNum);
			load_test_screen(DisplayNum); 
		} 
	} else 
		init_home_screen();		// opening screen!	Located in window_layouts.cpp
		
	UpdateDisplaySemaphore=1;
	MainDisplay.onCreate();

	//audio_file_open();
	//play_waveform( &dWave, 1 );
	//audio_play();

	printf("================= Main Loop ==========================\n");	
	while (1)
	{	
		gui_interface();
		//if (bkInstant_connected)
		//	ethernet_interface();

		MainDisplay.idle_tasks();		
		//printf("done with MainDisplay.idle_tasks()\n");
		
		// Want to be able to open the screen leaving everything as is.
		// then do an end again to render the mouse.

		/*sequencer_interface();
		voice_interface();
		behavior_interface(); */
	}
}
