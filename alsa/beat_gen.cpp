#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <fftw3.h>

#include "dspwave.hpp"
#include "alsa_record.hpp"
#include "beat_detect.hpp"
#include "beat_gen.hpp"


std::string cfn = "wavs/drum_clap_mono.wav";
Wave clap;

//std::string bassD_fn  = "wavs/bass_drum_1_mono.wav";
std::string bassD_fn  = "wavs/bass_drum_3.wav";
std::string snareD_fn = "wavs/snare_mono.wav";
Wave bassDrum;
Wave snareDrum;



void beat_gen_init()
{
	clap.Load( cfn );
	bassDrum.Load( bassD_fn );
	snareDrum.Load( snareD_fn );
}


size_t copy_clap( Wave& mclip, byte* mPlaybackData )
{
	size_t clip_len = mclip.get_samples_recorded() * 2;
	
	memcpy ( (char*)mPlaybackData, (char*)mclip.m_data, clip_len );
	return clip_len;
}

void generate_full( Wave& mPlaybackWave, float mBeatsPerMin )
{
	float beats_per_sec = mBeatsPerMin / 60.0;
	float period = 1.0 / beats_per_sec;
	
	int SampPeriod = round(mPlaybackWave.TimeToSamples(period));
	byte* ptr = (byte*)mPlaybackWave.m_data;
	size_t sample_counter = 0;
	size_t pb_length      = (mPlaybackWave.get_samples_allocated() - clap.get_samples_recorded() - 1);
	size_t alternating    = 0;
	
	while (sample_counter < pb_length)
	{
		size_t bytes_copied =0;
		if (alternating)
			bytes_copied = copy_clap( snareDrum, ptr );
		else 
			bytes_copied = copy_clap( bassDrum, ptr );


		ptr += SampPeriod*2;	
		sample_counter += (SampPeriod);
		alternating = !alternating;
	}
}

