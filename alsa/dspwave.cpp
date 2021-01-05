/*
 * dspwave.cpp
 *
 *  Created on: Dec 20, 2016
 *      Author: stephen.tenniswood
 */

#ifndef DSPWAVE_CPP_
#define DSPWAVE_CPP_
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>

#include "wave.hpp"
#include "dspwave.hpp"
#include "dsp_funcs.h"


#define NO_BURST_FRACTION 0.05			// 0.1%  ==> can be lower (0.01 %)!
#define ENERGY_THRESHOLD 75000000
#define MOVING_THRESHOLD_TOL 0.9
vector<t_index> start_points;

bool  HammingWindowCreated = false;
float HammingWindow[WINDOW_SIZE];
void create_hamming_window()
{
	const float alpha = 0.54;
	const float beta  = 0.46;
	for (int i=0; i<WINDOW_SIZE; i++)
	{
		HammingWindow[i] = alpha - beta * cos( (2*M_PI*i)/(WINDOW_SIZE-1) );
	}	
}


// fft:  preemphasized vector of time samples (variable vet_re)
// and returns the real (vet_re) and the immaginary (vet_im) parts of its fft
void Fft_Call(vector<double>& vet_re, vector<double>& vet_im)
{	
	xft(vet_re.data(), vet_im.data(), 8, -1);	
}


DSPWave::DSPWave ()
:window_start_index(0)
{ 
	WindowAdvancement = trunc(SAMPLE_RATE / FrameRate);		// ~160
	printf("SAMPLE_RATE=%d; FrameRate=%6.3f; WindowAdvancement=%ld\n", 
		SAMPLE_RATE, FrameRate, WindowAdvancement );

	if (HammingWindowCreated==false)
	{
		create_hamming_window();
		HammingWindowCreated = true;
	}
}

DSPWave::~DSPWave()
{
 
}

void	tremolo( float LeftConst, float RightConst, int mSamplePeriod )
{
	for (int i=0; i<m_buffer_length; i+= m_block_align)
	{
		float L_alpha = LeftConst * sin ( (float)i / (float)mSamplePeriod );
		float R_alpha = RightConst * cos ( (float)i / (float)mSamplePeriod );
	}
}

void DSPWave::Preemphasize			( )
{ 
	const float alpha     = 0.95;
	const float one_alpha = 1.0 - alpha;
	short* out = new short[get_samples_allocated()];
	out[0] = m_data[0];
	for (int i=1; i<WINDOW_SIZE; i++)
	{
		out[i] = alpha * m_data[i] + one_alpha*m_data[i-1];
	}
	/* Note this could be done in parallel:  
	 * 		alpha * Signal
	 * 		one_alpha * Signal
	 * 		then shift one and add the two results. Think GPU. 
	 */
}

// Place the window and copy it into window[]
void DSPWave::PositionWindow		( long int mWindowIndex )
{
	window_start_index = mWindowIndex;
}

// Move the window and copy it into window[]
void DSPWave::AdvanceWindow 		( )
{ 
	window_start_index += WindowAdvancement*m_number_channels;
	//160 samples *2 = 320 shorts
}

bool DSPWave::window_is_past_end	( )	// if more samples needed for full window..
{
	long int end_index = window_start_index + WINDOW_SIZE*m_number_channels;
	long int last_index = get_samples_recorded()*m_number_channels;  // 340992 * 2 = 681984 
	if (end_index >= last_index )
		return true;
	return false;
}

void DSPWave::LoadWindow()		// put current samples into window[] array
{
	int length = WINDOW_SIZE;
	long int total_samples = m_bytes_recorded / (2 * m_number_channels);
	long int remaining = (total_samples - window_start_index);
	if (length > remaining )
		length = remaining;
	memcpy( window, &(m_data[window_start_index]), length );
}

vector<short>& DSPWave::GetRawSamples()
{
	static vector<short> retval;
	retval.resize(WINDOW_SIZE);
	int length = WINDOW_SIZE;
	long int total_samples = m_bytes_recorded / (2 * m_number_channels);
	long int remaining = (total_samples - window_start_index);
	if (length > remaining )
		length = remaining;
	memcpy( retval.data(), &(m_data[window_start_index]), length );
	return retval;
}
	

void DSPWave::ApplyHammingWindow( )
{
	//printf("ApplyHammingWindow \n");
	for (int s=0; s<WINDOW_SIZE; s++)
		window[s] = window[s] * HammingWindow[s];
}

#define FORWARD_FFT 1
/* 
INPUT  :
	window[]
OUTPUT :
	freq_real_space[]
	freq_imag_space[]
*/
void DSPWave::DFT( )
{
	//printf("DFT \n");
 	// Transfer samples into freq_real_space as doubles.
 	for (int s=0; s<WINDOW_SIZE; s++) {
 		freq_real_space[s] = window[s];
		freq_imag_space[s] = 0.0;
 	} 	
/*	Magnitude_And_Energy_Of_Real_Vectors( 	
					freq_real_space, freq_imag_space,
					WINDOW_SIZE, m_energy ); */
}


