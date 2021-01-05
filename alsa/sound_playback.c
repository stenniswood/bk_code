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
#include "alsa_prep.hpp"

#define PCM_DEVICE "default"

int main(int argc, char **argv) 
{
	Wave imp;

	std::string fn = "impending.wav";
	if (argc > 1) {
		fn = argv[1];
	}

	imp.Load (fn);
	//imp.tremolo( 4.0, 4.0, 44100 );
	 // Construct FIR of echos
/*#define MAX_FIR_LENGTH 64000
	 float fir[MAX_FIR_LENGTH];
	 for (int f=0; f<MAX_FIR_LENGTH; f++)
	 	fir[f] = 0.0;
	 fir[0] = 1.0;
	 int delay_samps1  = original.TimeToSamples( 0.1 );		printf("delay_samps1=%d\n", delay_samps1);
	 fir[delay_samps1] = 0.8;
	 delay_samps1      = original.TimeToSamples( 0.5 );	printf("delay_samps2=%d\n", delay_samps1);
	 fir[delay_samps1] = 0.5;
*/	

	// PRINT FORMAT INFO :
	string info = imp.get_format_string();
	printf( "%s\n\n", info.c_str() );


	// PREPARE HARDWARE:
	char* buff = NULL;		// buffer for ALSA
	snd_pcm_uframes_t frames = 0;
	snd_pcm_t       *pcm_handle = prepare_alsa( imp.m_number_channels, imp.m_samples_per_second,
												 &buff, &frames );
	size_t 			buff_size   = (frames-1) * imp.m_number_channels * 2;

	size_t d_len = imp.get_data_length_bytes();
	size_t bytes_played = 0;
	char*  ptr = (char*)imp.m_data;		// points to Wave m_data
	int counter = 0;
	while ( (bytes_played+buff_size) <  d_len )
	{
		//printf("counter=%d\n", counter++);
		memcpy( buff, (char*)ptr, buff_size );
		ptr          += buff_size;
		bytes_played += buff_size;
		
		unsigned int pcm =0;
		pcm = snd_pcm_writei(pcm_handle, buff, frames-1);	
		printf("pcm=%d\n", pcm);	
		if (pcm == -EPIPE) {
			printf("XRUN.\n");
			snd_pcm_prepare(pcm_handle);	// clears the buffer for use.
		} else if (pcm < 0) {
			printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
		}
	}

	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	free(buff);

	return 0;
}