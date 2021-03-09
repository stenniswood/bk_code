#ifndef _PEAK_PICK_HPP_
#define _PEAK_PICK_HPP_



#include <vector>


extern std::vector<double> RawData;
extern std::vector<double> SmoothData;


struct stPeakInfo {
	int	   	SmoothIndex;
	double   e_value;
};

struct stPulseCharacterization 
{
	float	leading_edge_factor;	// multiple of data point prior to 
	float	sustain_duration;		// time before falling edge
	float	fall_level;				// multiple of data point prior to 
	float	fall_rate;				// db per window
};


//void min_max_data	( std::vector<double>& mHistData,  double& mMin, double& mMax );

void set_peakdata_I		( std::vector<int> mData   );
void set_peakdata_D		( double mValues[], size_t mLength );

void smooth_data	( int mSmoothSize );
void find_peaks		( );

void init_peaks		( int mNumBins );
void print_peaks	( );





void get_2lambdas	( std::vector<int>& m2Lambdas_HistIndex );
void fix_2lambdas	( std::vector<int>& m2Lambdas_HistIndex );






#endif



