#ifndef _ASCII_SOUND_ENCODER_HPP_
#define _ASCII_SOUND_ENCODER_HPP_


void encode_one_nibble( char mByte,  int mNumSamps, short* mPtr, float mSampleRate );

void string_to_wav( int mLength, char* mString,  int mNumSamps, short* mPtr );



#endif
