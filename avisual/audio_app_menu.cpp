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
#include "draw_app.hpp"
#include "audio_app_menu.hpp"
#include "audio_app.hpp"


VerticalMenu   audio_master		(-1,-1);
VerticalMenu   audio_file		(-1,-1);
VerticalMenu   audio_view		(-1,-1);
VerticalMenu   audio_play_menu	(-1,-1);
VerticalMenu   audio_effects	(-1,-1);


int handle_audio_master_menu( void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex)
	{
		case 0:	audio_app->About			();		break;
		case 1:	audio_app->Preferences		();		break;
		case 2:	audio_app->file_quit		();		break;
		default: break;
	}
}

int handle_audio_file_menu( void* menuPtr, int mMenuIndex )
{
	printf("handle_audio_file_menu()\n");
	switch(mMenuIndex)
	{
		case 0:	audio_app->file_new			();		break;
		case 1:	audio_app->file_open		();		break;
		case 2:	audio_app->file_open_recent	();		break;	
		case 3:	audio_app->file_save		();		break;
		case 4:	audio_app->file_save_as		();		break;
		case 5:	audio_app->file_quit		();		break;
		default: break;
	}
	printf("handle_audio_file_menu() - Done!\n");
}

int handle_audio_view_menu(void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex) 
	{	
		case 0:	audio_app->zoom_in	();			break;
		case 1:	audio_app->zoom_out	();			break;
		case 2:	audio_app->show_mixer();		break;
		case 3:	audio_app->show_frequency();	break;
		case 4:	audio_app->show_fft  ();		break;
		default: break;
	}
}
int handle_audio_play_menu(void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex) 
	{	
		case 0:	audio_app->audio_play();		break;
		case 1:	audio_stop	();					break;
		case 2:	audio_rewind();					break;
		case 3:	audio_select_output_device();	break;
		case 4:	audio_record_from();			break;
		case 5:	audio_broadcast_to();			break;
		default: break;
	}
}
int handle_audio_effects_menu(void* menuPtr, int mMenuIndex )
{
	switch(mMenuIndex) 
	{	
		case 0:	audio_reverb			();			break;
		case 1:	audio_equalize			();			break;
		case 2:	audio_backwards			();			break;
		case 3:	audio_convert_to_mono	();			break;
		case 4:	audio_convert_to_stereo ();			break;
		default: break;
	}
}
//////////////////////////////////////////////////////////
void init_audio_master_menu( )
{
	audio_master.add_simple_command( "About Audio Master" 	);
	audio_master.add_simple_command( "Preferences" 			);
	audio_master.add_simple_command( "Quit" 				);	
	audio_master.add_callback_all_items( handle_audio_master_menu );	
}
void init_audio_File_menu( )
{
	audio_file.create_std_file_menu();
	audio_file.add_callback_all_items( handle_audio_file_menu );	
}

void init_audio_View_menu( )
{
	audio_view.add_simple_command( "Zoom In"   );
	audio_view.add_simple_command( "Zoom Out"  );
	audio_view.add_simple_command( "Show Mixer"  );
	audio_view.add_simple_command( "Show Frequency"  );
	audio_view.add_simple_command( "Show FFT"  );
	audio_view.add_callback_all_items( handle_audio_view_menu  );
}

void init_audio_Play_menu( )
{
	audio_play_menu.add_simple_command( "Play Waveform"  );
	audio_play_menu.add_simple_command( "Stop Waveform"  );
	audio_play_menu.add_simple_command( "Rewind"  		 );
	audio_play_menu.add_simple_command( "Select Output"  );	
	audio_play_menu.add_simple_command( "Record"  );	
	audio_play_menu.add_simple_command( "Record from..."  );		
	audio_play_menu.add_simple_command( "Broadcast to..."  );	
	audio_play_menu.add_callback_all_items( handle_audio_play_menu  );
}

void init_audio_Effects_menu( )
{
	audio_effects.add_simple_command( "Reverb"  );
	audio_effects.add_simple_command( "Equalize"  );
	audio_effects.add_simple_command( "Backwards"  );
	audio_effects.add_simple_command( "Convert to Mono"  );	
	audio_effects.add_simple_command( "Convert to Stereo"  );		
	audio_effects.add_callback_all_items( handle_audio_effects_menu  );
}

void init_audio_menu( HorizontalMenu* audio_menu )
{
	printf("init_audio_menu() \n");
	init_audio_master_menu( );
	printf("init_audio_menu() 2\n");
	init_audio_File_menu();
	printf("init_audio_menu() 3\n");	
	init_audio_Play_menu();
	printf("init_audio_menu() 4\n");	
	init_audio_View_menu(); 
	printf("init_audio_menu() 5\n");	
	init_audio_Effects_menu();
	printf("init_audio_menu() 6\n");
	
	printf("init_audio_menu() %4x \n", audio_menu);
	audio_menu->m_entries.clear();
	audio_menu->add_sub_menu( "Audio Master", &audio_master );
	audio_menu->add_sub_menu( "File",     &audio_file 		);
	printf("init_audio_menu() 2 \n");

	audio_menu->add_sub_menu( "View",     &audio_view 		);
	audio_menu->add_sub_menu( "Play",     &audio_play_menu 	);
	audio_menu->add_sub_menu( "Effects",  &audio_effects 	);
}

