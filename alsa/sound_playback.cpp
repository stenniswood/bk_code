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
#include <string>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <math.h>

#include "wave.hpp"
#include "dspwave.hpp"
#include "alsa_prep.hpp"
#include "read_pendant.h"
#include "visual/gl_setup.hpp"
#include "visual/gl_graph.hpp"
#include "resample.hpp"


#define PCM_DEVICE "default"


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

int sound_playback( DSPWave& wav, bool mLoopMode )
{
	// PREPARE HARDWARE:
	char* buff = NULL;		// buffer for ALSA
	snd_pcm_uframes_t frames = 0;
	snd_pcm_t       *pcm_handle = prepare_alsa( wav.m_number_channels, wav.m_samples_per_second, &buff, &frames );												 
	size_t 			buff_size   = (frames-1) * wav.m_number_channels * 2;  // BYTES

	size_t d_len = wav.get_data_length_bytes();
	size_t bytes_played = 0;
	char*  ptr = (char*)wav.m_data;		// points to Wave m_data
	int    counter = 0;
	bool   done = true;
	if (mLoopMode) done = false;
	printf("Entering playback loop...\n");
	
	do {
		while ( (bytes_played+buff_size) <  d_len )
		{			
			//read_pendant();			// for pan 
			memcpy( buff, (char*)ptr, buff_size );
			//pan_buffer( (short*)buff, buff_size, Slider_values[0] );	// must be stereo!
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
			printf("snd_pcm_writei() \n");
		}		
		// Restart:
		bytes_played = 0;
		ptr = (char*)wav.m_data;
	} while (!done);
	printf("Finished playback...\n");
	
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	free(buff);
	return 1;
}

void autocorrelation(DSPWave& wav)
{
	int Start = wav.TimeToSamples(0.417);
	int End   = wav.TimeToSamples(0.560);
	std::vector<t_real> autocorr = wav.full_auto_correlation( Start, End );

//	afn = "wavs/landofcotton.wav";	
	// 0.417 to 0.560 seconds ==> Samples 18389 to 24696	
	// Should give pitch of 118.6hz (122-116)		
	
	std::string a_corr( "auto_corr.csv" );
//	save_vector( autocorr, a_corr );	
	std::string e_csv = "Energy.csv";			
}

	
int sound_play_main(int argc, char **argv) 
{
	std::string afn = "wavs/Clapping_2.wav";
	std::string cfn = "wavs/drums_clap_wide_bright.wav";
	
	Wave clap;
	clap.Load( cfn );


	/*if (VISUALIZE) { 
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
		//int  graph_result = gl_main(  );	
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
//	std::string info = wav.get_format_string();
//	printf( "%s\n\n", info.c_str() );


	return 0;
}

