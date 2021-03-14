#ifndef _GENERATE_HPP_
#define _GENERATE_HPP_



int play_note( char* mNoteName, int mAmplitude, short* mData, int mDuration );
int play_note_number( int mNoteNum, int mAmplitude, short* mData, int mDuration );

void midi_track_to_wave(  smf::MidiFile& midifile, int track, DSPWave& wave );

void square_wave( int mAmplitude, int mPeriodSamples, short* mData, int mLength );
void saw_wave( int mAmplitude, int mPeriodSamples, short* mData, int mLength );
void sine_wave( int mAmplitude, int mPeriodSamples, short* mData, int mLength );



#endif
