#ifndef _MIDI_SYNTH_HPP_
#define _MIDI_SYNTH_HPP_

#include <string.h>

#include "midi/MidiFile.h"
#include "sound_patch.hpp"

struct stTrackSetup
{
	SoundPatch* sp;
	int			track;
};

class MidiSynth
{
public:
	MidiSynth ();
	~MidiSynth();

	void	read_file		( std::string mFilename );
	
	void 	select_sound	( char* mName, int track );
	
	void	synth_midi		( Wave& wave );		// all tracks

private:
	std::vector<struct stTrackSetup>	m_track;
	smf::MidiFile						m_midi;
};





#endif
