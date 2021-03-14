#ifndef _SOUND_PATCH_HPP_
#define _SOUND_PATCH_HPP_

#include <string>



class SoundPatch
{
public:
	SoundPatch ( );
	SoundPatch ( std::string& mWaveFN, int mMinNote, int mMaxNote );	
	~SoundPatch( );
	
	void 	set_wave  ( std::string& mWaveFN, int mMinNote=21, int mMaxNote=21+88 );
	void 	stamp_wave( int mMidiNote, short* mData, int mLength );
	

protected:


private:
	std::string m_filename;
	Wave	m_wave;
	int		m_root_note;    // ie. the waveform is a piano C2
	int		m_max_note;		// Above the max, another waveform will be used -- better sampling.
	int		m_min_note;		// 

};


#endif
