#include <stdlib.h>
#include <stdio.h>

#include "peak_pick.hpp"



std::vector<double> RawData;
std::vector<double> SmoothData;

static std::vector<int>	peak_indices;
static std::vector<int>	peak_deltas;

#define BAND_WIDTH 5	
#define BAND_WIDTH_2X (2*BAND_WIDTH)
#define BAND_WIDTH_3X 15


void set_peakdata_I		( std::vector<int> mData   )
{
	size_t len = mData.size();
	
	for (int i=0; i<len; i++)
	{
		RawData.push_back( mData[i] );
	}	
}

void set_peakdata_D( double mValues[], size_t mLength )
{
	for (int i=0; i<mLength; i++)
	{
		RawData.push_back( mValues[i] );
	}	
}

void smooth_data	( int mSmoothSize )
{
	size_t len = RawData.size()-mSmoothSize;
	
	// Smooth Values:	
	double avg = 0.0;
	for (int i=0; i<len; i++)
	{
		avg = 0.0;
		for (int n=0; n<mSmoothSize; n++)
			avg += RawData[i+n];
		avg /= mSmoothSize;
		SmoothData.push_back( avg );
	}
}

int local_search( double mValues[], int mNeighborhoodSize, int mStartIndex )
{
	double maxv = mValues[mStartIndex];
	double maxi = mStartIndex;
	int half_size = (mNeighborhoodSize>>1);

	int j = mStartIndex-half_size;
	for ( ; j < mStartIndex+half_size; j++)
	{
		double tmp = mValues[j];
		if (tmp > maxv)		{  maxv=tmp;  maxi = j; }
	}
	return maxi;	
}

/* Pick peaks based on surrounding energy levels. */
void peak_pick_energy( double mValues[], int mLength )
{
	double after, before, on;
	double after2,before2;
	double avg_ab;
	double avg_ab2;
	
	peak_indices.clear();

	for (int i=BAND_WIDTH; i<(mLength-BAND_WIDTH); i++)
	{
		on      = mValues[i];
		after   = mValues[i+BAND_WIDTH];			// Narrow 
		before  = mValues[i-BAND_WIDTH];
		
		if (i>=BAND_WIDTH_2X) 
			before2 = mValues[i-BAND_WIDTH_2X];
		if (i<(mLength-BAND_WIDTH_2X-1)) 
			after2  = mValues[i+BAND_WIDTH_2X];		// Wide band			

		double after_ratio  = ( on / after );  //learned_scale_1 * (above+below)/2.0;		
		double before_ratio = ( on / before ); //learned_scale_1 * (above+below)/2.0;

		double after2_ratio  = ( on / after2  );  //learned_scale_1 * (above+below)/2.0;		
		double before2_ratio = ( on / before2 ); //learned_scale_1 * (above+below)/2.0;

		if ((before_ratio > 1.6) || (after_ratio>2.0))
		//avg_ab  = learned_scale_1 * (above+below)/2.0;		
		//if ((on > avg_ab) || (on > avg_ab2))
		{
			int maxi = local_search( mValues, BAND_WIDTH_2X, i );
			peak_indices.push_back( maxi );
			i = i+4;
		}
	}
	printf("Selected %d Energy Peaks!\n", peak_indices.size() );
	
}

/* TOP LEVEL FUNCTION */
int find_peaks( double mValues[], size_t mLength )
{
	set_peakdata_D  ( mValues, mLength );
	smooth_data     ( 5  );
	peak_pick_energy( SmoothData.data(), SmoothData.size() );
	
	// PEAK CANDIDATES ARE NOW IN "peak_indices"	
	return peak_indices.size();
}

void init_peaks		( int mNumBins )
{
	peak_indices.clear();
}


void print_peaks( )
{
	printf("Peak Energies Array[%d] = \n", peak_indices.size() );
	char str[256];
	for (int i=0; i < peak_indices.size(); i++)
	{
		sprintf(str, "PeakValues[%d] = %6.3lf\n", peak_indices[i], SmoothData[peak_indices[i]] );
		printf("%s,  ", str );
	}
	printf("\n");
}

