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




SoundSource::SoundSource()
{
	m_min_note  = 21;
	m_min_note  = 21+88;
	m_root_note = 0;
	m_filename  = "";
}

SoundSource::SoundSource( std::string& mWaveFN, int mRootNote, int mMinNote, int mMaxNote )
{
	m_min_note = mMinNote;
	m_max_note = mMaxNote;
	m_root_note = 0;
	m_filename = mWaveFN;
	//printf("Loading : %s\n", m_filename.c_str() );	
	m_wave.Load(m_filename);
}

SoundSource::~SoundSource()
{

}
	
void SoundSource::set_wave( std::string& mWaveFN, int mRootMidiNote, int mMinNote, int mMaxNote )
{
	m_min_note = mMinNote;
	m_max_note = mMaxNote;
	m_filename = mWaveFN ;
	m_root_note = mRootMidiNote-21;
	m_wave.Load( m_filename );
}

void SoundSource::stamp_wave( int mMidiNote, float mVolumeRatio, short* mData, int mLength )
{
	if (mLength == -1)
		mLength = m_wave.get_samples_recorded();

	// Get the frequency:
	int   m_index = mMidiNote-21+12;

	float freq  = scale_freqs[m_index];
	float rFreq = scale_freqs[m_root_note];
	
	float ratio =  freq / rFreq;
	printf("Golden ratio: %6.3f\n", ratio );
	// m_wave.get_samples_recorded()
	int   result = resample( m_wave.m_data, mData, mVolumeRatio, mLength, ratio );
}

bool SoundSource::is_in_range( int mMidiNote )
{
	//printf("min=%d;  max=%d\n", m_min_note, m_max_note);
	bool inrange = ((mMidiNote >= m_min_note) && (mMidiNote <= m_max_note));
	return inrange;
}

////////////////////////////////////////////////////////////
//  Sound Patch is for generated sound sources (such as square wave,
//	sinewave, saw, triangle, etc)
//  with different root note samples)
////////////////////////////////////////////////////////////

GendSoundSource::GendSoundSource()
{
	m_type = GS_SQUARE;
	m_envelope.attack_time  = 0.0;		// 
	m_envelope.decay_time   = 200.0;	// 
	m_envelope.release_time = 60*10;	// 	19 minutes (essentially no release)
	m_envelope.decay_level  = 1.0;		//  1.0 * max volume
	set_envelope( 44100, m_envelope );

	m_min_note 		= 21;
	m_max_note 		= 21+88;
}
GendSoundSource::GendSoundSource( enum eGenType mType )
{
	m_type = mType;
	m_envelope.attack_time  = 0.0;		// 
	m_envelope.decay_time   = 200.0;	// 
	m_envelope.release_time = 60*10;	// 	19 minutes (essentially no release)
	m_envelope.decay_level  = 1.0;		//  1.0 * max volume
	set_envelope( 44100, m_envelope );

	m_min_note 		= 21;
	m_max_note 		= 21+88;	
}

GendSoundSource::~GendSoundSource()
{
}
void GendSoundSource::set_wave( std::string& mWaveFN, int mRootMidiNote, int mMinNote, int mMaxNote )
{
// ignore mWaveFN
// ignore mRootMidiNote
	m_min_note = 21;
	m_max_note = 21+88;
}

void GendSoundSource::set_envelope( int mSamplesPerSecond, struct stEnvelope mEnvelope )
{
	m_envelope = mEnvelope;

	m_attack_samples  = mSamplesPerSecond * m_envelope.attack_time  ;
	m_decay_samples   = mSamplesPerSecond * m_envelope.decay_time   ;
	m_sustain_samples = mSamplesPerSecond * m_envelope.sustain_time ;	
	m_release_samples = mSamplesPerSecond * m_envelope.release_time ;
	m_phase         = 0;
	m_phase_counter = 0;	
	
	printf("set_envelope() : Attack Samples %6.3f %d\n", m_envelope.attack_time, m_attack_samples );
	printf(": Decay Samples %6.3f %d\n",   m_envelope.decay_time,   m_decay_samples   );
	printf(": Release Samples %6.3f %d\n", m_envelope.release_time, m_release_samples );	
	
	printf(": Decay Level %6.3f\n", 	 m_envelope.decay_level );
}

