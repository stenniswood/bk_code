/*
 * dsp_effects_wave.hpp
 *
 *  Created on: Dec 20, 2016
 *      Author: stephen.tenniswood
 */

#ifndef _EFFECTS_WAVE_HPP_
#define _EFFECTS_WAVE_HPP_

#include <string>
#include <vector>
#include "global_vr.h"
#include "wave.hpp"


	
	
class EffectsWave : public Wave 
{
public:
	EffectsWave();
	EffectsWave( BYTE mChannels, int mSamplesPerSecond, int mBufferSizeSamples, short* mData=NULL );
	~EffectsWave();


	void	echo   ( float mDelay, float mAlpha );
	void	tremolo( float LeftConst, float RightConst, int mSamplePeriod );
	void	pan    ( float LeftConst, float RightConst, int mSamplePeriod );

	void 	add(  DSPWave& mNewSound, float mNewVolumeFraction, float mFractionRetain );
		
	void 			DFT				( );
	void 			resample 		( float mMultiplier );
	EffectsWave& 	operator=		( EffectsWave& mOriginal );



private:
	long int 	WindowAdvancement;	// 160
	long int 	window_start_index;		// index of the sample starting the window.
	short		window        	[WINDOW_SIZE];
	t_real 		m_energy;
	t_real		freq_real_space [WINDOW_SIZE];
	t_real		freq_imag_space [WINDOW_SIZE];
	t_real		feature_vector[NUM_FEATURE_VECTORS];

};


#endif /* _EFFECTS_WAVE_HPP_ */

