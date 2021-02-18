#ifndef _ALSA_RECORD_HPP_
#define _ALSA_RECORD_HPP_




extern Wave recorded;

void init_hw_record();

void* record( void* argp );

void clean_up();


void* record_thread_func( void* argp );



#endif
