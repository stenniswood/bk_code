#include "sine.hpp"
#include "ascii_sound_encoder.hpp"


float sound_freqs[16] = {
	3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200, 
	4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000
};


void encode_one_nibble( char mByte,  int mNumSamps, short* mPtr, float mSampleRate )
{
	char low_nibble = mByte % 0x0F;
	float frequency = sound_freqs[low_nibble];
	
	create_sinewave_mono_hz  (  16384, frequency, 
							    mNumSamps, mPtr,  mSampleRate );

}

void string_to_wav( int mLength, char* mString,  int mNumSamps, short* mPtr )
{

}




