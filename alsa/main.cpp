#include <stdint.h>
#include <string>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <pthread.h>


#include "midi/MidiFile.h"
#include "dspwave.hpp"
#include "resample.hpp"
#include "generate.hpp"
#include "sound_playback.hpp"
#include "sound_patch.hpp"
#include "midi_synth.hpp"
#include "alsa_record.hpp"


bool LoopMode = false;
BOOL VISUALIZE = TRUE;


bool scan_args( int argc, char** argv, const char* mSearchArg )
{
	int result = 0;
	for (int a=1; a<argc; a++)
	{
		result = strcmp(argv[a], mSearchArg);  	
		if (result==0)
			return true;
	}
	return false;	
};

DSPWave 		wave( 1, 44100, 44100 * 60 * 3.0, NULL );
int 			beats_per_min   = 100;
float 			beats_per_sec = (float)beats_per_min/60.;
float 			secs_per_beat = 60.0 / (float)beats_per_min;
smf::MidiFile 	midifile;
	

void print_midi()
{
   int tracks = midifile.getTrackCount();
   std::cout << "TPQ: " << midifile.getTicksPerQuarterNote() << std::endl;
   
   if (tracks > 1) std::cout << "TRACKS: " << tracks << std::endl;

   for (int track=0; track<tracks; track++) 
   {
      if (tracks > 1) 
      	std::cout << "\nTrack " << track << std::endl;
      std::cout << "Tick\tSeconds\tDur\tMessage" << std::endl;
      
      for (int event=0; event<midifile[track].size(); event++) {
         std::cout << std::dec << midifile[track][event].tick;
         std::cout << '\t' << std::dec << midifile[track][event].seconds;
         std::cout << '\t';
         if (midifile[track][event].isNoteOn())
            std::cout << midifile[track][event].getDurationInSeconds();
         std::cout << '\t' << std::hex;
         for (int i=0; i< midifile[track][event].size(); i++)
            std::cout << (int)midifile[track][event][i] << ' ';
         std::cout << std::endl;
      }
	}
}

void print_freq()
{
	float c = 130.800;
	for (int s=0; s<=12; s++)
	{
		printf("%6.3f \n", higher_freq_scale_ratios[s]*c);
	}
}

std::string fn = "midi/mario-dies.mid";

void process_args( int argc, char** argv )
{
	if (argc > 1) {
		bool found = FALSE;	

		found = scan_args( argc, argv, "loop" );
		if (found==true)  { 
			LoopMode = true;  printf("Running in Loop Mode\n"); 
		} 
		found = scan_args( argc, argv, "nogui" );
		if (found==true)  {
			 VISUALIZE = false; printf("Running in Nogui mode.\n"); 						
		}
		if (!found)
			fn = argv[1];
	}
}

void synthesize_midifile()
{
	MidiSynth ms;
	printf("Reading MIDI file: %s\n", fn.c_str() );
	
	ms.read_file( fn.c_str() );
	midifile.doTimeAnalysis();
    midifile.linkNotePairs ();
	print_midi();
		
	ms.select_sound( "Square",  1 );
	ms.select_sound( "Brass",   2 );
	ms.select_sound( "Piano",   3 );	
	ms.select_sound( "Trumpet", 4 );		
	ms.synth_midi( wave ); 
}


int main (int argc, char** argv)
{
	process_args(argc, argv );
	
	GendSoundSource square( eGenType::GS_SQUARE );	
	struct stEnvelope test;
	test.attack_time  = 0.01;
	test.decay_time   = 0.50;
	test.sustain_time = 3.00;	
	test.release_time = 1.50;
	test.decay_level  = 0.25;
	square.set_envelope(44100, test);
	square.stamp_wave( 0x40, 1.0, wave.m_data, 44100*60*2 );

//	synthesize_midifile();
	
	wave.Save("WaveSynth.wav");

	init_hw_record();
	pthread_t thread_id; 
	pthread_create(&thread_id, NULL, record, NULL); 

	//int result = sound_playback( wave, false );		
	clean_up();
	
	return 0;
}