// At the current Window;  ==> feature_vector[]	
tFeatureVector&  DSPWave::get_features( )
{
	static tFeatureVector feats;	
	feats.d.resize( NUM_FEATURE_VECTORS );
	for (int i=0; i<NUM_FEATURE_VECTORS; i++)
		feats.d[i] = feature_vector[i];
	feats.symbol = " ";
	return feats;
}

void DSPWave::output_features( ostream& out, long int& fv_index, char delim )
{
	for (int i=0; i<NUM_FEATURE_VECTORS; i++)
		out << feature_vector[i] << delim;
}

void DSPWave::output_window( ostream& out, long int& fv_index )
{
	out << fv_index << " : ";
	for (int i=0; i<WINDOW_SIZE/8; i++)
		out << window[i] << " ";
	out << endl;
	out << endl;
}

void DSPWave::output_dft( ostream& out, long int& fv_index )
{
	out << fv_index << " : ";
	for (int i=0; i<WINDOW_SIZE/8; i++)
		out << freq_real_space[i] << " ";
	out << endl;
	out << endl;
}

t_index DSPWave::GetWindowPosition	( )
{
	return window_start_index;
}


/* 	Resample : 
	Use this to take sampled sound and resample to another pitch of the musical scale.
	Frequency ratio for the chromatic scale is in freq_sale_ratios[].

Return :	mOutput length The buffer should be same size as input buffer. but not all of it will be used.
			b/c this is upscaling to higher frequency.
*/
short linear_interpolate( short mSample1, short mSample2, float mFraction )
{
	double delta = mSample2 - mSample1;
	double fract_delta = (mFraction * delta)+mSample1;
	return trunc(fract_delta);
}

void DSPWave::add( DSPWave& mNewSound, float mNewVolumeFraction, float mFractionRetain )	
{
	size_t new_sound_samples = mNewSound.m_buffer_length / mNewSound.m_number_channels ;
	
	if (mNewSound.m_buffer_length > m_buffer_length)
		Resize( new_sound_samples );
	
	size_t sample_length = m_buffer_length / m_number_channels;
	for (int i=0; i<sample_length; i++)
		m_data[i] = mFractionRetain*m_data[i] + mNewVolumeFraction* mNewSound.m_data[i];		
}

t_real DSPWave::compute_energy( int mStartIndex, int mEndIndex )
{
	m_energy = 0.0;
	for (int i=mStartIndex; i<mEndIndex; i+=m_number_channels) 
	{
		long int sample_sqrd_left = m_data[i] * m_data[i];
		long int sample_sqrd_right;
		m_energy += ((t_real)sample_sqrd_left)/m_number_channels;
		if (m_number_channels>1) {
			sample_sqrd_right = m_data[i] * m_data[i];		
			m_energy += ((t_real)sample_sqrd_right)/m_number_channels; 
		}
	}
	return m_energy;
}

DSPWave& DSPWave::operator=( DSPWave& mOriginal )
{
	Wave::operator=( mOriginal );
	return *this;
}
/*	Truth data for bass.wav	(approximate sample numbers)
1		4419
2		8960
3		19109
4		39470
5		49684
6		69987
7		80139
8		90350
9		100560
10		120806
11		131196
12		151380
13		161532
14		171803
15		181895
16		202437
17		212579
18		232773
19		243044
20		253136
21		263348
22		283771
23		293922
24		314288
25		324437		
Total Samples: 340,992	Audacity report 
Total Bursts : 25
*/

// Does it inplace.  Resizes m_data as needed.
void DSPWave::resample ( float mMultiplier )
{
	float fraction;
	int   oIndex = 0;
	float rIndex = 0;
	int   sIndex =0;
	int   eIndex =1;
	int    SampLength = (m_buffer_length/m_number_channels);		// orginal
	int total_o_samples = round( SampLength / mMultiplier );
	short* output     = new short[total_o_samples]();	
	
	// 
	for (int oIndex=0; oIndex < total_o_samples; oIndex++)
	{		
		sIndex = floor( rIndex );
		eIndex = ceil ( rIndex );
		fraction = (rIndex - sIndex);
		output[oIndex] = linear_interpolate( m_data[sIndex], m_data[eIndex], fraction );
		rIndex += mMultiplier;		// free parameter
	}
	//printf("made it here\n");
	Resize( total_o_samples );
	memcpy(m_data, output, 	total_o_samples*m_number_channels);
}

vector<t_real> energies;
#define MOVING_AVG_PERIOD 10
vector<t_real> energy_simple_moving;
vector<t_real> time_deltas;

void DSPWave::compute_simple_moving_avg()
{
	printf("Waveform Moving Avg Energies scan:\n");
	vector<t_real>::iterator bin_start = energies.begin();
	vector<t_real>::iterator bin_end = (energies.begin()+MOVING_AVG_PERIOD);
	while (bin_end != energies.end() )
	{
		long int average = 0.0;
		for (vector<t_real>::iterator iter=bin_start; iter<=bin_end; iter++)
			average += *iter;
		average /= MOVING_AVG_PERIOD;
		energy_simple_moving.push_back(average);							
		bin_start++;
		bin_end++;
	}	
}

