#include <math.h>


#include "sine.hpp"



float frequency_to_samples( float mFrequency, float mSampleRate )
{
	float period = 1 / mFrequency;
	float SampsPerCycle = period * mSampleRate;
	return SampsPerCycle;
}

float samples_to_frequency( float mSamples, float mSampleRate )
{
	float periodsPerSec = mSampleRate / mSamples;
	return periodsPerSec;
}

void create_sinewave_stereo( int mDesiredChannel, int mAmplitude, float mPeriodInSamples, int mNumSamps, short* mPtr )
{
	float alpha = 1 / mPeriodInSamples;
	float phase = 0.0;
	
	for (int i=0; i<mNumSamps; i+=2)
	{
		if (mDesiredChannel==0)
			mPtr[i] = mAmplitude * sin( i * alpha + phase );
		else if (mDesiredChannel==1)
			mPtr[i+1] = mAmplitude * sin( i * alpha + phase );
		else  {
			mPtr[i]   = mAmplitude * sin( i * alpha + phase );
			mPtr[i+1] = mAmplitude * sin( i * alpha + phase );
		}	
	}
}

void create_sinewave_mono( int mDesiredChannel, int mAmplitude, int mPeriodInSamples, 
							int mNumSamps, short* mPtr )
{
	float alpha = 1 / mPeriodInSamples;
	float phase = 0.0;

	for (int i=0; i<mNumSamps; i++)
	{
		mPtr[i] = mAmplitude * sin( i * alpha + phase );
	}
}

void create_sinewave_stereo_hz( int mDesiredChannel, int mAmplitude, float mHerz, 
								int mNumSamps, short* mPtr, float mSampleRate )
{
	float periodSamps = frequency_to_samples( mHerz,  mSampleRate );
	create_sinewave_stereo( mDesiredChannel, mAmplitude,  periodSamps, mNumSamps, mPtr );
	
}

void create_sinewave_mono_hz  ( int mAmplitude, float mHerz, 
								int mNumSamps, short* mPtr, float mSampleRate )
{
	float periodSamps = frequency_to_samples( mHerz,  mSampleRate );
	create_sinewave_stereo( 2, mAmplitude,  periodSamps, mNumSamps, mPtr );	
}

