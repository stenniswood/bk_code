#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "dspwave.hpp"
#include "alsa_record.hpp"
#include "beat_detect.hpp"
#include "histo_bin.hpp"


std::vector<struct stBinInfo> bins;
double increment = 0.0;						// between bins

std::vector<double> HistoData;				// RAW DATA
size_t HistoIndex = 0;


/* Find Min/Max data  */
void min_max_data( std::vector<double>& mHistData,  double& mMin, double& mMax )
{
	mMin = -32767;
	mMax = +32767;
	for (int i=0; i < mHistData.size(); i++)
	{
		if (mHistData[i] < mMin )			mMin = mHistData[i];
		if (mHistData[i] > mMax )			mMax = mHistData[i];
	}
}


void set_data_I( std::vector<int> mData )
{
	HistoData.clear();	
	for (int i=0; i<mData.size(); i++)
		HistoData.push_back( (double)mData[i] );		
}
void set_data_S( std::vector<short> mData )
{
	HistoData.clear();	
	for (int i=0; i<mData.size(); i++)
		HistoData.push_back( (double)mData[i] );		
}


// PRINT BINS:
void print_bins()
{
	for (int i=0; i < bins.size(); i++)
	{
		printf("Bin[%d] %6.3lf : %ld\n", i, bins[i].s_value,  bins[i].count );
	}
}

void init_bins( int mNumBins ) {

	double BMax, BMin;
	min_max_data( HistoData, BMin, BMax );
	
	double range = (BMax - BMin);
	increment    = (range / (double)mNumBins);

	struct stBinInfo tmpBin;	
	tmpBin.s_value = BMin;
	tmpBin.e_value = BMin+increment;
	tmpBin.count   = 0;
	
	for (int b=0; b<mNumBins; b++)
	{
		bins.push_back( tmpBin );
		tmpBin.s_value += increment;
		tmpBin.e_value = (tmpBin.s_value + increment);
	}
}

int get_mode()
{
	double max_value = 0.0;
	int    max_index =  -1;
	
	for (int b=0; b < bins.size(); b++)
	{
		if (bins[b].count > max_value)
		{	max_index = b;  max_value = bins[b].count;	}		
	}
	return max_index;
}

int find_proper_bin( double mDataPoint )
{
	double start   = bins[0].s_value;
	int    b_index = round((mDataPoint - start) / increment);
	return b_index;	
}

inline void bump_count(int mIndex ) {  bins[mIndex].count++; };

long int get_count( int mBinIndex ) 
{
  return bins[mBinIndex].count; 
};

void bin_data(  )
{
	int d_index = 0;
	for (int d=0; d<HistoData.size(); d++) {
		d_index = find_proper_bin( HistoData[d] );
		bump_count( d_index );
	}
}
void get_2lambdas( std::vector<int>& m2Lambdas_HistIndex )
{
	m2Lambdas_HistIndex.clear();
	for (int i=0; i < HistoData.size(); i++)
	{
		int bin_offset      = find_proper_bin( HistoData[i] );
		int half_bin_offset = find_proper_bin( HistoData[i]/2.0 );
		
		if (get_count(half_bin_offset) > get_count(bin_offset))
		{
			m2Lambdas_HistIndex.push_back( i );
//			bins[half_bin_offset].count += 2;
//			bins[bin_offset].count--;			
			printf("Found '2 lambda' beat. %6.3lf %6.3lf\n", bins[half_bin_offset].s_value, bins[bin_offset].s_value  );
		}
	}		
}

/* if a HistoValue is 2x a common bin, then split it and count twice for the lower bin. */
void fix_2lambdas( std::vector<int>& m2Lambdas_HistIndex )
{
	for (int i=0; i < m2Lambdas_HistIndex.size(); i++)
	{
		int bin_offset      = find_proper_bin( HistoData[i] );
		int half_bin_offset = find_proper_bin( HistoData[i]/2.0 );

		bins[bin_offset].count--;		
		bins[half_bin_offset].count += 2;


		//int pindex = ((peak_indices[i+1] - peak_indices[i]) / 2.0) + peak_indices[i];		
		// We want the index into the energy array :
		//learn_peak( pindex );
	}
}


