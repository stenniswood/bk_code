#ifndef _SOUND_PATCH_HPP_
#define _SOUND_PATCH_HPP_

#include <string>
#include "midi/MidiFile.h"
#include "wave.hpp"


class SoundSource
{
public:
	SoundSource ( );
	SoundSource ( std::string& mWaveFN, int mRootNote, int mMinNote, int mMaxNote );	
	~SoundSource( );
	
	virtual void 	set_wave  	( std::string& mWaveFN, int mRootMidiNote, int mMinNote=21, int mMaxNote=21+88 );
	virtual void 	stamp_wave	( int mMidiNote, float mVolumeRatio, short* mData, int mLength=-1 );
	bool			is_in_range	( int mMidiNote );

	int		get_samples_recorded() { return m_wave.get_samples_recorded(); };
	int		get_root_note() 	   { return m_root_note; };

protected:
	int			m_max_note;		// Above the max, another waveform will be used -- better sampling.
	int			m_min_note;		// 
	Wave		m_wave;
	
private:
	std::string m_filename;
	int			m_root_note;    // ie. the waveform is a piano C2
};


struct stEnvelope 
{
	float 	attack_time;			// 10ms
	float	decay_time;				// 2 seconds
	float 	sustain_time;
	float	release_time;

	float	decay_level;
};

enum eGenType { GS_SQUARE, GS_SINE, GS_TRIANGLE, GS_SAW };

class GendSoundSource : public SoundSource
{
public:
	GendSoundSource		( );
	GendSoundSource		( enum eGenType mType );
	~GendSoundSource	( );
	void			set_type  	( enum eGenType mType )		{ m_type = mType;  };
	virtual void 	set_wave  	( std::string& mWaveFN, int mRootMidiNote, int mMinNote=21, int mMaxNote=21+88 );
	virtual void 	stamp_wave	( int mMidiNote, float mVolume, short* mData, int mLength=-1 );

	void			set_envelope  ( int mSamplesPerSecond, struct stEnvelope m_envelope    );
	float			phase_tracking( bool mRestart, float mMaxVolume );
	
	void			gen_square	( int mMidiNote, float mVolume, short* mData, int mLength );
	void			gen_triangle( int mMidiNote, float mVolume, short* mData, int mLength );
	void			gen_saw		( int mMidiNote, float mVolume, short* mData, int mLength );
	void			gen_sine	( int mMidiNote, float mVolume, short* mData, int mLength, int mUnisonNotes=0 );
	
private:
	enum eGenType	m_type;
	struct stEnvelope m_envelope;
	
	int				m_attack_samples;
	int				m_decay_samples;
	int				m_sustain_samples;	
	int				m_release_samples;
	int				m_phase;
	int				m_phase_counter;
};

////////////////////////////////////////////////////////////////////

class SoundPatch
{
public:
	SoundPatch();
	~SoundPatch();
	
	int		find_proper_source	( int mMidiNote );
	void 	stamp_wave			( int mMidiNote, float mVolume, short* moData, int mLength=-1 );
	int		get_samples_recorded( int mMidiNote);			
	void 	midi_track_to_wave	( smf::MidiFile& midifile, int track, Wave& wave );
	
protected:
	std::vector<SoundSource*>	m_ss;
};

class PianoSoundPatch : public SoundPatch
{
public:
	PianoSoundPatch();
	~PianoSoundPatch();
	
private:
};

class SquareSoundPatch : public SoundPatch
{
public:
	SquareSoundPatch();
	~SquareSoundPatch();
};
class TrumpetSoundPatch : public SoundPatch
{
public:
	TrumpetSoundPatch();
	~TrumpetSoundPatch();
};
class BrassSoundPatch : public SoundPatch
{
public:
	BrassSoundPatch();
	~BrassSoundPatch();
};
class SynthBrassSoundPatch : public SoundPatch
{
public:
	SynthBrassSoundPatch();
	~SynthBrassSoundPatch();
};

#endif