#define ATTACK 0
#define DECAY  1
#define SUSTAIN 2
#define RELEASE 3


/* Return is a fraction of max volume */
float GendSoundSource::phase_tracking( bool mRestart, float mMaxVolume )
{	
	float VolumeFraction = 1.0;
		
	if (mRestart)  {  m_phase=0;  m_phase_counter=0;  };
	
	// ENVELOPE SHAPING:
	m_phase_counter++;
	float decayVol = mMaxVolume * m_envelope.decay_level;

	switch(m_phase)
	{
	case ATTACK :  if (m_phase_counter >= m_attack_samples) 
					{ m_phase++;  m_phase_counter=0; printf("New Phase\n"); };
					VolumeFraction = ((float)m_phase_counter / (float)m_attack_samples) * mMaxVolume;
			break;
	case DECAY :	if (m_phase_counter >= m_decay_samples ) 
					{ m_phase++;  m_phase_counter=0; printf("New Phase\n"); };
					VolumeFraction = mMaxVolume - ((float)m_phase_counter / (float)m_decay_samples) * ( mMaxVolume-decayVol );
			break;
	case SUSTAIN :	if (m_phase_counter >= m_sustain_samples ) 
					{ m_phase++;  m_phase_counter=0; printf("New Phase\n"); };
					VolumeFraction = ( decayVol );
			break;

	case RELEASE :  if (m_phase_counter >= m_release_samples ) 
					{ m_phase++;  m_phase_counter=0; printf("New Phase\n"); };
					VolumeFraction = decayVol - 
							((float)m_phase_counter / (float)m_release_samples) * decayVol;
			break;
	default : 
			break;	
	}	
	//printf("%6.3f  ", VolumeFraction);
	return VolumeFraction;
}

#define GEND_AMPLITUDE 16384

void GendSoundSource::gen_sine( int mMidiNote, float mVolume, short* mData, int mLength )
{
	int   PeriodSamps = get_note_num_period( mMidiNote-21 );
	int   max_ampl        = GEND_AMPLITUDE * mVolume;
	//printf("max amplitude=%d\n", max_ampl );
	
	short newdata = 0;
	int phase = 0;		// attack
	int phase_counter = 0;

	float ampl = phase_tracking( true, max_ampl );
			
	for (int i=0; i<mLength; i++)
	{
		// ENVELOPE SHAPING:
		float ampl = phase_tracking(false, max_ampl);	
		//float ampl = max_ampl;
		
		// WAVEFORM:
		newdata = ampl * sin( (float)i/(float)PeriodSamps * 2.0*M_PI );
		mData[i] = add_value (mData[i], newdata );
	}
}

void GendSoundSource::gen_saw( int mMidiNote, float mVolume, short* mData, int mLength )
{
	int   PeriodSamps = get_note_num_period( mMidiNote-21 );

	int dec 	 = GEND_AMPLITUDE / PeriodSamps;
	int new_data  = GEND_AMPLITUDE;
	int value    = 0;

	for (int i=0; i<mLength; i++)
	{
		value    -= dec;
		if (value <=0) 	value = GEND_AMPLITUDE;

		new_data   = value - ( GEND_AMPLITUDE >> 1 );
		mData[i]   = add_value( mData[i], new_data  );
	}
}

void GendSoundSource::gen_triangle( int mMidiNote, float mVolume, short* mData, int mLength )
{
	int   PeriodSamps = get_note_num_period( mMidiNote-21 );
	int   dec 	 	  = GEND_AMPLITUDE / (PeriodSamps >> 1);
	
	short new_data     = mVolume*(GEND_AMPLITUDE-GEND_AMPLITUDE/2);
	short value 	  = GEND_AMPLITUDE;

	for (int i=0; i<mLength; i++)
	{
		value      -= dec;
		if (value <= 0) 			  dec = -fabs(dec);
		if (value >= GEND_AMPLITUDE)  dec = fabs(dec);

		new_data   = value - ( GEND_AMPLITUDE >> 1 );
		mData[i] = add_value (mData[i], new_data );
	}
}

