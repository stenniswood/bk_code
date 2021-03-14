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
#include <algorithm>
#include <ostream> 


#include "wave.hpp"
#include "dspwave.hpp"
#include "dsp_funcs.h"


//using namespace std;

#define NO_BURST_FRACTION 0.05			// 0.1%  ==> can be lower (0.01 %)!
#define ENERGY_THRESHOLD 75000000
#define MOVING_THRESHOLD_TOL 0.9


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

void save_vector_long( std::vector<long int>& mLongSave, std::string mFilename )
{
	FILE* file = fopen(mFilename.c_str(), "wr");
	if (file==NULL)	{ perror("Error cannot open file for writing!\n"); }
	char str[25];
	
	for (int i=0; i < mLongSave.size(); i++)
	{
		sprintf(str, "%ld ", mLongSave[i] );
		fwrite(str, strlen(str), 1, file);
	}
	fclose( file );
}

void save_vector( std::vector<t_real>& mRealSave, std::string mFilename )
{
	FILE* file = fopen(mFilename.c_str(), "wr");
	if (file==NULL)	{ perror("Error cannot open file for writing!\n"); }
	char str[25];
	
	for (int i=0; i<mRealSave.size(); i++)
	{
		sprintf(str, "%6.3f ", mRealSave[i] );
		fwrite(str, strlen(str), 1, file);
	}
	fclose( file );
}

// fft:  preemphasized vector of time samples (variable vet_re)
// and returns the real (vet_re) and the immaginary (vet_im) parts of its fft
void Fft_Call( std::vector<double>& vet_re, std::vector<double>& vet_im)
{	
	xft(vet_re.data(), vet_im.data(), 8, -1);	
}


DSPWave::DSPWave ()
:window_start_index(0)
{ 
	WindowAdvancement = trunc(SAMPLE_RATE / FrameRate);		// ~160
	//printf("SAMPLE_RATE=%d; FrameRate=%6.3f; WindowAdvancement=%ld\n", 
	//	SAMPLE_RATE, FrameRate, WindowAdvancement );

	if (HammingWindowCreated==false)
	{
		create_hamming_window();
		HammingWindowCreated = true;
	}
}

DSPWave::DSPWave( BYTE mChannels, int mSamplesPerSecond, int mBufferSizeSamples, short* mData )
:Wave( mChannels, mSamplesPerSecond, mBufferSizeSamples, mData )
{

}

DSPWave::~DSPWave()
{
 
}

/*void	tremolo( float LeftConst, float RightConst, int mSamplePeriod )
{
	for (int i=0; i<m_buffer_length; i+= m_block_align)
	{
		float L_alpha = LeftConst * sin ( (float)i / (float)mSamplePeriod );
		float R_alpha = RightConst * cos ( (float)i / (float)mSamplePeriod );
	}
}*/

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

