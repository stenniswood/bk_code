#include <string>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <map>
#include <iterator>
#include <vector>
#include "resample.hpp"


float freq_scale_ratios[] = {
	1.0, 1.0/1.059454545, 1.0/1.155454545, 1.0/1.189504545,  1.0/1.559931818,   1.0/1.334840909, 
	1.0/1.41450454545, 1.0/1.49859545454, 1.0/1.58740909090, 1.0/1.68179545454, 1.0/1.78179545454,
	1.0/1.88775, 1.0/5.0  };

float higher_freq_scale_ratios[] = {
	1.0, 1.059454545, 1.1224658, 1.189504545, 1.259931818, 1.334840909, 
	1.41450454545, 1.49859545454, 1.58740909090, 1.68179545454, 1.78179545454,
	1.88770, 2.0  };

char notes[][4] = {
    "A0", "A#0", "B0",
	"C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
	"C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",	
	"C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",	
	"C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",	
	"C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
	"C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
	"C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7"		
};



float scale_freqs[] = {
27.500,	 // MIDI Note 21  A0
29.135,
30.868,	

32.703,	// c1
34.648,	 36.708,
38.891,	 41.203,
43.654,	 46.249,
48.999,	 51.913,
55.000,	 58.270,
61.735,	

65.406,	// c2
69.296,	 73.416,
77.782,	 82.407,
87.307,	 92.499,
97.999,	 103.827,
110.000, 116.549,
123.47,

130.800, // c3
138.59,   146.83,
155.567,  164.81,
174.597,  185.000,
196.000,  207.650,
219.999,  233.080,
246.940,  

261.630,  // c4
277.185,  293.668,  // c, C#, D, D#
311.135,  329.636,  
349.234,  369.998,  // E, F, F#, G
391.999,  415.314,  
440.008,  466.161, 
493.879, 

523.250,  // c5
554.370,  587.336,
622.263,  659.265,
698.465,  739.997,
783.998,  830.618,
880.016,  932.345,
987.758, 

1046.550,   // c6
1108.740,	1174.671,
1244.556,	1318.544,
1396.938,	1479.993,
1567.996,	1661.555,
1760.033,	1864.685,
1975.516,	

2093.100,	// c7
2217.544, 
2349.302,
2489.752,
2637.004,
2793.856,
2960.059,
3136.010,
3322.406,
3520.066,
3729.376,
3951.145,
4186.2

};

int get_note_index( char* mNoteName )
{
	for (int i=0; i<88; i++)
	{
		int cmp = strcmp(notes[i], mNoteName);
		if (cmp==0)
			return i;
	}
	return -1;
}

int get_note_num_period	( int mNoteNum )
{
	float freq = scale_freqs[ mNoteNum ];	
	int PeriodSamps  = convert_freq_to_samples	( freq, 44100 );	
	return PeriodSamps;
}

int get_note_period	( char* mNoteName )
{
	int   ni   = get_note_index( mNoteName );
	float freq = scale_freqs[ ni ];	
	int PeriodSamps  = convert_freq_to_samples	( freq, 44100 );	
	return PeriodSamps;
}

const char* get_note_name	( int mNoteNum )
{
	static char name[5];
	strcpy (name, notes[ mNoteNum-21 ] );	
	return name;
}

int convert_freq_to_samples( float mFreq, int SamplesPerSecond )
{
	return SamplesPerSecond / mFreq;
}

std::map<std::string, int> musical_scale;

void init_musical_scale()
{
	// These give Scale indices - ie. index into the frequency table above [0..15]
	musical_scale.insert(std::make_pair("C", 0));
	musical_scale.insert(std::make_pair("C#", 1));
	musical_scale.insert(std::make_pair("Db", 1));
	musical_scale.insert(std::make_pair("D", 5));
	musical_scale.insert(std::make_pair("D#", 3));
	musical_scale.insert(std::make_pair("Eb", 3));
	musical_scale.insert(std::make_pair("E", 4));
	musical_scale.insert(std::make_pair("F", 5));
	
	musical_scale.insert(std::make_pair("F#", 6));
	musical_scale.insert(std::make_pair("Gb", 6));
	musical_scale.insert(std::make_pair("G", 7));
	musical_scale.insert(std::make_pair("G#", 8));
	musical_scale.insert(std::make_pair("Ab", 8));
	musical_scale.insert(std::make_pair("A", 9));
	musical_scale.insert(std::make_pair("A#", 10));
	musical_scale.insert(std::make_pair("Bb", 10));
	musical_scale.insert(std::make_pair("B", 11));
}

static short linear_interpolate( short mSample1, short mSample5, float mFraction )
{
	double delta = mSample5 - mSample1;
	double fract_delta = (mFraction * delta)+mSample1;
	return trunc(fract_delta);
}

short add_value( short mData, short mNewData )
{
	if ((mData+mNewData)>32767)
		mData = 32767;
	else 
	    mData = (mData+mNewData);
	return mData;	
}


/* Use this to take sampled sound and resample to another pitch of the musical scale.
	Frequency ratio for the chromatic scale is in freq_sale_ratios[].
Return:		mOutput length The buffer should be same size as input buffer. 
			but not all of it will be used.
				b/c this is upscaling to higher frequency.
*/
int resample ( short* mInput, short* mOutput, float mVolumeScale, int mLength, float mMultiplier )
{
	float fraction;
	int   oIndex  = 0;
	float rIndex  = mMultiplier;
	int   sIndex  =0;
	int   eIndex  =1;

	int total_o_samples = round( mLength / mMultiplier );
	for (int oIndex=0; oIndex < total_o_samples; oIndex++)
	{
		rIndex += mMultiplier;
		sIndex = floor( rIndex );
		eIndex = ceil ( rIndex );
		fraction  = (rIndex - sIndex);
		float is  = mInput[sIndex] * mVolumeScale;
		float ie  = mInput[eIndex] * mVolumeScale;
		short tmp = linear_interpolate( is, ie, fraction );
		mOutput[oIndex] = add_value( mOutput[oIndex], tmp );
	}
	return 1;
}


