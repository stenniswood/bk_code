/*
 * dspwave.hpp
 *
 *  Created on: Dec 20, 2016
 *      Author: stephen.tenniswood
 */

#ifndef _DSPWAVE_HPP_
#define _DSPWAVE_HPP_

#include <string>
#include <vector>
#include "global_vr.h"
#include "wave.hpp"




const int SAMPLE_RATE = 16000;
const int WINDOW_SIZE = 256;
const float FrameRate = 100;

const int NUM_FEATURE_VECTORS = 13 * 1;		// 2 derivates


void save_vector_long( std::vector<long int>& mLongSave, std::string mFilename );
void save_vector	 ( std::vector<t_real>&   mRealSave, std::string mFilename );
	
	
class DSPWave : public Wave 
{
public:
	DSPWave();
	DSPWave( BYTE mChannels, int mSamplesPerSecond, int mBufferSizeSamples, short* mData=NULL );
	~DSPWave();


	void	tremolo( float LeftConst, float RightConst, int mSamplePeriod );
	
	
	void 	Preemphasize		( );	// in place!
	void 	PositionWindow		( long int mWindowIndex );
	t_index GetWindowPosition( );
	void 	AdvanceWindow 		 ( );
	
	void			 LoadWindow	 ( );
	std::vector<short>&	 GetRawSamples( );


	tFeatureVector&  get_features   ( );
	void 			 output_features( std::ostream& out, long int& fv_index, char delim );
	void 			 output_window  ( std::ostream& out, long int& fv_index );
	void 			 output_dft     ( std::ostream& out, long int& fv_index );
	void 			 read_feature_vector( std::ifstream& in, long int& fv_index, std::string& mSymbol );


	void 			ApplyHammingWindow	( );
	void 			DFT				( );


	bool 			window_is_past_end	( );	// if more samples needed for full window..
	void 			resample 			( float mMultiplier );
	DSPWave& 		operator=			( DSPWave& mOriginal );
	void 			add	 				(  DSPWave& mNewSound, float mNewVolumeFraction, float mFractionRetain );
//	t_real	compute_energy_block( int mStartIndex, int mEndIndex );		// short index

	double 					compute_mean			  ( int mStartIndex, int mEndIndex);
	t_real					compute_energy		 	  ( int mStartIndex, int mEndIndex );		// short index
	std::vector<t_real> 	create_energy_contour	  (  );
	std::vector<t_real>		compute_simple_moving_avg ( std::vector<t_real>& mData );
	std::vector<t_index> 	detect_beat_starts		  ( std::vector<t_real>& energies );
	std::vector<t_index> 	detect_beat_starts2		  ( std::vector<t_real>& energies );
	std::vector<t_index> 	remove_close_duplicates	  ( std::vector<t_index>& start_points );
	
	std::vector<t_real>		compute_time_between_beats( std::vector<t_index>& start_points );

	// Auto Correlation:
	double 					compute_auto_correlation ( int mStartIndex, int mEndIndex, int mSampleShift );	// Mono only.
	std::vector<t_real> 	full_auto_correlation 	 ( int mStartIndex, int mEndIndex );	// Mono only.

	void 			print_energies			  ( std::vector<t_real>& mEnergies );
	void			print_detects			  ( std::vector<t_index>& start_points  );
	void 			print_detects_w_time	  ( std::vector<t_index>& start_points, std::vector<t_real>& times  );
	void			create_histogram		  ( std::vector<t_real>& data, int mNumBins );

private:
	long int 	WindowAdvancement;	// 160
	long int 	window_start_index;		// index of the sample starting the window.
	short		window        	[WINDOW_SIZE];
	t_real 		m_energy;
	t_real		freq_real_space [WINDOW_SIZE];
	t_real		freq_imag_space [WINDOW_SIZE];
	t_real		feature_vector[NUM_FEATURE_VECTORS];

};


#endif /* _DSPWAVE_HPP_ */
