/*
 * Simple sound playback using ALSA API and libasound.
 *
 * Compile:
 * $ cc -o play sound_playback.c -lasound
 * 
 * Usage:
 * $ ./play <sample_rate> <channels> <seconds> < <file>
 * 
 * Examples:
 * $ ./play 44100 2 5 < /dev/urandom
 * $ ./play 22050 1 8 < /path/to/file.wav
 *
 * Copyright (C) 2009 Alessandro Ghedini <alessandro@ghedini.me>
 * --------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Alessandro Ghedini wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we
 * meet some day, and you think this stuff is worth it, you can
 * buy me a beer in return.
 * --------------------------------------------------------------
 */

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <math.h>

#include "wave.hpp"
#include "dspwave.hpp"
#include "alsa_prep.hpp"
#include "read_pendant.h"
#include "visual/gl_setup.hpp"
#include "visual/gl_graph.hpp"



#define PCM_DEVICE "default"
bool LoopMode = false;

void pan_buffer(short* buff, int buff_size, int mPanPot )	// must be stereo!
{
	float L_alpha = (float)mPanPot / 1023.0;
	float R_alpha = (1 - L_alpha);
	//printf("PAN POT: %d ==> %6.3f %6.3f\n", mPanPot, L_alpha, R_alpha );
	
	size_t n_samples = buff_size / 2*2;  // Bytes / 4 
	for (int i=0; i<n_samples; i+=2)
	{
		buff[i+0] *= L_alpha;
		buff[i+1] *= R_alpha;
	}
}
BOOL VISUALIZE = TRUE;


BOOL scan_args( int argc, char** argv, const char* mSearchArg )
{
	int result = 0;
	for (int a=1; a<argc; a++)
	{
		result = strcmp(argv[a], mSearchArg);  	
		if (result==0)
			return TRUE;
	}
	return FALSE;	
}

int main(int argc, char **argv) 
{
	DSPWave imp;

	Wave clap;
	std::string cfn = "wavs/drums_clap_wide_bright.wav";
	clap.Load( cfn );

	std::string fn = "wavs/Clapping_2.wav";
	if (argc > 1) {

		BOOL found = FALSE;	

		found = scan_args( argc, argv[a], "loop" );
		if (found==TRUE)  { 
			LoopMode = true;  printf("Running in Loop Mode\n"); 
		} 
		found = scan_args( argc, argv[a], "nogui" );
		if (found==TRUE)  {
			 VISUALIZE = FALSE; printf("Running in Nogui mode.\n"); }						
		}
		fn = argv[argc];
	}

	/*printf("Opening TIMIT speech file\n");	
	std::string sfn = "TIMIT/SA1.WAV";
	TIMITWave sp_wave;
	sp_wave.Load( sfn );
	sp_wave.amplify(8.0);
	printf("Opening TIMIT speech file\n");	*/

	//std::string ofn = "wavs/Front_Left_stereo.wav";
	//std::string ofn = "wavs/landslide.wav";		
	//std::string ofn = "wavs/jennie.wav";	

	fn = "wavs/landofcotton.wav";	
	// 0.417 to 0.560 seconds ==> Samples 18389 to 24696	
	// Should give pitch of 118.6hz (122-116)
		
	std::string e_csv = "Energy.csv";			
	imp.Load (fn);
	//imp.convert_to_stereo(); 
	
	int Start = imp.TimeToSamples(0.417);
	int End   = imp.TimeToSamples(0.560);
	vector<t_real> autocorr = imp.full_auto_correlation( Start, End );
	
	std::string a_corr = "auto_corr.csv";			
	save_vector( autocorr, a_corr );	
	
	if (VISUALIZE) {
		size_t len = autocorr.size();
		size_t maxlen =  SIZE_OF_GRAPH;
		printf("AutoCorrel %ld Graph size=%ld\n", len, maxlen);

		double maximum = 0;	
		for (int i=0; i< maxlen; i++)
		{
			if (autocorr[i] > maximum)	maximum = autocorr[i];
		
			graph[i].y = autocorr[i];
			graph[i].x = i;
		}
		int  graph_result = gl_main(  );	
	}
	//while(1) {};
	
/*	long int Samps1 = imp.TimeToSamples( 3.26 );
	long int Samps2 = imp.TimeToSamples( 3.36 );
	printf("3.26 to 3.36 seconds = samples:  <%d,%d>\n", Samps1, Samps2 ); 
	
	vector<t_real> e_contour = imp.create_energy_contour(  );
	save_vector( e_contour, e_csv );
	
	vector<t_index> detects = imp.detect_beat_starts2( e_contour );
	vector<t_index> no_dups = imp.remove_close_duplicates( detects );
	vector<t_real> time_deltas = imp.compute_time_between_beats( no_dups );
	imp.print_detects_w_time( no_dups, time_deltas );
	
	imp.create_histogram( time_deltas, 20 );
*/
	
	//imp.convert_to_stereo();
	//imp.Save( ofn );
	
	//imp.tremolo( 1.0, 1.0, 0.5 );
/*	 // Construct FIR of echos
#define MAX_FIR_LENGTH 64000
	 float fir[MAX_FIR_LENGTH];
	 for (int f=0; f<MAX_FIR_LENGTH; f++)
	 	fir[f] = 0.0;
	 fir[0] = 1.0;
	 int delay_samps1  = imp.TimeToSamples( 0.1 );	printf("delay_samps1=%d\n", delay_samps1);
	 fir[delay_samps1] = 0.8;
	 delay_samps1      = imp.TimeToSamples( 0.5 );	printf("delay_samps2=%d\n", delay_samps1);
	 fir[delay_samps1] = 0.5;
	imp.apply_fir( fir, MAX_FIR_LENGTH );
*/
	// PRINT FORMAT INFO :
	string info = imp.get_format_string();
	printf( "%s\n\n", info.c_str() );


	// PREPARE HARDWARE:
	char* buff = NULL;		// buffer for ALSA
	snd_pcm_uframes_t frames = 0;
	snd_pcm_t       *pcm_handle = prepare_alsa( imp.m_number_channels, imp.m_samples_per_second,
												 &buff, &frames );
	size_t 			buff_size   = (frames-1) * imp.m_number_channels * 2;  // BYTES

	size_t d_len = imp.get_data_length_bytes();
	size_t bytes_played = 0;
	char*  ptr = (char*)imp.m_data;		// points to Wave m_data
	int counter = 0;
	bool done = true;
	if (LoopMode) done = false;
	do {
		while ( (bytes_played+buff_size) <  d_len )
		{			
			//printf("counter=%d\n", counter++);
			read_pendant();			
			memcpy( buff, (char*)ptr, buff_size );
			pan_buffer( (short*)buff, buff_size, Slider_values[0] );	// must be stereo!
			ptr          += buff_size;
			bytes_played += buff_size;
		
			unsigned int pcm =0;
			pcm = snd_pcm_writei(pcm_handle, buff, frames-1);	
			if (pcm == -EPIPE) {
				printf("XRUN.\n");
				snd_pcm_prepare(pcm_handle);	// clears the buffer for use.
			} else if (pcm < 0) {
				printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
			}
		}		
		// Restart:
		bytes_played = 0;
		ptr = (char*)imp.m_data;
	} while (!done);
	
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	free(buff);

	return 0;
}