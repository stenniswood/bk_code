#ifndef _BEAT_GEN_HPP_
#define _BEAT_GEN_HPP_






void beat_gen_init( );
void copy_clap    ( );
void generate_full( Wave& mPlaybackWave, float mBeatsPerMin );

int  bass_drum_detect( short* mData, int mLength );






#endif
