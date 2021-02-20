#ifndef _HISTO_BIN_HPP_
#define _HISTO_BIN_HPP_


#include <vector>


extern std::vector<double> HistoData;


struct stBinInfo {
	double   s_value;
	double   e_value;	
	long int count;
};

void min_max_data	( std::vector<double>& mHistData,  double& mMin, double& mMax );

void set_data_I		( std::vector<int> mData   );
void set_data_S		( std::vector<short> mData );
void init_bins		( int mNumBins );

void print_bins		( );
long int get_count	( int mBinIndex );

int  find_proper_bin( double mDataPoint 		);
void bin_data		( );
int  get_mode		( );
void get_2lambdas	( std::vector<int>& m2Lambdas_HistIndex );
void fix_2lambdas	( std::vector<int>& m2Lambdas_HistIndex );

#endif
