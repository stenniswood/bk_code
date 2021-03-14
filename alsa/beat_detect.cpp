#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fftw3.h>
#include "gtk/dataseries.hpp"
#include "dspwave.hpp"
#include "alsa_record.hpp"
#include "beat_detect.hpp"
#include "histo_bin.hpp"
#include "gtk/graph.hpp"
#include "gtk/annotated_graph.hpp"

#define WINDOW_SIZE 512


double Energies[NUM_FRAMES*2];
size_t EnergiesIndex = 0;
double SmoothEnergies[NUM_FRAMES*2];
size_t SmoothLength = 0;


double learned_scale_1 = 1.25;	// Weights for accepting a peak energy. narrow.
double learned_scale_2 = 1.25;	// wider

std::vector<int>	peak_indices;
std::vector<int>	peak_deltas;

int BestDelta = 40;
#define MIN_FRAME_DIST 20	/* no faster than 200 beats per min */
#define MAX_FRAME_DIST 172  /* no slower than 30 beats per min */



/* Compute Energy on a Window (typically 256 to 512 samples) */
double compute_energy( short* mSamps, long int mLength )
{
	double Sum = 0;
	for (long int i=0; i<mLength; i++)
	{
		Sum += (mSamps[i]*mSamps[i]);
	}
	double len    = (double)mLength;
	double energy = (double)Sum / len;
	
	// printf("length = %6.3lf  \n", len );
	// printf("energy = %6.3lf  \n", energy );
	return energy;
}


/*  For all detected peaks, get the number of frames between.   */
void compute_peak_deltas()
{
	int delta = 0;
	peak_deltas.clear();
	
	for (int i=0; i < peak_indices.size()-1; i++)
	{
		delta = (peak_indices[i+1] - peak_indices[i]);
		if ((delta>MIN_FRAME_DIST) && (delta<MAX_FRAME_DIST))
			peak_deltas.push_back( delta );
	}
	printf("compute_peak_deltas() : %ld peak_deltas\n", peak_deltas.size() );
}


void learn_peaks( std::vector<int>& m2Lambdas_HistIndex )
{
	// go from histo index to
	// frame index -- energy
	// 
}

/* Given an expected place for a peak.  Find out why it was not detected and 
   adjust the learning weights accordingly.    								*/
void learn_peak( int mExpectedCenterFrame )
{
	// FIND HIGHEST PEAK IN REGION : 
	double Max_e = 0;
	int max_index = 0;
	for (int i=mExpectedCenterFrame-5; i < mExpectedCenterFrame+5; i++)
	{
		double on = Energies[i];
		if (on > Max_e)
		{
			Max_e = on;
			max_index = i;
		}		
	}
	
	// SCAN BOTH SIDES:
	double above  = Energies[max_index+4];
	double below  = Energies[max_index-4];
	double above2 = Energies[max_index+8];
	double below2 = Energies[max_index-8];

	printf("Found peak at: [%d]= %6.3lf\n", max_index, Max_e );
	
	printf("Below %6.3f On %6.3f Above %6.3f\n", below, Max_e, above );
	printf("Below2 %6.3f On %6.3f Above2 %6.3f\n", below2, Max_e, above2 );	
	double avg = (above + below)/2.0;
	double avg2 = (above2 + below2)/2.0;	
	printf("Intensity Ratio = %6.3lf; /avg2=%6.3lf \n", Max_e / avg, Max_e/avg2 );
}

void bin_peak_deltas()
{
	set_data_I  ( peak_deltas );	
	init_bins   ( 15 );
//	printf("bin_peak_deltas() : Bins initted...\n");
	bin_data    (    );
	std::vector<int>	m2Lambdas;	
	get_2lambdas( m2Lambdas );
    fix_2lambdas( m2Lambdas );	
	
	print_bins();
	int  mode_index = get_mode( );
	
	printf("bin_peak_deltas() : mode = %d\n", mode_index );	
	if (mode_index>=0)
		BestDelta       = peak_deltas[mode_index];	
	
	printf("MaxOccupancyBin[%d]=%d;  value=%d\n", mode_index, get_count(mode_index), BestDelta );		
}


float beats_per_min_to_seconds(float mBPM )
{
	float time = 1.0 / (mBPM / 60.0);		// seconds per beat	
	return time;
}

