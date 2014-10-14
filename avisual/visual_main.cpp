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
#include "pican_defines.h"
#include "CAN_Interface.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "can_txbuff.h"
#include "packer.h"
#include "board_list_oop.hpp"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "catagorization.h"
#include "line_graph.hpp"
#include "leveler.hpp"
#include "histogram.hpp"
#include "window_layouts.hpp"
#include "callbacks.hpp"
#include "display_manager.hpp"
#include "vector_file.hpp"
#include "ipc_mem_bkinstant.h"
#include "mouse.h"
#include "test_layouts.hpp"

#include "visual_memory.h"


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

void init( const char* mVectorFileName )
{
	// Ready-to-send, Receive buffer full 
	create_threads();
	bkInstant_connected = connect_shared_abkInstant_memory(); 
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

void load_screen(int mDisplay)
{
	printf("init_display() DisplayNum=%d\n", mDisplay);
	switch(mDisplay)
	{
	case 0:	//init_simple_test();
			load_test_screen(0);
			//load_test_screen(1);
			break;
	case 1:	init_pot_objs();
			break;
	case 2:	init_tilt_objs();
			break;
	case 3: init_control_test();
			break;
	case 4: init_avisual();
			break;
	default:	
			break;
	}
	if (Debug) printf("init_display() completed!\n");
}

int icount=0;
int Last_Retrieved_Number=0;

void gui_interface()
{
	// HANDLE MOUSE EVENTS:
	int result = mouse_timeslice();
	MainDisplay.end_screen();

	Control* object_clicked = NULL;
	int x = round(mouse.x);
	int y = round(mouse.y);
	if (result == LEFT_BUTTON_DOWN) 
	{		
		//printf(" Left button clicked!  mousex=(%d,%d)\n", x,y);
		object_clicked = MainDisplay.HitTest( x, y );
		if (object_clicked)
		{
			printf("clicked an object %x!\n", object_clicked);
			int num = object_clicked->onClick( x, y ); 
			UpdateDisplaySemaphore=1;
		} else
			printf(" hit test returned null\n");
	}

	if (ipc_memory_avis->NumberClients != Last_Retrieved_Number)
	{
		update_available_client_list();
		Last_Retrieved_Number = ipc_memory_avis->NumberClients;
		UpdateDisplaySemaphore = 1;
	}
    ////////////////////////////////////////
	if (UpdateDisplaySemaphore)
	{
		UpdateDisplaySemaphore = 0;
		//update_to_controls();
		MainDisplay.draw();
	}
}

void sequencer_interface()
{	/* select script, play, loop, link, etc */ }

void ethernet_interface()		/* wifi comms */
{
	if ( is_new_sentence() )
	{	
		printf("New sentence : %s\n", get_sentence());
		CmdText.set_text( get_sentence() );		
		UpdateDisplaySemaphore=1;			
	}
	if ( is_new_connection_status() )
	{		
		printf("New status : %s\n", get_connection_status());	
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

/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.		*/
int main( int argc, char *argv[] )
{
	print_args( argc, argv );
	int first_param = 1;		// sudo ./pican cfg 
	int value    	= 0;
	DisplayNum      = 0; 
	init("simple_walk.csv");
	
	if (argc>1)
	{
		if ((strcmp(argv[first_param], "help") == 0))
		{
			print_test_list();
			help();			
			return 0;
		} else {
			DisplayNum = atoi(argv[first_param]);
			printf("Loading Test Screen # %d\n", DisplayNum);
			load_test_screen(DisplayNum);
		}
	} else 
		load_screen(4);

	UpdateDisplaySemaphore=1;
	printf("================= Checking command line arguments ========================\n");

    if (argc>1) {
		if (strcmp(argv[first_param], "help") == 0)
		{
			help();
			exit(1);
		}
		if (strchr(argv[first_param], 'a') > 0)		
		{		
		}

		if (argc>2) {
		}
	}

	printf("================= Main Loop ==========================\n");
	
	while (1)
	{	
		gui_interface();
		if (bkInstant_connected)
			ethernet_interface();

		// Want to be able to open the screen leaving everything as is.
		// then do an end again to render the mouse.
		
		
		
		/*sequencer_interface();
		voice_interface();
		behavior_interface(); */
	}
}

