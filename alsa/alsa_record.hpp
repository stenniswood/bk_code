#ifndef _ALSA_RECORD_HPP_
#define _ALSA_RECORD_HPP_




extern bool 	stop_recording;

extern DSPWave recorded;

int   init_hw_record();
void* record( void* argp );
void  clean_up();


void  process_window( short* mSamps, int mLength, size_t mSampleIndex );
void* record_thread_func( void* argp );



#endif
