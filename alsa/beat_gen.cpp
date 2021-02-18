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

#define WINDOW_SIZE 512

double compute_low_energy( fftw_complex* d_out )
{
	double energy = 0.0;
	for (int f=0; f<7; f++)
		energy += (d_out[f][0] * d_out[f][0]);	
	return energy/7.0;
}

double compute_high_energy( fftw_complex* d_out, int mLength )
{
	double energy = 0.0;
	for (int f=7; f<mLength; f++)
		energy += ( d_out[f][0] * d_out[f][0] );	
	return energy/(mLength-7);
}

/* Should be a window of 512 samples */
void bass_drum_detect( short* mData, int mLength )
{
	double* 	d_in  = fftw_alloc_real( WINDOW_SIZE );
	fftw_complex* d_out = fftw_alloc_complex ( WINDOW_SIZE );

	// FFT: 
	fftw_plan plan = fftw_plan_dft_r2c_1d( WINDOW_SIZE,   d_in, d_out,  FFTW_ESTIMATE );
	int out_size = WINDOW_SIZE / 2;
	
	// PUT DATA INTO REAL ARRAY:
	for (int i=0; i<mLength; i++)
	{
		d_in[i] = mData[i];
	}

	fftw_execute( plan );	

	// COMPUTE SPECTRAL ENERGY BELOW AND ABOVE:
	double low_pass  =  compute_low_energy ( d_out );		// LOW PASS FILTER 1100Hz
	double high_pass =  compute_high_energy( d_out, out_size );
	
	double low_ratio = (low_pass / high_pass);
		// For S fricative, high_pass will vastly be larger than low_pass.
		// For Do vowel, closer to 1 ratio.
	if (low_ratio < 0.5)
		printf("Drum=Snare. L/H= %6.3lf\n", low_ratio );
	else
		printf("Drum=Bass. L/H= %6.3lf \n", low_ratio );

	// HIGH BASS ==> BASS DRUM
}
