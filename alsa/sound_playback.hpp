#ifndef _SOUND_PLAYBACK_HPP_


void pan_buffer(short* buff, int buff_size, int mPanPot );	// must be stereo!


int sound_playback ( DSPWave& wav, bool mLoopMode );

int sound_play_main( int argc, char **argv );


#endif