void GendSoundSource::gen_square( int mMidiNote, float mVolume, short* mData, int mLength )
{
	int   PeriodSamps = get_note_num_period( mMidiNote-21 );
	int   half        = PeriodSamps >> 2;
	short newdata     = mVolume*(GEND_AMPLITUDE-GEND_AMPLITUDE/2);

	for (int i=0; i<mLength; i++)
	{
		if ((i%PeriodSamps)<half) {
			mData[i] = add_value( mData[i], newdata );
		} else 
			mData[i] = 0;
	}
}

void GendSoundSource::stamp_wave( int mMidiNote, float mVolume, short* mData, int mLength )
{
	switch (m_type)
	{
	case GS_SQUARE   : gen_square	(mMidiNote, mVolume, mData, mLength);	break;
	case GS_SINE     : gen_sine		(mMidiNote, mVolume, mData, mLength);	break;
	case GS_TRIANGLE : gen_triangle	(mMidiNote, mVolume, mData, mLength);	break;
	case GS_SAW      : gen_saw		(mMidiNote, mVolume, mData, mLength);	break;
	default:  break;
	};
}

////////////////////////////////////////////////////////////
//  Sound Patch is a collection of SoundSources (ie. 
//  with different root note samples)
////////////////////////////////////////////////////////////
SoundPatch::SoundPatch()
{
}
SoundPatch::~SoundPatch()
{
}

int	SoundPatch::find_proper_source( int mMidiNote )
{
	for (int i=0; i<m_ss.size(); i++)
		if (m_ss[i]->is_in_range( mMidiNote ))
			return i;
	return -1;		
}

void SoundPatch::stamp_wave( int mMidiNote, float mVolume, short* moData, int mLength )
{
	int ss_index = find_proper_source( mMidiNote );
	//printf("ss_index=%d midinote=%d;  size=%d\n", ss_index, mMidiNote, m_ss.size() );/
	printf("Soundsource[%d] root=%s : note=%d %s  \n", 
		ss_index, notes[m_ss[ss_index]->get_root_note()], 
		mMidiNote, notes[mMidiNote-21] );

	m_ss[ss_index]->stamp_wave( mMidiNote, mVolume, moData, mLength );	
}

int	SoundPatch::get_samples_recorded( int mMidiNote )
{
	int ss_index = find_proper_source( mMidiNote );	
	return m_ss[ss_index]->get_samples_recorded();
}

void SoundPatch::midi_track_to_wave	( smf::MidiFile& midifile, int track, Wave& wave )
{
	assert( track < midifile.getTrackCount());
    printf("Track %d\n", track);
    		
    for (int event=0; event < midifile[track].size(); event++) 
    {	

    	if (midifile[track][event].isNoteOn()) {
    		float dur_secs = midifile[track][event].getDurationInSeconds();
    		int   duration = 2*round(wave.TimeToSamples( dur_secs ));
			int   samples  = round(wave.TimeToSamples( midifile[track][event].seconds ));
			int note_num   = midifile[track][event][1];
			int velocity   = midifile[track][event][2];
			float Amplitude= velocity / 255.0;
			//printf("%s : %6.3f\n", notes[note_num-21], scale_freqs[note_num-21] );
			stamp_wave( note_num, Amplitude, &(wave.m_data[samples]), duration );
		}
	}
}