void DSPWave::print_energies( )
{
	printf("Waveform Window energies\tMoving Avg:\n");
	t_index size = energies.size();
	for (int p=0; p<MOVING_AVG_PERIOD; p++) 
			printf("%6.0f\t \n", energies[p] );

	for (int i=0; i<size; i++) {
		printf("%6.0f\t%6.0f\n", energies[i], energy_simple_moving[i] );
	}
	printf("end. energies=%ld moving Avg size=%ld\n", energies.size(), energy_simple_moving.size() );
}


void DSPWave::print_detects( vector<t_index>& mValues )
{
	printf("Detected Beat Starts\tTime Delta:\n");
	t_index size = start_points.size();
	
	for (int i=0; i<size; i++) 
		printf("%ld\t%6.3f\n", start_points[i], time_deltas[i] );
	printf("end. %ld\n", size );
}

/* To find the best starting edge,
	places where the energy. jumps above 75million.
	and the moving average is still below 0.9 * 75 million (when it goes above, we are 
				in the middle of the beat.)	
*/
void DSPWave::detect_beat_starts(  )
{
	bool last_window_was_detect = false;
	printf("detect_beat_starts():\n");
	t_index e_index   = MOVING_AVG_PERIOD;
	t_index mae_index = 0;
	t_index esize 	  = energies.size();
	start_points.clear();
	while (e_index <= esize )
	{
		bool cond1a = (energies[e_index-2] < (NO_BURST_FRACTION * ENERGY_THRESHOLD));
		bool cond1b = (energies[e_index]   > ENERGY_THRESHOLD);
		bool cond2  = (energy_simple_moving[mae_index] < (MOVING_THRESHOLD_TOL*ENERGY_THRESHOLD));
		
		if (cond1a && cond1b && cond2 )
		{
			printf("%zu energies[e_index-1]=%6.0f;\tenergies[e_index]=%6.0f;\tenergy_simple_moving[mae_index]=%6.0f\t",
				e_index*WindowAdvancement, energies[e_index-1], energies[e_index], energy_simple_moving[mae_index] );
			printf("cond1a=%d, cond1b=%d; cond2=%d;\n", cond1a, cond1b, cond2 );

			// convert window index into sample index:
			t_index window_index  = e_index;
			t_index sample_index  = (window_index * WindowAdvancement);
			// If 2 windows in a row give a detect, we only want one and we'll give it
			// the sample index halfway between the windows.  More accurate positioning.
			if (last_window_was_detect)				// update the sample index
				start_points.end() += ((WINDOW_SIZE/2) * m_number_channels);
			else 
				start_points.push_back( sample_index );		// sample index			
			printf(" Acceptable detect!  %ld\n", start_points.size());
			last_window_was_detect = true;
		}
		else
			last_window_was_detect = false;

		e_index++;
		mae_index++;
	}
	printf("end. %ld\n", start_points.size());
}


void DSPWave::compute_time_between_beats()
{
	t_real sample_delta;
	t_real time_delta;
	printf("compute_time_between_beats()  m_samples_per_second=%d\n", m_samples_per_second);
	time_deltas.clear();
	t_index size = start_points.size();
	for (int i=0; i<size-1; i++)
	{
		sample_delta = (t_real)(start_points[i+1] - start_points[i]);
		time_delta   = sample_delta / m_samples_per_second;
		time_deltas.push_back( time_delta );
	}	
	printf("time_deltas size =%ld\n", time_deltas.size());
}

// Scan entire waveform and no more.
void DSPWave::scan_energy_only(  )
{
	printf("Scanning energy_only()...\n");
	PositionWindow( 0 );		// start of waveform.
	while  (window_is_past_end()==false )
	{
		t_real tmp = compute_energy( window_start_index, window_start_index+(WINDOW_SIZE*m_number_channels) );
		energies.push_back(tmp);		
		AdvanceWindow();			// 160 samples * 2 channels = 320 shorts
	}
	printf("last start_index computed = %ld;  %ld\n", window_start_index, window_start_index+(WINDOW_SIZE*m_number_channels) );
	compute_simple_moving_avg();

	printf("Time domain energies:\n");
	print_energies( );

	detect_beat_starts();
	compute_time_between_beats();
	print_detects( start_points );
}

void DSPWave::energy_sort_avg()
{
	printf("\nSorted energies:\n");
	std::sort (energies.begin(), energies.end() );
	print_energies( );

}

void DSPWave::read_feature_vector( ifstream& in, long int& fv_index, string& mSymbol )
{
	/* This reads 1 line from the feature vector file: */
	string symbol,tmp,file_index;
	in >> file_index;
	in >> fv_index;
	in >> window_start_index;
	in >> mSymbol;
	in >> tmp;
	for (int i=0; i<NUM_FEATURE_VECTORS; i++)
	{
		in >> feature_vector[i];
	}	
}


#endif /* DSPWAVE_CPP_ */

