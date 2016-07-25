/*************************************************************************
NAME	:	test_layouts.cpp
DESCRIPTION:

	This file is used to test the primitive objects in the Adrenaline
	windowing package.

	For combinations of objects see:  test_combo_layouts.cpp
	
DATE	: November 20, 2014
AUTHOR	: Steve Tenniswood
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <string.h>
#include <string>
#include <iostream>     // std::cout
#include <new>     
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include "bk_system_defs.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "visual_memory.h"
#include "audio_memory.h"

#include "audio_app_menu.hpp"
#include "bcm_host.h"
#include "ilclient.h"
#include "AUDIO_device.h"
#include "audio_app.hpp"


extern int UpdateDisplaySemaphore;

AudioApp* audio_app = NULL;
////////////////////// AUDIO GRAPHICS VARIABLES : 
pthread_t 	audio_view_thread_id;	

float samples [1024];
float freq_mag[1024];
short freqs   [1024];


void init_audio_app()
{
	if (audio_app==NULL)
		audio_app = new AudioApp();
	MainDisplay.start_app( audio_app );
}

// This is the "OnCreate() function!
// Parse an xml
AudioApp::AudioApp()
: Application(),
dWave(),WaveformInfo(-1,-1), spl(-1,-1), freq_view(-1,-1),
wave_view_left(-1,-1), wave_view_right(-1,-1), VolumeSlider(-1,-1)
{
	Initialize();
}

AudioApp::AudioApp( Rectangle* mRect )
: Application( mRect ),
dWave(),WaveformInfo(-1,-1), spl(-1,-1), freq_view(-1,-1),
wave_view_left(-1,-1), wave_view_right(-1,-1), VolumeSlider(-1,-1)
{
	Initialize();
}

AudioApp::~AudioApp()
{
	printf("AudioApp::~AudioApp()\n");
	audio_app= NULL;
}

/* */
short* sample_waveform()
{
	static short data[200];
	for (int i=0; i<100; i++)
		data[i] = i*32700./100.;
	for (int i=101; i<120; i++)
		data[i] = 1700;
	for (int i=121; i<200; i++)
		data[i] = 32700. - ((i-121) * 32700/80.);
	return data;
}
void* process_audio(void*)
{
	static long int last_count = 0;
	if (ipc_memory_aud==NULL) return 0;

	while ( 1 )
	{
		if ( (ipc_memory_aud->UpdateCounter) > last_count )
		{
/*			last_count  = ipc_memory_aud->update_counter;
			ipc_memory_aud->acknowledge_counter++;
			long energy = get_audio_energy( (short*)ipc_memory_aud->audio_data, ipc_memory_aud->update_samples );

			float gain  = 5.0;
 			float level = gain * ( (float) energy / 327.67 );
			// printf("Audio energy: %d, %6.1f\n", energy, level ); 
			spl.set_level_left	(  level   );
			spl.set_level_right	(  level   );
			spl.Invalidate		(		   );

			// Do the FFT and Bin it:
			int length = 256;
			if (ipc_memory_aud->update_samples < length)
			{
				ShortToFloat	( (short*)ipc_memory_aud->audio_data, samples, length );
				fftMag			( samples, length,  freq_mag 	);
				int bins = Bin4	( freq_mag, length 				);
				FloatToShort	( freq_mag, freqs, ipc_memory_aud->update_samples );
			}
			UpdateDisplaySemaphore=1;  */
		} 
	} 
}

// create all the objects here.
void 	AudioApp::Initialize		(	)
{
	/*  Base class is initialized in the base class constructor.
		ie. The Application::Initialize is invoked there (not this one)
		Even though the function is virtual, for the base class,
		it calls the same level (base class) Initialize()
		Application::Initialize();	This will get called anyway!
		Therefore it is uneccessary and should not be put in.
	*/
	m_application_name = "AudioMaster";
	m_welcome_status   = "Audio Master";
	Application::Initialize();
	
	VolumeSlider.set_level_percent( 50.0    );	
	VolumeSlider.set_max		  ( 100.0   );
	VolumeSlider.set_width_height ( 70,200  );
	VolumeSlider.move_to		  ( 30, 100 );	
	VolumeSlider.set_text		  ( "Volume" );

	// also spl stereo power level: 
	spl.set_max					( 100.0	   );
	spl.set_min					(   0.0	   );
	spl.set_level_left			(  50.0    );
	spl.set_level_right			(  50.0    );
	spl.set_number_boxes		(  -1 	   );
	spl.set_width_height		( 100, 200 );
	spl.move_to					( 100, 100 );
	//printf("AudioApp::Initialize spl done\n");
 
	WaveformInfo.set_width_height( 300, 150 );
	WaveformInfo.move_to		 (  10, 730 );
	char* txt=NULL;
	if (ipc_memory_aud!=NULL)
	{ 
		txt = get_audio_text	( &(ipc_memory_aud->audio_header));	
		printf("AudioApp::Initialize txt=%s\n",txt);		
		WaveformInfo.set_text	( txt );		
	}
	WaveformInfo.set_text_size	 ( 12  );

	if (ipc_memory_aud != NULL)
		configure_wave_views( ipc_memory_aud->audio_header.num_channels,
						  (short*)ipc_memory_aud->audio_data,
						  (short*)ipc_memory_aud->audio_data   );
	
	freq_view.set_width_height		( 1000, 200 );
	freq_view.move_to				( 220, 520  );
	freq_view.set_data( sample_waveform(), 200 );
	//freq_view.set_data( freqs, 64 );
	printf("AudioApp::Initialize 3\n");

	// CREATE Audio Visual Thread : 
	int iret1 = pthread_create( &audio_view_thread_id, NULL, process_audio, NULL);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
	printf("thread created.\n");
}

