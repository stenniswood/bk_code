/*
 * effects_wave.cpp
 *
 *  Created on: Feb 20, 2021
 *      Author: stephen.tenniswood
 */
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ostream> 

#include "wave.hpp"
#include "dspwave.hpp"
#include "dsp_funcs.h"





EffectsWave::EffectsWave()
{
}
EffectsWave::EffectsWave( BYTE mChannels, int mSamplesPerSecond, int mBufferSizeSamples, short* mData=NULL )
{
}
EffectsWave::~EffectsWave()
{
}

void	EffectsWave::echo   ( float mDelay, float mAlpha )
{
}
void	EffectsWave::tremolo( float LeftConst, float RightConst, int mSamplePeriod )
{
}
void	EffectsWave::pan    ( float LeftConst, float RightConst, int mSamplePeriod )
{
}
void 	EffectsWave::add	(  DSPWave& mNewSound, float mNewVolumeFraction, float mFractionRetain )
{
}


