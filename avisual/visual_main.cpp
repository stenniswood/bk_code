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
#include "mouse_oop.hpp"
#include "touch_gesture.hpp"
#include "keyboard_dev.hpp"
#include "GyroViewI.hpp"
#include "test_layouts.hpp"

#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include "bcm_host.h"
#include "ilclient.h"
#include "AUDIO_device.h"

#include "audio_app.hpp"
#include "client_memory.hpp"
#include "visual_memory.hpp"
#include "audio_memory.h"
#include "CAN_memory.h"

#include "client_list_control.hpp"
#include "home_screen.hpp"
#include "can_sql_logger.hpp"
#include "serial_loadcell.hpp"
#include "CAN_dispatcher.hpp"
#include "ipc_mem_bkinstant.h"

#include "fuse_ag.h"
/***********************************************************************/
/*
	What's displayed here is going to be under the control of bkInstant.
	We'll start by showing the incoming text.
*/
/***********************************************************************/
extern int DisplayNum;

#define Debug 1

void* serial_interface(void* );
pthread_t serial_leftfoot_thread_id;
pthread_t serial_rightfoot_thread_id;
//pthread_t udp_thread;  
LoadCell_SerialInterface left_foot;
LoadCell_SerialInterface right_foot;


void create_threads()
{
	int iret1 = pthread_create( &serial_leftfoot_thread_id, NULL, serial_interface, (void*) &left_foot);
	if (iret1)
	{
		fprintf(stderr,"Error - Could not create right_foot thread. return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	} 	

	int iret2 = pthread_create( &serial_rightfoot_thread_id, NULL, serial_interface, (void*) &right_foot);
	if (iret2)
	{
		fprintf(stderr,"Error - Could not create right_foot thread. return code: %d\n",iret2);
		exit(EXIT_FAILURE);
	}
}

/***********************************************************************/
int bkInstant_connected = FALSE;
int can_connected 		= FALSE;
int audio_mem_connected = FALSE;			// audio memory is not necessary to hear audio.

aVisualMemory avisual_mem;

void init_ipc( )
{
	printf("============== IPC MEMORY ==============\n");
	bkInstant_connected = connect_shared_client_memory( FALSE );
	if (bkInstant_connected)
		printf("IPC Client - attached to memory.\n");
	else
		printf("IPC Client - unavailable.\n");
	
	avisual_mem.connect_shared_memory(TRUE);		// data from Instant server arrives here.

	// AUDIO : 
	audio_mem_connected = audio_connect_shared_memory(TRUE);
	if (audio_mem_connected)
		printf("IPC Audio - attached to memory.\n");
	else
		printf("IPC Audio - unavailable.\n");

	can_connected = can_connect_shared_memory(FALSE);
	if (can_connected)
		printf("IPC CAN - attached to memory.\n");
	else
		printf("IPC CAN - unavailable.\n");

	printf("=====================================\n");
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

DraggerGesture mouse; 

void can_interface()
{
	static int	m_rx_tail =0;
	static int	m_rx_tail_laps =0;
	struct sCAN* ptr=NULL;
	
	// Re-check availability!
	if (can_connected==FALSE)		
		can_connected = can_connect_shared_memory(FALSE);
	if (can_connected)
		while (shm_isRxMessageAvailable( &m_rx_tail, &m_rx_tail_laps) )
		{	
			ptr = shm_GetNextRxMsg (&m_rx_tail, &m_rx_tail_laps);
			if (ptr) 
			{
				dispath_to_all_can_receivers(ptr);
			}
		}
}

void gui_interface()
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

	if (cli_is_new_client())
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
//		mouse.save_mouse();
		//printf("done with save_mouse()\n");
	}
}

// Where does it evalutate Menu's and call their callbacks?

void sequencer_interface()
{	/* select script, play, loop, link, etc */ 
}


void abkInstant_interface()		/* wifi comms */
{
	if (avisual_mem.is_IPC_memory_available()==false)	return 0;
	
	if ( avisual_mem.is_new_connection_status() )		
	{	
		//if (Debug) 
		printf("New status : %s\n", avisual_mem.get_connection_status());	
		RobotResponse.set_text( avisual_mem.get_connection_status() );
		avisual_mem.ack_status_counter();
		UpdateDisplaySemaphore=1;
	}

	if ( avisual_mem.is_new_sentence() )	// a new request (sentence)	
	{
		printf("---CLIENT New Sentence! %s\n", avisual_mem.get_sentence() );
		ClientInputEdit.set_text( avisual_mem.get_sentence() ); 
		avisual_mem.ack_sentence_counter();
		UpdateDisplaySemaphore=1;			
		// Will want to parse text as well (sometimes & sometimes just relay): 
		//result = Parse_adrenaline_statement( theSentence, mh );	this should occur inside of avisual's parser!
	}
	
	if (0)
	{
		printf("---NEW CLIENT AVAILABLE! \n" );	// udp_thread found a new beacon.
		AvailClients.m_clients->update_available_client_list();
		AvailClients.Invalidate();
		UpdateDisplaySemaphore=1;					
	}
	if (0) // is_new_response() )
	{
		printf("---Receive New Response: %s\n", ipc_memory_client->Sentence);
		RobotResponse.set_text( ipc_memory_client->Sentence );		//ConnectionStatus
		printf("Receive New Response: %s\n", ipc_memory_client->Response);
		RobotResponse.Invalidate();
		cli_ack_response();
		UpdateDisplaySemaphore=1;			
	}
}

void ethernet_interface()		/* wifi comms */
{
	if (ipc_memory_client==NULL)  return ;
	
	if ( cli_is_new_connection_status() )		// not really used anymore.
	{	
		//if (Debug) printf("New status : %s\n", get_connection_status());	
		if (ipc_memory_client)
			RobotResponse.set_text( cli_get_connection_status() );
		cli_ack_connection_status();
		UpdateDisplaySemaphore=1;
	}
	if ( cli_is_new_client() )		// udp_thread found a new beacon.
	{
		printf("---NEW CLIENT AVAILABLE! \n" );
		AvailClients.m_clients->update_available_client_list();
		AvailClients.Invalidate();
		cli_ack_new_client();
		UpdateDisplaySemaphore=1;					
	}
	if ( cli_is_new_update() )		// a new request (sentence)
	{
		printf("---CLIENT New Sentence! \n" );		
		UpdateDisplaySemaphore=1;			
	}
	if (is_new_response() )
	{
		printf("---Receive New Response: %s\n", ipc_memory_client->Sentence);
		RobotResponse.set_text( ipc_memory_client->Sentence );		//ConnectionStatus
		printf("Receive New Response: %s\n", ipc_memory_client->Response);
		RobotResponse.Invalidate();
		cli_ack_response();
		UpdateDisplaySemaphore=1;			
	}
}

void* serial_interface(void* mParam)				// serial port used for arduino connections & GPS.
{
	LoadCell_SerialInterface* mFoot = (LoadCell_SerialInterface*) mParam;
	static char  app_name[12];
	static char  read_r[4];
	static char  no_write_w[4];
	static char  device_p[4];

	static char  read_[20];
	static char  no_write_[20];
	static char  device_[20];

	sprintf(app_name,   "./avisual");
	sprintf(read_r,     "-R");
	sprintf(no_write_w, "-w");
	sprintf(device_p,   "-p");

	sprintf(read_,     "ascii");
	sprintf(no_write_, "0");

	//printf("mFoot=%p;  left_foot=%p\n", mFoot, &left_foot);
	if (mFoot == &left_foot)	{
		sprintf(device_,   "/dev/ttyACM0");
		mFoot->_cl_rx_dump = 1;
		mFoot->_cl_rx_dump_ascii = 1;		
		mFoot->_cl_port = strdup("/dev/ttyACM0");
		mFoot->_cl_tx_bytes = 0;
		mFoot->left_foot = false;
	} else {
		mFoot->_cl_port = strdup("/dev/ttyACM1");	
		mFoot->_cl_rx_dump = 1;
		mFoot->_cl_rx_dump_ascii = 1;		
		mFoot->_cl_tx_bytes = 0;
		mFoot->left_foot = true;		
		//sprintf(device_,   "/dev/ttyACM1");
	}
	char * argv[] = { app_name, read_r, read_, no_write_w, no_write_, device_p, device_ }; 
	
	mFoot->serial_loadcell_main( 7, argv );	
	return NULL;
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
#include "calendar_entry.hpp"

/* WORK ON RECEIVE.  SOME ACTIVITY DETECTED WITH BUTTON PUSHES.
	Seems like functionality doesn't work without interrupts.  ie. flags 
	are only set when the Enable is.  maybe.		*/
int main( int argc, char *argv[] )
{ 
//	DLL_API 
//  void DLL_CALLCONV FreeImage_Initialise(BOOL load_local_plugins_only FI_DEFAULT(FALSE));
//    DLL_API FIBITMAP *DLL_CALLCONV FreeImage_Load(FREE_IMAGE_FORMAT fif, const char* filename, int flags FI_DEFAULT(0));

	printf("======= main() ==============\n");		
	create_threads();
	init_ipc( );
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
	} else 
		init_home_screen();		// opening screen!	

	printf("================= init_home_screen() ==========================\n");	
	UpdateDisplaySemaphore=1;
	MainDisplay.onCreate();
	dev_keyboard_init();

	sql_logger.connect_to_logger_db();
	
	//audio_file_open();
	//play_waveform( &dWave, 1 );
	//audio_play();
	mouse.Initialize(); 
	fuse_init();
	CAN_add_rx_callback( callback_tilt_reading );
	//CAN_add_rx_callback( callback_main );
	
	printf("================= Main Loop ==========================\n");	
	while (1)
	{	
		abkInstant_interface();
		ethernet_interface();
		gui_interface();
		can_interface();

		MainDisplay.idle_tasks();		
		//printf("done with MainDisplay.idle_tasks()\n");

		// Want to be able to open the screen leaving everything as is.
		// then do an end again to render the mouse.

		/*sequencer_interface();
		voice_interface();
		behavior_interface(); */
		//serial_interface();	on separate thread!			// serial port used for arduino connections & GPS.
	}

//	DLL_API void DLL_CALLCONV FreeImage_DeInitialise();
}

