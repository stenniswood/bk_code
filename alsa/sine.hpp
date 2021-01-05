#ifndef _SINE_HPP_
#define _SINE_HPP_



float frequency_to_samples( float mFrequency, float mSampleRate );
float samples_to_frequency( float mSamples, float mSampleRate );

void create_sinewave_stereo( int mDesiredChannel, int mAmplitude, float mPeriodInSamples, int mNumSamps, short* mPtr );
void create_sinewave_mono( int mDesiredChannel, int mAmplitude, int mNumSamps, short* mPtr );

void create_sinewave_stereo_hz( int mDesiredChannel, int mAmplitude, float mHerz, 
								int mNumSamps, short* mPtr, float mSampleRate );
void create_sinewave_mono_hz  ( int mAmplitude, float mHerz, 
								int mNumSamps, short* mPtr, float mSampleRate );
								


#endif
