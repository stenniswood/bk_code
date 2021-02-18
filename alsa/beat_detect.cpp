#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "dspwave.hpp"
#include "alsa_record.hpp"
#include "beat_detect.hpp"


double Energies[NUM_FRAMES*2];
size_t EnergiesIndex = 0;

double learned_scale_1 = 1.25;	// Weights for accepting a peak energy. narrow.
double learned_scale_2 = 1.25;	// wider

std::vector<int>	peak_indices;
std::vector<int>	peak_deltas;

int BestDelta = 40;
#define MIN_FRAME_DIST 20	/* no faster than 200 beats per min */
#define MAX_FRAME_DIST 172  /* no slower than 30 beats per min */

struct stBinInfo {
	double   s_delta;
	long int count;
};

struct stBinInfo* bins;// = [peak_deltas.size()];
int max_bin   = 0;
int max_index = 0;


/* Compute Energy on a Window (typically 256 to 512 samples) */
double compute_energy( short* mSamps, long int mLength )
{
	double Sum = 0;
	for (long int i=0; i<mLength; i++)
	{
		Sum += (mSamps[i]*mSamps[i]);
	}
	double len = (double)mLength;
	//printf("length = %6.3lf  \n", len );
	double energy = (double)Sum / len;	
		//printf("energy = %6.3lf  \n", energy );
	return energy;
}

/* Pick peaks based on surrounding energy levels. */
void peak_pick_energy()
{
	double above,below,on;
	double above2,below2;
	double avg_ab;
	double avg_ab2;
	
	peak_indices.clear();
	
	for (int i=4; i<(EnergiesIndex-4); i++)
	{
		on     = Energies[i];
		above  = Energies[i+4];			// Narrow 
		below  = Energies[i-4];
		if ((i>=8) && (i<(EnergiesIndex-9))) {
			above2 = Energies[i+8];		// Wide band
			below2 = Energies[i-8];
			avg_ab2 = learned_scale_2 * (above2+below2)/2.0;
		}

		avg_ab  = learned_scale_1 * (above+below)/2.0;		
		if ((on > avg_ab) || (on > avg_ab2))
		{
			peak_indices.push_back( i );
			i = i+4;
		}
	}
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
}

/* Find Min/Max frame deltas between detected peaks */
void min_max_deltas( int& mMin, int& mMax )
{
	for (int i=0; i < peak_deltas.size(); i++)
	{
		if (peak_deltas[i] < mMin )
			mMin = peak_deltas[i];
		if (peak_deltas[i] > mMax )
			mMax = peak_deltas[i];
	}
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
}

/* Given a frame Best Delta, compute the rate of beats per minute. */
float determine_beats_per_min()
{
	const int SampleRate = 44100;
	const int WindowSize = 512;

	double FrameRate = (float)SampleRate / (float)WindowSize;
	float bpm = ( FrameRate / BestDelta ) * 60.0;

	printf("Beats Per Minute (detected) = %6.3f\n", bpm );
	return (bpm);	
}

void  process_waveform(  )
{
	long int len = recorded.get_samples_recorded();
	printf("length=%ld;  %d  %d \n", len, recorded.m_block_align, recorded.m_bytes_recorded );
	double  e = compute_energy( recorded.m_data, len );
printf("Energy computed done.");
	// print_array		();
	peak_pick_energy	();
	compute_peak_deltas ();
	bin_peak_deltas		();
	determine_beats_per_min();
	
	print_peaks     	();
	print_deltas		();
}

///////////////////// PRINTING FUNCTIONS ///////////////////////////
void print_array( )
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

void print_peaks( )
{
	printf("Peak Energies Array[%d] = \n", peak_indices.size() );
	char str[256];
	for (int i=0; i < peak_indices.size(); i++)
	{
		sprintf(str, "Energies[%d] = %6.3lf\n", peak_indices[i], Energies[peak_indices[i]] );
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






