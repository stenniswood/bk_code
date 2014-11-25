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
#include "bk_system_defs.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "visual_memory.h"
#include "audio_memory.h"
#include "audio_app.hpp"



extern int UpdateDisplaySemaphore;

static Window		ParentWindowF(450, 1050, 500, 100);
static RadioButton 	MyRadio1 ( -1, -1 );
static RadioButton 	MyRadio2 ( -1, -1 );
static RadioButton 	MyRadio3 ( -1, -1 );
static RadioButton 	MyRadio4 ( -1, -1 );
static CheckBox 	audio_source1(-1, -1);
static CheckBox 	audio_source2(-1, -1);
static Button   	Okay	   	 (-1,-1);
static Button   	Cancel	   	 (-1,-1);
static TextView 	SampleText;
static IconView		test_icon 	 ( 50,200 );
static TextView 	WaveformInfo ( -1, -1);

void print_combo_test_list()
{
	printf("30 : Audio app \n"	 );
	printf("31 : CAN app\n"	 );
	printf("32 : \n"	 );
}

void load_combo_test_screen( int number )
{
	printf("init_display() DisplayNum=%d\n", number);
	switch(number) 
	{	
	case 29: init_okay_cancel_dlg();
			break;
	case 30: init_audio_view();
			break;
	case 31: //init_CAN_app();
			break;	
	default : break;
	}
}

void init_okay_cancel_dlg()
{	
	MyRadio1.set_text("AM Radio");
	MyRadio2.set_text("FM Radio");
	MyRadio3.set_text("XM Radio");
	MyRadio4.set_text("Internet Radio");

/*	MyRadio2.set_position_below ( &MyRadio1 );
	MyRadio3.set_position_below ( &MyRadio2 );
	MyRadio4.set_position_below ( &MyRadio3 ); */

	MyRadio1.join_group( &MyRadio2 );
	MyRadio1.join_group( &MyRadio3 );
	MyRadio1.join_group( &MyRadio4 );
	MyRadio1.expand_group_widths();		
	MyRadio3.select();
	MyRadio2.select();

	audio_source1.set_text("V6 Engine");
	audio_source2.set_text("All wheel drive");
	audio_source1.set_check();
	//check1.set_position_above( &check2 );
	//check2.set_position_below( &check1 );
	//check1.copy_position_horiz( &check2 );

	Okay.set_text("Okay");
	Cancel.set_text("Cancel");
	
	SampleText.set_width_height(-1,-1);
	SampleText.set_text("My Car dialog\n");
	//SampleText.center_vertical(TRUE);
	ParentWindowF.pack_control( &SampleText, PACK_FILL_PARENT, PACKER_ALIGN_TOP );
	ParentWindowF.pack_control( &MyRadio1, PACK_RIGHT, PACKER_ALIGN_TOP );
	ParentWindowF.pack_below  ( &MyRadio2, &MyRadio1, PACK_COPY_HORIZ );
	ParentWindowF.pack_below  ( &MyRadio3, &MyRadio2, PACK_COPY_HORIZ );
	ParentWindowF.pack_below  ( &MyRadio4, &MyRadio3, PACK_COPY_HORIZ );

	ParentWindowF.pack_control( &audio_source1, PACK_LEFT, PACKER_ALIGN_TOP  	);
	ParentWindowF.pack_below  ( &audio_source2, &audio_source1, PACK_COPY_HORIZ );

	ParentWindowF.pack_control( &test_icon , PACK_LEFT	 , 	PACKER_ALIGN_BOTTOM );
	ParentWindowF.pack_control( &Okay	   , PACK_RIGHT  ,  PACKER_ALIGN_BOTTOM );
	ParentWindowF.pack_control( &Cancel	   , PACK_RIGHT  ,  PACKER_ALIGN_BOTTOM );

	// Add to display manager:
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object		  ( &ParentWindowF );	
}


// AUDIO GRAPHICS VARIABLES : 
static StereoPowerLevels spl   			 (-1,-1);
static FrequencyView	 freq_view  	 (-1,-1);
static WaveView 		 wave_view_left  (-1,-1);
static WaveView 		 wave_view_right (-1,-1);
static Leveler			 VolumeSlider	 (100.,0.);
pthread_t 	audio_view_thread_id;	

float samples [1024];
float freq_mag[1024];
short freqs   [1024];

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

void* process_audio(void*)
{
	static long int last_count = 0;
	if (ipc_memory_aud==NULL) return 0;

	while ( 1 )
	{
		if ( (ipc_memory_aud->update_counter) > last_count )
		{
			last_count  = ipc_memory_aud->update_counter;
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
			UpdateDisplaySemaphore=1;
		}
	}
}

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

// This is the "OnCreate() function!
// Parse an xml
void init_audio_view()
{
	VolumeSlider.set_width_height ( 50,200  );
	VolumeSlider.move_to		  ( 10, 100 );	
	VolumeSlider.set_level_percent( 50.0    );	
	VolumeSlider.set_max		  ( 100.0   );

	// also spl stereo power level: 
	spl.set_width_height		( 100, 200 );
	spl.move_to					( 100, 100 );
	spl.set_max					( 100.0	   );
	spl.set_min					(   0.0	   );
	spl.set_level_left			(  50.0    );
	spl.set_level_right			(  50.0    );
	spl.set_number_boxes		(  -1 	   );

	WaveformInfo.set_width_height( 300, 150 );
	WaveformInfo.move_to		 (  10, 310 );
	char* txt = get_audio_text( &(ipc_memory_aud->audio_header));
	printf(txt);
	WaveformInfo.set_text		 ( txt );
	WaveformInfo.set_text_size	 ( 12  );
	
	wave_view_left.set_width_height( 1000, 200 );
	wave_view_left.move_to		   ( 220, 100 );
	if (ipc_memory_aud != NULL)
		wave_view_left.set_data		( (short*)ipc_memory_aud->audio_data );

	if (ipc_memory_aud->audio_header.num_channels > 1)
	{
		wave_view_right.set_width_height( 1000, 200 );
		wave_view_right.move_to		    ( 220,  310 );
		if (ipc_memory_aud != NULL)
			wave_view_right.set_data	( (short*)ipc_memory_aud->audio_data );
	}

	freq_view.set_width_height		( 1000, 200 );
	freq_view.move_to				( 220, 520  );
	freq_view.set_data( sample_waveform(), 200 );
	//freq_view.set_data( freqs, 64 );

	// CREATE Audio Visual Thread : 
	int iret1 = pthread_create( &audio_view_thread_id, NULL, process_audio, NULL);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
	printf("thread created.\n");

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &VolumeSlider );	
	MainDisplay.add_object( &WaveformInfo );	
	MainDisplay.add_object( &spl );
	MainDisplay.add_object( &wave_view_left  );
	MainDisplay.add_object( &wave_view_right );
	MainDisplay.add_object( &freq_view );	
}


