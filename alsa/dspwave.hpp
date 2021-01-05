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

using namespace std;


const int SAMPLE_RATE = 16000;
const int WINDOW_SIZE = 256;
const float FrameRate = 100;

const int NUM_FEATURE_VECTORS = 13 * 1;		// 2 derivates


class DSPWave : public Wave 
{
public:
	DSPWave();
	~DSPWave();


	void	tremolo( float LeftConst, float RightConst, int mSamplePeriod );
	
	
	void 	Preemphasize		( );	// in place!
	void 	PositionWindow		( long int mWindowIndex );
	t_index GetWindowPosition( );
	void 	AdvanceWindow 		 ( );
	
	void			 LoadWindow	 ( );
	vector<short>&	 GetRawSamples( );



	void 	ApplyHammingWindow	( );
	void 	DFT				( );


	bool 	window_is_past_end	( );	// if more samples needed for full window..
	void 	resample 			( float mMultiplier );
	DSPWave& operator=		( DSPWave& mOriginal );
	void 	add	 			(  DSPWave& mNewSound, float mNewVolumeFraction, float mFractionRetain );
	t_real	compute_energy	( int mStartIndex, int mEndIndex );		// short index
	void	scan_energy_only( );
	void 	energy_sort_avg ( );
	void 	print_energies	( );
	void	print_detects	( vector<t_index>& mValues );
	void 	compute_simple_moving_avg();
	void	detect_beat_starts(  );
	void	compute_time_between_beats();
	

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
