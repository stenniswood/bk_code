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


//VerticalMenu   audio_master		(-1,-1);
//VerticalMenu   audio_file		(-1,-1);
VerticalMenu   audio_view		(-1,-1);
VerticalMenu   audio_play_menu	(-1,-1);
VerticalMenu   audio_effects	(-1,-1);


/*int handle_audio_file_menu( void* menuPtr, int mMenuIndex, Application* mApp )
{
	printf("handle_audio_file_menu()\n");
	switch(mMenuIndex)
	{
		case 0:	mApp->file_new		();		break;
		case 1:	mApp->file_open		();		break;
		case 2:	mApp->file_open_recent	();		break;	
		case 3:	mApp->file_save		();		break;
		case 4:	mApp->file_save_as	();		break;
		default: break;
	}
	printf("handle_audio_file_menu() - Done!\n");
}*/

int handle_audio_view_menu(void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{	
		case 0:	((AudioApp*)mApp)->zoom_in	();			break;
		case 1:	((AudioApp*)mApp)->zoom_out	();			break;
		case 2:	((AudioApp*)mApp)->show_mixer();		break;
		case 3:	((AudioApp*)mApp)->show_frequency();	break;
		case 4:	((AudioApp*)mApp)->show_fft  ();		break;
		default: break;
	}
} 
int handle_audio_play_menu(void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{	
		case 0:	((AudioApp*)mApp)->audio_play();		break;
		case 1:	audio_stop	();					break;
		case 2:	audio_rewind();					break;
		case 3:	audio_select_output_device();	break;
		case 4:	audio_record_from();			break;
		case 5:	audio_broadcast_to();			break;
		default: break;
	}
}
int handle_audio_effects_menu(void* menuPtr, int mMenuIndex, Application* mApp )
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
void init_audio_File_menu( )
{

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
	//audio_file.create_std_file_menu();
	//audio_file.add_callback_all_items( handle_audio_file_menu );	

	init_audio_Play_menu();
	printf("init_audio_menu() 4\n");	
	init_audio_View_menu(); 
	printf("init_audio_menu() 5\n");	
	init_audio_Effects_menu();
	printf("init_audio_menu() 6\n");	

}

