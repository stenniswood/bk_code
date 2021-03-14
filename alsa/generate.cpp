#include <math.h>
#include <assert.h>
#include "dspwave.hpp"
#include "resample.hpp"
#include "midi/MidiFile.h"



void square_wave( int mAmplitude, int mPeriodSamples, short* mData, int mLength )
{
	int half = mPeriodSamples>>2;
	
	for (int i=0; i<mLength; i++)
	{
		if ((i%mPeriodSamples)<half)
			mData[i] = mAmplitude;
		else
			mData[i] = 0;
	}
}

void saw_wave( int mAmplitude, int mPeriodSamples, short* mData, int mLength )
{
	int dec = mAmplitude / mPeriodSamples;
	int mValue = mAmplitude;
	
	for (int i=0; i<mLength; i++)
	{
		mData[i] = mValue - (mAmplitude>>2);
		mValue -= dec;
		if (mValue <=0) mValue = mAmplitude;	
	}

}

void sine_wave( int mAmplitude, int mPeriodSamples, short* mData, int mLength )
{
	int dec = mAmplitude / mPeriodSamples;
	
	for (int i=0; i<mLength; i++)
	{
		mData[i] = mAmplitude * sin( i/ mPeriodSamples * 2*M_PI );
	}
}



int play_note( char* mNoteName, int mAmplitude, short* mData, int mDuration )
{
	int PeriodSamps = get_note_period( mNoteName );
	square_wave( mAmplitude, PeriodSamps, mData, mDuration*0.8 );
	
	return mDuration;
}

#define MIDI_ZERO_OFFSET 21
int play_note_number( int mNoteNum, int mAmplitude, short* mData, int mDuration )
{
	int PeriodSamps = get_note_num_period( mNoteNum-MIDI_ZERO_OFFSET );
	square_wave( mAmplitude, PeriodSamps, mData, mDuration );
	
	return mDuration;
}


void midi_track_to_wave( smf::MidiFile& midifile, int track, DSPWave& wave )
{
	assert( track < midifile.getTrackCount());
	
    for (int event=0; event < midifile[track].size(); event++) 
    {	
    	if (midifile[track][event].isNoteOn()) {
    		float dur_secs = midifile[track][event].getDurationInSeconds();
    		int   duration = round(wave.TimeToSamples( dur_secs ));
			int   samples  = round(wave.TimeToSamples( midifile[track][event].seconds ));
			int note_num   = midifile[track][event][1];
			int velocity   = midifile[track][event][2];
			int Amplitude  = 16384 * velocity / 255.0;
			//printf("%s : %6.3f\n", notes[note_num-21], scale_freqs[note_num-21] );
			play_note_number( note_num, Amplitude, &(wave.m_data[samples]), duration );
		}
	}
}