////////////////////////////////////////////////////////////
//  Piano Sound Patch is a collection of SoundSources (ie. 
//  with different root note samples)
////////////////////////////////////////////////////////////
PianoSoundPatch::PianoSoundPatch()
{
	SoundSource* inst = new SoundSource();
	
	std::string inst_fn("midi/C1v10.wav");			// ss 0
	inst->set_wave   ( inst_fn, 24, 21, 29 );
	m_ss.push_back   ( inst );
	inst_fn = "midi/F#1v10.wav";					// ss 1
	inst    = new SoundSource();
	inst->set_wave  ( inst_fn, 30, 30, 35 );
	m_ss.push_back  ( inst );


	inst_fn = "midi/C2v10.wav";						// ss 2
	inst    = new SoundSource();
	inst->set_wave  ( inst_fn, 36, 36, 41 );
	m_ss.push_back  ( inst );
	inst_fn = "midi/F#2v10.wav";					// ss 3
	inst    = new SoundSource();
	inst->set_wave  ( inst_fn, 42, 42, 47 );
	m_ss.push_back  ( inst );

	
	inst_fn = "midi/C3v10.wav";						// ss 4
	inst    = new SoundSource();	
	inst->set_wave  ( inst_fn, 48, 48, 53 );
	m_ss.push_back ( inst );
	inst_fn = "midi/F#3v10.wav";					// ss 5
	inst    = new SoundSource();
	inst->set_wave  ( inst_fn, 54, 54, 59 );
	m_ss.push_back  ( inst );

	
	inst_fn = "midi/C4v10.wav";						// ss 6
	inst    = new SoundSource();	
	inst->set_wave  ( inst_fn, 60, 60, 65 );
	m_ss.push_back ( inst );
	inst_fn = "midi/F#4v10.wav";					// ss 7
	inst    = new SoundSource();
	inst->set_wave  ( inst_fn, 66, 66, 71 );
	m_ss.push_back  ( inst );


	inst_fn = "midi/C5v10.wav";						// ss 8
	inst    = new SoundSource();	
	inst->set_wave  ( inst_fn, 72, 72, 77 );
	m_ss.push_back ( inst );
	inst_fn = "midi/F#5v10.wav";					// ss 9
	inst    = new SoundSource();
	inst->set_wave  ( inst_fn, 78, 78, 83 );
	m_ss.push_back  ( inst );


	inst_fn = "midi/C6v10.wav";						// ss 10
	inst    = new SoundSource();	
	inst->set_wave  ( inst_fn, 84, 84, 89 );
	m_ss.push_back ( inst );
	inst_fn = "midi/F#6v10.wav";					// ss 11
	inst    = new SoundSource();
	inst->set_wave  ( inst_fn, 90, 90, 95 );
	m_ss.push_back  ( inst );

	inst_fn = "midi/C7v10.wav";						// ss 12
	inst = new SoundSource();	
	inst->set_wave  ( inst_fn, 96, 96, 101 );
	m_ss.push_back ( inst );
	inst_fn = "midi/F#7v10.wav";					// ss 13
	inst    = new SoundSource();
	inst->set_wave  ( inst_fn, 102, 102, 107 );
	m_ss.push_back  ( inst );

	inst_fn = "midi/C8v10.wav";						// ss 14
	inst = new SoundSource();	
	inst->set_wave  ( inst_fn, 108, 108, 110 );
	m_ss.push_back ( inst );

	printf("PianoSoundPatch() ctor done.\n");	
}

PianoSoundPatch::~PianoSoundPatch()
{
	
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
SquareSoundPatch::SquareSoundPatch()
{
	GendSoundSource* sq = new GendSoundSource();
	m_ss.push_back( sq );
}
SquareSoundPatch::~SquareSoundPatch()	{  delete m_ss[0];	}

///////////////////////////////////////////////////////////
///////////////////////Trumpet////////////////////////////////////
TrumpetSoundPatch::TrumpetSoundPatch()
{
	SoundSource* inst = new SoundSource();
	
	std::string inst_fn("midi/brass-trumpet-note-one-shot_110bpm_C.wav");			// ss 0
	inst->set_wave   ( inst_fn, 48, 21, 21+88 );
	m_ss.push_back   ( inst );	
}
TrumpetSoundPatch::~TrumpetSoundPatch() { delete m_ss[0]; }

///////////////////////////////////////////////////////////
/////////////////////////Brass//////////////////////////////////
BrassSoundPatch::BrassSoundPatch()
{
	SoundSource* inst = new SoundSource();
	
	std::string inst_fn("midi/vintage-analog-trumpet-hit-casio-c-5_140bpm_C_major.wav");			// ss 0
	inst->set_wave   ( inst_fn, 48, 21, 21+88 );
	m_ss.push_back   ( inst );	
}
BrassSoundPatch::~BrassSoundPatch() { delete m_ss[0]; }

///////////////////////////////////////////////////////////
//////////////////////////SynthBrass/////////////////////////////////
SynthBrassSoundPatch::SynthBrassSoundPatch()
{
	SoundSource* inst = new SoundSource();
	
	std::string inst_fn("midi/SynthBrass_C.wav");			// ss 0
	inst->set_wave   ( inst_fn, 48, 21, 21+88 );
	m_ss.push_back   ( inst );	
}
SynthBrassSoundPatch::~SynthBrassSoundPatch() { delete m_ss[0]; }

 