void AudioApp::file_new			() 
{ 
	
}		

void AudioApp::file_open	()
{ 
	//static string Filename = "/home/pi/bk_code/avisual/resources/InTheBeginning.wav";
	static string Filename = "/home/pi/bk_code/avisual/resources/impending.wav";
	printf("Loading Wave from SD card:  %s\n", Filename.c_str());
	try {
		//dWave.m_data = new short[10512000];
		dWave.Load(Filename.c_str());	
	} catch (std::string msg) {
		printf("Error :  %s\n", msg.c_str() );
		WaveformInfo.set_text( msg.c_str() );
	}
	WAVEHDR* d1 = dWave.extract_channel_data(0);
	WAVEHDR* d2 = dWave.extract_channel_data(1);
	configure_wave_views( dWave.m_number_channels, (short*)d1->lpData , (short*)d2->lpData );
	WaveformInfo.set_text( dWave.get_format_string().c_str() );
	printf("Wave Loaded!\n");
}

void AudioApp::file_open_recent	() 
{ 
}

void AudioApp::file_save		() 
{ 
}

void AudioApp::file_save_as		() 
{ 
}	


void AudioApp::zoom_in			() { }		
void AudioApp::zoom_out			() { }		
void AudioApp::show_mixer		() { }		
void AudioApp::show_frequency	() { }
void AudioApp::show_fft  		() { }		

void AudioApp::audio_play( )
{ 
	printf("audio_play				() \n");
	int audio_dest = 1;				// 0=headphones, 1=hdmi
	int samplerate = 48000;			// audio sample rate in Hz
	int channels   = 2;				// numnber of audio channels   
	int bitdepth   = 16;			// number of bits per sample   
	bcm_host_init();
	printf("audio_play:host_init'd() \n");
	
	play_waveform( &dWave, audio_dest );
	//play_api_test( samplerate, bitdepth, channels, audio_dest );
}

void audio_stop				() { }
void audio_rewind			() { }
void audio_select_output_device () { }
void audio_record_from			() { }
void audio_broadcast_to			() { }

void audio_reverb			 () { }	
void audio_equalize			 () { }	
void audio_backwards		 () { }	
void audio_convert_to_mono	 () { }	
void audio_convert_to_stereo () { }	


/**********************************************
  How to get the audio in?  use audio_memory.h
  How to update it on periodic basis?
  		Create a thread - (where)
**********************************************/

/**********************************************
	result is scaled [0..32767]
**********************************************/
long int get_audio_energy(short* buffer, int length)
{
	long int sum = 0;
	for (int i=0; i<length; i++)
	   sum += abs( buffer[i] );
	sum /= length;
	return sum;	
}


void	AudioApp::setup_app_menu(	)
{
	Application::setup_app_menu();
}

void	AudioApp::setup_main_menu  	( )
{
	Application::setup_main_menu();

	try {
		printf("AudioApp::setup_main_menu() \n");
		init_audio_menu( &m_main_menu );

		//m_main_menu.m_entries.clear();
		//audio_menu->add_sub_menu( "Audio Master", &audio_master );
		//m_main_menu.add_sub_menu( "File",     &audio_file 		);
		m_main_menu.add_sub_menu( "View",     &audio_view 		);
		m_main_menu.add_sub_menu( "Play",     &audio_play_menu 	);
		m_main_menu.add_sub_menu( "Effects",  &audio_effects 	);
		printf("AudioApp::setup_main_menu() init audio menu done\n");		

	} catch (std::bad_alloc& ba)
	{
		std::cerr << "bad_alloc caught: " << ba.what() << '\n';
	}
	printf("AudioApp::Initialize menu done\n");
}

void	AudioApp::About			(	)
{
	Application::About();
}
void	AudioApp::Preferences		(	)
{
	Application::Preferences();
}


void AudioApp::register_with_display_manager()
{
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &VolumeSlider 	 );
	MainDisplay.add_object( &WaveformInfo 	 );
	MainDisplay.add_object( &spl 			 );
	MainDisplay.add_object( &wave_view_left  );
	MainDisplay.add_object( &wave_view_right );
	MainDisplay.add_object( &freq_view 	 	 );
	MainDisplay.set_menu  ( &m_main_menu  	 );
	//MainDisplay.m_status.set_text( m_welcome_status.c_str() );	
}

void AudioApp::configure_wave_views(int mChannels, short* mDataCh1, short* mDataCh2 )
{
	// Left Audio Channel Display:
	wave_view_left.m_wave = &dWave;	
	wave_view_left.set_width_height( 1400, 200 );
	wave_view_left.move_to		   (  220, 100 );
	wave_view_left.set_data		   ( mDataCh1  );

	// Left Audio Channel Display:
	if (mChannels>1)
	{
		wave_view_right.m_wave = &dWave;
		wave_view_right.set_width_height( 1400, 200 );
		wave_view_right.move_to		    (  220, 310 );	
		wave_view_right.set_data	( mDataCh2 );
	}
}




