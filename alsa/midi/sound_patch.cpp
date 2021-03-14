#include <stdint.h>
#include <string>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <math.h>
#include <assert.h>
#include "dspwave.hpp"
#include "resample.hpp"
#include "midi/MidiFile.h"

#include "sound_patch.hpp"




SoundPatch::SoundPatch()
{
	m_min_note = 21;
	m_min_note = 21+88;
	m_filename = "";
}

SoundPatch::SoundPatch( std::string& mWaveFN, int mMinNote, int mMaxNote )
{
	m_min_note = mMinNote;
	m_max_note = mMaxNote;
	m_filename = mWaveFN;
	m_wave.Load(m_filename);
}

SoundPatch::~SoundPatch()
{

}
	
void SoundPatch::set_wave( std::string& mWaveFN, int mMinNote, int mMaxNote )
{
	m_min_note = mMinNote;
	m_max_note = mMaxNote;
	m_filename = mWaveFN;
	m_wave.Load( m_filename );
}

void SoundPatch::stamp_wave( int mMidiNote, short* mData, int mLength )
{
	// Get the frequency:
	int m_index = mMidiNote-21;

	float freq  = scale_freqs[m_index];
	int rFreq   = scale_freqs[m_root_note];
	
	float ratio =  freq / rFreq;
	
	result = resample( m_wave.m_data, &(mData), m_wave.get_samples_recorded(), ratio );	
	
}



