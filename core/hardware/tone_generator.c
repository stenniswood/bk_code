#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tone_generator.h"


short Volume = 3000;


const char* musical_scale_names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

float musical_scale_frequencies[] = /* 4th Octave, (starting at middle c) */
{	261.63,
 	277.18,	
	293.66,	
 	311.13,	
	329.63,	
	349.23,	
 	369.99,	
	392.00,	
 	415.30,	
	440.00,	
 	466.16,	
	493.88	};

void sine_wave_1( short* mData, int mPeriodSamples, int mBuffSize, int mDesiredChannel, int mNumChannels )
{
	int i,last_edge = mDesiredChannel;
	float value     = Volume;
	float alpha = (2.*(float)3.14159 / (float)mPeriodSamples);
			
	for (i=mDesiredChannel; i<mBuffSize; i+=mNumChannels)
	{	
		value = Volume * sin(alpha * (float)i);
		mData[i] = round(value);
	}	
}
void sine_wave_left( short* mData, int mPeriodSamples, int mBuffSize )
{
	sine_wave_1( mData, mPeriodSamples, mBuffSize, 0, 2);
}
void sine_wave_right( short* mData, int mPeriodSamples, int mBuffSize )
{
	sine_wave_1( mData, mPeriodSamples, mBuffSize, 1, 2);
}


void saw_tooth_1( short* mData, int mPeriodSamples, int mBuffSize, int mDesiredChannel, int mNumChannels )
{
	int i,last_edge = mDesiredChannel;
	short value     = Volume;
	float increment = (2.*(float)Volume / (float)mPeriodSamples);
			
	for (i=mDesiredChannel; i<mBuffSize; i+=mNumChannels)
	{	
		value -= increment;
		if (value<-Volume)
			value = Volume;
		mData[i] = value;
	}	
}
void saw_tooth_left( short* mData, int mPeriodSamples, int mBuffSize )
{
	saw_tooth_1( mData, mPeriodSamples, mBuffSize, 0, 2);
}
void saw_tooth_right( short* mData, int mPeriodSamples, int mBuffSize )
{
	saw_tooth_1( mData, mPeriodSamples, mBuffSize, 1, 2);
}

void square_wave_1( short* mData, int mPeriodSamples, int mBuffSize, int mDesiredChannel, int mNumChannels )
{
	int i,last_edge=mDesiredChannel;
	short value = Volume;

	short middle = round(mPeriodSamples/2.);
	middle -= (middle % mNumChannels);
		
	for (i=mDesiredChannel; i<mBuffSize; i+=mNumChannels)
	{	
		if ((i % mPeriodSamples)==0)
		{
			last_edge = i;
			value = Volume;
		}		
		else if (i==round(last_edge + middle))
			value = -Volume;		
		mData[i] = value;
	}	
}

void square_wave_left( short* mData, int mPeriodSamples, int mBuffSize )
{
	square_wave_1( mData, mPeriodSamples, mBuffSize, 0, 2);
}

void square_wave_right( short* mData, int mPeriodSamples, int mBuffSize )
{
	square_wave_1( mData, mPeriodSamples, mBuffSize, 1, 2);
}

void silence_channel( short* mData, int mBuffSize, int mChannel )
{
	for (int i=mChannel; i<mBuffSize; i+=2)
		mData[i] = 0;
}



int get_period_in_samples( float Herz, float sample_rate )
{
	float period_samples = (sample_rate/Herz);
	return round(period_samples);
}
 
/*  mIndex may be [0..88]   */
float get_note_frequency( int mIndex, float sample_rate )
{
	// Lowest Note is :  (A,-4)
	const int BASE_OCTAVE     = -4;
	const int BASE_NOTE_INDEX =  9;  // A 
	
	//printf("get_note_frequency()\n");	 
	int   note = (mIndex + BASE_NOTE_INDEX) % 12;
	float freq = musical_scale_frequencies[note];
 
	// determine octave : 
	int multiple = floor(  (float)(mIndex+BASE_NOTE_INDEX) / 12. );
	int o = multiple + BASE_OCTAVE;
 	//printf("%d %d  %6.1f oct=%d %6.1f\t", mIndex, note, sum, o, freq );
 	
	if (o>0)
		while (o)   { freq *= 2.; o--; };
	if (o<0)
		while (o<0) { freq /= 2.; o++; };
	 //printf(" %6.1f\n", freq );	
	return freq; 
}

int get_note_period( int mIndex, int octave, float sample_rate )
{
	int o = (octave-4);
	float freq = musical_scale_frequencies[mIndex];
	if (o>0)
		while (o>0)	{ freq *= 2.; o--; };
	if (o<0)	
		while (o<0) { freq /= 2.; o++; };
	
	return 	get_period_in_samples( freq, sample_rate ); 
}


