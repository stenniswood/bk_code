#include <string.h>

//#include "midi/MidiFile.h"
#include "wave.hpp"
#include "midi_synth.hpp"





MidiSynth::MidiSynth ()
{
}
MidiSynth::~MidiSynth()
{
}

void	MidiSynth::read_file( std::string mFilename )
{
	m_midi.read			 ( mFilename.c_str() );
	m_midi.doTimeAnalysis( );
    m_midi.linkNotePairs ( );
}

void 	MidiSynth::select_sound( char* mName, int track )
{
	struct stTrackSetup ts;
	ts.track = track;
	
	if (strcmp(mName, "Piano")==0)			{  ts.sp = new PianoSoundPatch(); 	   }
	else if (strcmp(mName, "Square")==0)  	{  ts.sp = new SquareSoundPatch(); 	   }
	else if (strcmp(mName, "Brass")==0)		{  ts.sp = new BrassSoundPatch(); 	   }
	else if (strcmp(mName, "Trumpet")==0)	{  ts.sp = new TrumpetSoundPatch();    }
	else if (strcmp(mName, "SynthBrass")==0){  ts.sp = new SynthBrassSoundPatch(); };
	m_track.push_back(ts);
}

void	MidiSynth::synth_midi( Wave& wave )		// all tracks
{
	size_t size = m_track.size();
	for (int t=0; t<size; t++)
	{
		m_track[t].sp->midi_track_to_wave( m_midi, m_track[t].track, wave );
	}	
}
