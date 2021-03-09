#include <stdint.h>
#include <string>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <pthread.h>

#include "gtk/graph.hpp"
#include "gtk/annotated_graph.hpp"


#include "midi/MidiFile.h"
#include "dspwave.hpp"
#include "resample.hpp"
#include "generate.hpp"
#include "sound_playback.hpp"
#include "sound_patch.hpp"
#include "midi_synth.hpp"
#include "dspwave.hpp"
#include "alsa_record.hpp"
#include "beat_detect.hpp"
#include "beat_gen.hpp"


#include "gtk/gtk_main.hpp"


bool LoopMode = false;
BOOL VISUALIZE = TRUE;

void help()
{
	printf(" Audio Synthesizer \n");
	printf(" ./asynth [mode] [filename]\n\n");
	
	printf(" loop 			 - Continuous looping of an audiofile. \n");
	printf(" gui  [filename] - read a audio file and display it. \n");			
	printf(" midi [filename] - read a midifile and synthesize it. \n");			
	
	printf(" beat - play a drum pattern Overlay melodies. \n");
	printf(" anal [filename] - Play and process an audio file for Musical DNA.\n");
	printf(" rp   [filename] - Record & Playback mode.\n");	

	printf(" test  - Generates a square wave with envelope. \n");	
	exit(1);
}

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

#define MIDIFILE  1
#define AUDIOFILE 2
#define TEST 	  3
#define BEATBOX   4
#define RECORD_PLAY 5

std::string fn = "midi/mario-dies.mid";
int InputFileType = 0;

void process_args( int argc, char** argv )
{
	if (argc > 1) {
		bool found = FALSE;	
		found = scan_args( argc, argv, "help" );
		if (found==true)  { 
			help();
		} 

		found = scan_args( argc, argv, "loop" );
		if (found==true)  { 
			LoopMode = true;  printf("Running in Loop Mode\n"); 
		} 
		found = scan_args( argc, argv, "gui" );
		if (found==true)  {
			InputFileType=AUDIOFILE; 
			fn = argv[2];			
			VISUALIZE = true; printf("Running in GUI mode.\n"); 						
		}
		
		found = scan_args( argc, argv, "midi" );
		if (found==true)  { 
			InputFileType=MIDIFILE;   printf("Running in Midi File\n"); 
			fn = argv[2];
		} 
		found = scan_args( argc, argv, "beat" );
		if (found==true)  { 
			InputFileType=BEATBOX;   printf("Running in BeatBox Mode\n"); 
		} 

		found = scan_args( argc, argv, "anal" );
		if (found==true)  { 
			InputFileType=AUDIOFILE;   printf("Running in Audio Analysis \n"); 
			fn = argv[2];
		} 

		found = scan_args( argc, argv, "rp" );
		if (found==true)  { 
			InputFileType=RECORD_PLAY;   printf("Running in Record/Playback Mode\n"); 
			fn = argv[2];
		} 

		found = scan_args( argc, argv, "test" );
		if (found==true)  { 
			InputFileType=TEST;   printf("Running in Test Mode. Square wave with envelope.\n"); 
		} 
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

#define NUM_BEATS_DESIRED 4*4

int main (int argc, char** argv)
{
	process_args(argc, argv );

	if (InputFileType==TEST) {
		
		GendSoundSource square( eGenType::GS_SQUARE );	
		struct stEnvelope test;
		test.attack_time  = 0.01;
		test.decay_time   = 0.50;
		test.sustain_time = 3.00;	
		test.release_time = 1.50;
		test.decay_level  = 0.25;
		square.set_envelope(44100, test);
		square.stamp_wave( 0x40, 1.0, wave.m_data, 44100*60*2 );
	} 
	else if (InputFileType==MIDIFILE) {
		synthesize_midifile();
		wave.Save("WaveSynth.wav");
	}
	else if (InputFileType==AUDIOFILE)
	{
		// OPEN AUDIO FILE AND PROCESS IT:  (for beat detection)
		recorded.Load   ( fn       );
		std::string info = recorded.get_format_string();
		printf("%s\n", info.c_str() );
		if (recorded.m_number_channels==2)
			recorded.convert_to_mono();

		process_waveform( recorded );
		if (VISUALIZE)
		{						
		printf("VISUALIZE annotatedGraph \n");		
			// Energies into Graph : 
			int ok  = graph_init( );
		printf("graph_init %d \n", ok );					
			AnnotatedGraph* e_graph = create_annotated_graph( "Energy -vs- Time", "Window", "Energy", 
					Energies, EnergiesIndex  );

		printf("created annotatedGraph \n");
		
			AddEnergyGraphData( e_graph );

			int graph_result = graph_main( );
		}
	}
	else if (InputFileType==BEATBOX)
	{
		beat_gen_init();

		float BeatsPerMin = 120.0;
		float time = (1.0/(BeatsPerMin / 60.0)) * NUM_BEATS_DESIRED;
		size_t SampLen = wave.TimeToSamples( time );

		wave.Resize( SampLen+5 );
		generate_full( wave, BeatsPerMin );

		// RECORD & PLAYBACK : 
		pthread_t thread_id; 
		pthread_create(&thread_id, NULL, record_thread_func, NULL); 
		int result = sound_playback( wave, true );		
	}
	else if (InputFileType==RECORD_PLAY)  {
		wave.Load   ( fn       );
		size_t play_samples = wave.get_samples_recorded();		
		float time = wave.SamplesToTime(play_samples);
		printf("Playing %6.3f seconds of audio; \n", time);
		
		// RECORD & PLAYBACK : 
		pthread_t thread_id; 
		pthread_create(&thread_id, NULL, record_thread_func, NULL); 
		int result = sound_playback( wave, false );		
	}
	return 0;
}