/* Given a frame index : BestDelta, compute the rate of beats per minute. */
float determine_beats_per_min()
{
	const int SampleRate = 44100;
	const int WindowSize = 512;

	double FrameRate = (float)SampleRate / (float)WindowSize;
	float bpm = ( FrameRate / BestDelta ) * 60.0;

	printf("Beats Per Minute (detected) = %6.3f\n", bpm );
	return (bpm);	
}

/************************************************************/


///////////////////// PRINTING FUNCTIONS ///////////////////////////
void print_energy_array( )
{
	printf("Energies Array[%d] = \n", EnergiesIndex );
	char str[256];
	for (int i=0; i < EnergiesIndex; i++)
	{
		sprintf(str, "%6.3f", Energies[i] );
		printf("%s,  ", str );
	}
	printf("\n");
}


void print_deltas( )
{
	printf("Peak Delta Indices Array(): %d \n", peak_deltas.size() );
	char str[256];
	for (int i=0; i < peak_deltas.size(); i++)
	{
		sprintf(str, "%d\t", peak_deltas[i] );
		printf("%s,  ", str );
		if ((i%8)==7)
			printf("\n");
	}
	printf("\n");
}



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
int bass_drum_detect( short* mData, int mLength, double& mLHRatio )
{
	double* 	  d_in  = fftw_alloc_real    ( WINDOW_SIZE );
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
	
	mLHRatio = (low_pass / high_pass);
	//printf(" L/H= %6.3lf  ", low_ratio );
	
	int retval = -1;
		// For S fricative, high_pass will vastly be larger than low_pass.
		// For Do vowel, closer to 1 ratio.
	if (mLHRatio < 0.5)
	{
		retval = 2;
	} else {		// HIGH BASS ==> BASS DRUM
		retval = 1;
	}
	return retval;
}





/*void bin_peak_deltas()
{
	int MMin=0; int MMax=0;
	bins = new struct stBinInfo[ peak_deltas.size() ];	
	min_max_deltas( MMin, MMax );
	
	float bin_width = (float)(MMax - MMin) / 10.0;
	printf("MMin=%d;  MMax=%d;  bin_width=%6.3f\n", MMin, MMax, bin_width );

	// INIT TO ZERO : 
	for (int i=0; i < peak_deltas.size(); i++)	{
		bins[i].count = 0;
		bins[i].s_delta = ((float)i)*bin_width+(float)MMin;
	}

	int bin_offset  = 0;

	// FILE UNDER APPROPRIATE BIN :
	for (int i=0; i < peak_deltas.size(); i++)
	{
		bin_offset = round((peak_deltas[i] - MMin) / bin_width);
		bins[bin_offset].count++;		
	}


	// LOOK FOR "2 Lambdas" ie (a missed beat)
	int half_bin_offset = 0;
	for (int i=0; i < peak_deltas.size(); i++)
	{
		bin_offset      = round((float)(peak_deltas[i] - MMin) / bin_width);
		half_bin_offset = round((float)(peak_deltas[i]/2.0 - MMin) / bin_width);
		
		if (bins[half_bin_offset].count > bins[bin_offset].count)
		{
			bins[half_bin_offset].count += 2;
			bins[bin_offset].count--;			
			printf("Found '2 lambda' beat. %6.3lf %6.3lf\n", bins[half_bin_offset].s_delta, bins[bin_offset].s_delta  );
			
			int pindex = ((peak_indices[i+1] - peak_indices[i]) / 2.0) + peak_indices[i];
			
			// We want the index into the energy array :
			learn_peak( pindex );
		}
	}	
	
	// PRINT BINS:
	for (int i=0; i < peak_deltas.size(); i++)
	{
		printf("Bin[%d] %6.3lf : %ld\n", i, bins[i].s_delta,  bins[i].count );
	}
	
	// FIND MODE:
	for (int i=0; i < peak_deltas.size(); i++)
		if ( bins[i].count > max_bin )
		{	max_bin = bins[i].count;	max_index = i;	};
	
	BestDelta = peak_deltas[max_index];
	printf("MaxBin[%d] = %d; count=%d\n", max_index, BestDelta, max_bin );	
} */