std::vector<short>& DSPWave::GetRawSamples()
{
	static std::vector<short> retval;
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

void DSPWave::output_features( std::ostream& out, long int& fv_index, char delim )
{
	for (int i=0; i<NUM_FEATURE_VECTORS; i++)
		out << feature_vector[i] << delim;
}

void DSPWave::output_window( std::ostream& out, long int& fv_index )
{
	out << fv_index << " : ";
	for (int i=0; i<WINDOW_SIZE/8; i++)
		out << window[i] << " ";
	out << std::endl;
	out << std::endl;
}

void DSPWave::output_dft( std::ostream& out, long int& fv_index )
{
	out << fv_index << " : ";
	for (int i=0; i<WINDOW_SIZE/8; i++)
		out << freq_real_space[i] << " ";
	out << std::endl;
	out << std::endl;
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

/* Computes energy for a short time block.  Window */
t_real DSPWave::compute_energy( int mStartIndex, int mEndIndex )
{
	float energy = 0.0;
	long int sample_sqrd_right = 0;
	long int sample_sqrd_left  = 0;
	
	for (int i=mStartIndex; i<mEndIndex; i+=m_number_channels) 
	{
		sample_sqrd_left = m_data[i+0] * m_data[i+0];
		energy += ((t_real)sample_sqrd_left)/m_number_channels;
		if (m_number_channels>1) 
		{
			sample_sqrd_right = m_data[i+1] * m_data[i+1];
			m_energy += ((t_real)sample_sqrd_right)/m_number_channels; 
		}
	}
	return energy;
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

//vector<t_real> energies;
#define MOVING_AVG_PERIOD 10


std::vector<t_real> DSPWave::compute_simple_moving_avg( std::vector<t_real>& mData )
{
	std::vector<t_real> energy_simple_moving;
	printf("Waveform Moving Avg Energies scan:\n");
	std::vector<t_real>::iterator bin_start = mData.begin();
	std::vector<t_real>::iterator bin_end = (mData.begin()+MOVING_AVG_PERIOD);
	while (bin_end != mData.end() )
	{
		long int average = 0.0;
		for (std::vector<t_real>::iterator iter=bin_start; iter<=bin_end; iter++)
			average += *iter;
		average /= MOVING_AVG_PERIOD;
		energy_simple_moving.push_back(average);							
		bin_start++;
		bin_end++;
	}	
	return energy_simple_moving;
}

void DSPWave::print_energies( std::vector<t_real>& mEnergies )
{
	printf("Time domain energies:\n");
	//printf("Waveform Window energies\tMoving Avg:\n");
	t_index size = mEnergies.size();
	for (int p=0; p<size; p++) 
			printf("%6.0f, ", mEnergies[p] );

	printf("\nend. Total energies=%ld \n", mEnergies.size() );
}

void DSPWave::print_detects_w_time( std::vector<t_index>& start_points, std::vector<t_real>& times  )
{
	printf("Detected Beat Starts\tTime Delta:\n");
	t_index size = start_points.size();
	
	for (int i=0; i<size; i++) 
	{					
		printf("%ld\t%6.3f\n", start_points[i], times[i] );
	}
	printf("end. %ld\n", size );

}

void DSPWave::print_detects( std::vector<t_index>& start_points )
{
	printf("Detected Beat Starts\n");
	t_index size = start_points.size();
	float time_delta = 0.0;
	
	for (int i=0; i<size; i++) 
	{
		printf("%ld\t\n", start_points[i] );
	}
	printf("end. %ld\n", size );
}


t_real get_max( std::vector<t_real>& data, t_index& max_index )
{
	t_real mmax = -32767.0;
	max_index = -1;
	
	for (int i=0; i<data.size(); i++)
	{
		if (data[i]>mmax) {  mmax = data[i];  max_index = i;  }		
	}
	return mmax;
}
t_real get_min( std::vector<t_real>& data, t_index& min_index )
{
	t_real mmin = +32767.0;
	min_index   = -1;
	
	for (int i=0; i<data.size(); i++)
	{
		if (data[i]<mmin) {  mmin = data[i];  min_index = i;  }		
	}
	return mmin;
}

void DSPWave::create_histogram( std::vector<t_real>& data, int mNumBins  )
{
	t_index max_index;
	t_index min_index;
	t_real hmin = get_min( data, min_index );
	t_real hmax = get_max( data, max_index );
	t_real bin_width = ((hmax-hmin) / (t_real)mNumBins);
	printf("%d bins : bin_width=%5.3f\n", mNumBins, bin_width );
	
	std::vector<t_index>  bin_occupancy;
	for (int b=0; b<mNumBins; b++)
		bin_occupancy.push_back(0);

	//
	for (int i=0; i<data.size(); i++)
	{
		t_index bin_number = round( (data[i]-hmin) / bin_width );
		bin_occupancy[bin_number]++;		
	}
	
	for (int b=0; b<mNumBins; b++)
		printf( "bin #%d : %d\n", b, bin_occupancy[b] );
		
	// Get Highest Occupancy:
	t_index max_occ = 0;
	t_index max_bin = 0;
	for (int b=0; b<mNumBins; b++)
		if (bin_occupancy[b] > max_occ)	
		{  max_occ = bin_occupancy[b];  max_bin = b; }

	t_real period_seconds = (max_bin * bin_width) + hmin;
	printf("Most common Period (occupancy:%d)= %5.3f seconds per beat\n", max_occ, period_seconds );
	
}

/* To find the best starting edge,
	places where the energy. jumps above 75million.
	and the moving average is still below 0.9 * 75 million (when it goes above, we are 
				in the middle of the beat.)	
*/
std::vector<t_index> DSPWave::detect_beat_starts2( std::vector<t_real>& energies )
{
	std::vector<t_index> start_points;
	start_points.clear();
	
	printf("detect_beat_starts():\n");
	t_index esize 	  = energies.size();
	float multiplier = 10.0;
	
	for (int i=0; i<esize; i++)
	{
		float before = energies[i+0];
		float on     = energies[i+3];		// More sudden onset.
		float after  = energies[i+8];		// Not perfectly symmetrical -- often a longer trailing energy to the curve.
		
		if ((on > (multiplier*before)) && (on > (multiplier*after)))
			{
				start_points.push_back( i );
			}
	}
	printf("end. %ld\n", start_points.size());
	return start_points;
}

std::vector<t_index> DSPWave::remove_close_duplicates( std::vector<t_index>& start_points )
{	
	printf("remove_close_duplicates():\n");
	t_index esize 	  = start_points.size();
	t_index prev      = start_points[0];
		
	for (int i=1; i<start_points.size(); i++)
	{
		t_index delta = (start_points[i] - prev);
		if (delta<2)
		{
			start_points.erase( start_points.begin()+i );
		}
		prev = start_points[i];
	}
	printf("end. %ld\n", start_points.size());
	return start_points;
} 


/* To find the best starting edge,
	places where the energy. jumps above 75million.
	and the moving average is still below 0.9 * 75 million (when it goes above, we are 
				in the middle of the beat.)	
*/
std::vector<t_index> DSPWave::detect_beat_starts(std::vector<t_real>& energies)
{
	std::vector<t_index> start_points;
	bool last_window_was_detect = false;
	printf("detect_beat_starts():\n");
	t_index e_index   = MOVING_AVG_PERIOD;
	t_index mae_index = 0;
	t_index esize 	  = energies.size();
	std::vector<t_real> energy_simple_moving = compute_simple_moving_avg( energies );
	
	start_points.clear();
	while (e_index <= esize )
	{
		bool cond1a = (energies[e_index-2] < (NO_BURST_FRACTION * ENERGY_THRESHOLD));
		bool cond1b = (energies[e_index]   > ENERGY_THRESHOLD);
		bool cond2  = (energy_simple_moving[mae_index] < (MOVING_THRESHOLD_TOL*ENERGY_THRESHOLD));
		
		if (cond1a && cond1b && cond2 )
		{
			printf("%zu energies[%d-1]=%6.0f;\tenergies[e_index]=%6.0f;\tenergy_simple_moving[mae_index]=%6.0f\t",
				e_index, e_index*WindowAdvancement, energies[e_index-1], energies[e_index], energy_simple_moving[mae_index] );
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
			printf(" Acceptable detect! %d %ld\n", sample_index, start_points.size());
			last_window_was_detect = true;
		}
		else
			last_window_was_detect = false;

		e_index++;
		mae_index++;
	}
	printf("end. %ld\n", start_points.size());
	return start_points;
}


std::vector<t_real> DSPWave::compute_time_between_beats( std::vector<t_index>& start_points )
{
	std::vector<t_real> time_deltas;
	t_index sample_delta;
	t_real time_delta;
	printf("compute_time_between_beats()  channels=%d \n", m_number_channels);
	time_deltas.clear();
	t_index size = start_points.size();
	for (int i=0; i<size-1; i++)
	{
		sample_delta = (start_points[i+1] - start_points[i]) * (WindowAdvancement * m_number_channels);		
		time_delta   = SamplesToTime( sample_delta );
		printf("samp delta = %d; dt=%6.3f\n", sample_delta, time_delta );
		time_deltas.push_back( time_delta );
	}	
	printf("samps:%d  -- time_deltas size =%ld\n", sample_delta, time_deltas.size());
	return time_deltas;
}

// Scan entire waveform and no more.
std::vector<t_real> DSPWave::create_energy_contour(  )
{
	std::vector<t_real> energies;
	printf("Scanning energy_only()...\n");
	PositionWindow( 0 );		// start of waveform.
	while  (window_is_past_end()==false )
	{
		t_real tmp = compute_energy( window_start_index, window_start_index+(WINDOW_SIZE*m_number_channels) );
		energies.push_back(tmp);		
		AdvanceWindow();			// 160 samples * 2 channels = 320 shorts
	}
	printf("last start_index computed = %ld;  %ld\n", window_start_index, window_start_index+(WINDOW_SIZE*m_number_channels) );

	return energies;
}

void DSPWave::read_feature_vector( std::ifstream& in, long int& fv_index, std::string& mSymbol )
{
	/* This reads 1 line from the feature std::vector file: */
	std::string symbol,tmp,file_index;
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

//===========================================================================
//=  Function to compute mean for a series m_data                                =
//===========================================================================
double DSPWave::compute_mean(int mStartIndex, int mEndIndex)
{
  double   mean;        // Computed mean value to be returned
  int      i;           // Loop counter
  long int N = (mEndIndex - mStartIndex);
  
  // Loop to compute mean
  mean = 0.0;
  for (i=mStartIndex; i<mEndIndex; i++)
    mean = mean + (m_data[i] / N);

  return(mean);
}

/* 
Waveform length must be (End-Start + Shift) number of samples.  ie. Shift samples past the end.
*/
double DSPWave::compute_auto_correlation ( int mStartIndex, int mEndIndex, int mSampleShift )	// Mono only.
{
	//printf("compute_auto_correlation()\n");
	
	long int mean = round(compute_mean( mStartIndex, mEndIndex ));
	//printf("\nMean = %ld\n", mean);
	long int N = (mEndIndex - mStartIndex);
	
	long int Samp1 = 0;
	long int Samp2 = 0;
	double Summation = 0;
	for (int i=mStartIndex; i<mEndIndex; i++)
	{
		Samp1 = m_data[i]-mean;
		Samp2 = m_data[i+mSampleShift]-mean;
		Summation += (Samp1*Samp2);	
//		if (i > (mEndIndex-5))	printf("s1=%ld s2=%ld;  sum=%ld\n", Samp1, Samp2, Summation);	
	}
	double scale = ((N - mSampleShift));
	//printf("start=%d; stop=%d;  Summation=%6.3f; d=%ld; (N-shift)=%6.3f\n", mStartIndex, mEndIndex, Summation, mSampleShift, scale );
	
	return Summation / scale;
}

/* Given a region including several periods (3-4)
	0.05 to 0.1 seconds total     
	 */
std::vector<double> 	DSPWave::full_auto_correlation ( int mStartIndex, int mEndIndex )	// Mono only.
{
	printf("Full auto correlation\n");

	std::vector<t_real>  auto_corr;
	long int NumSamples = (mEndIndex - mStartIndex);
	double scale = 1.0;
		
	for (int d=0; d < NumSamples; d++)
	{
		double result = compute_auto_correlation ( mStartIndex, mEndIndex, d );	// Mono only.	
		if (d==0) {
			printf("Autocorr at 0 delay = %9.3f\n", result);
			scale = result;
		}
		
		auto_corr.push_back( 1.0 * result / scale );
	}
	return auto_corr;
}


#endif /* DSPWAVE_CPP_ */

