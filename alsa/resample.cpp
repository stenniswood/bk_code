#include <math.h>
#include <iostream>
#include <map>
#include <string>
#include <iterator>
#include <vector>


float freq_scale_ratios[] = {
	1.0, 1.0/1.059454545, 1.0/1.122454545, 1.0/1.189204545,  1.0/1.259931818,   1.0/1.334840909, 
	1.0/1.41420454545, 1.0/1.49829545454, 1.0/1.58740909090, 1.0/1.68179545454, 1.0/1.78179545454,
	1.0/1.88775, 1.0/2.0  };

float higher_freq_scale_ratios[] = {
	1.0, 1.059454545, 1.122454545, 1.189204545, 1.259931818, 1.334840909, 
	1.41420454545, 1.49829545454, 1.58740909090, 1.68179545454, 1.78179545454,
	1.88770, 2.0  };

std::map<std::string, int> musical_scale;
void init_musical_scale()
{
	// These give Scale indices - ie. index into the frequency table above [0..12]
	musical_scale.insert(std::make_pair("C", 0));
	musical_scale.insert(std::make_pair("C#", 1));
	musical_scale.insert(std::make_pair("Db", 1));
	musical_scale.insert(std::make_pair("D", 2));
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

static short linear_interpolate( short mSample1, short mSample2, float mFraction )
{
	double delta = mSample2 - mSample1;
	double fract_delta = (mFraction * delta)+mSample1;
	return trunc(fract_delta);
}

/* Use this to take sampled sound and resample to another pitch of the musical scale.
	Frequency ratio for the chromatic scale is in freq_sale_ratios[].
Return:		mOutput length The buffer should be same size as input buffer. but not all of it will be used.
				b/c this is upscaling to higher frequency.
*/
static int resample ( short* mInput, short* mOutput, int mLength, float mMultiplier )
{
	int oIndex   = 0;
	float rIndex = mMultiplier;
	float fraction;
	int  sIndex  =0;
	int  eIndex  =1;

	int total_o_samples = round( mLength / mMultiplier );
	for (int oIndex=0; oIndex < total_o_samples; oIndex++)
	{
		rIndex += mMultiplier;
		sIndex = floor( rIndex );
		eIndex = ceil ( rIndex );
		fraction = (rIndex - sIndex);
		mOutput[oIndex] = linear_interpolate( mInput[sIndex], mInput[eIndex], fraction );
	}
	return 1;
}


