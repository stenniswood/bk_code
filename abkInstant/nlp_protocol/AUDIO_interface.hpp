
#include "serverthread.hpp"


void audio_interface( ServerHandler* mh );
void audio_subtract ( short* mSourceData, short* mDestData, int mLength,
					  short mFIR, int mFIRLength );
void MuteBuffer	    ( byte* mData, int mLength   );

BOOL handle_audio_header( int bytes_rxd                          );
BOOL handle_audio_data  ( unsigned char* mAudioData, int mLength );
void send_audio_message ( short* mAudioData, int mLength         );


