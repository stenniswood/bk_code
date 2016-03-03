/*			RASPBERRY PI AUDIO_device.c 		

// Audio output demo using OpenMAX IL though the ilcient helper library
*/
#ifndef _AUDIO_DEVICE_H_
#define _AUDIO_DEVICE_H_


#include <semaphore.h>
//#include "ilclient.h"

#define BUFFER_SIZE_SAMPLES 1024

void create_sinewave(short* mBuffer, int mSize, float freq, float phase);


struct WAVE_HEADER 
{
	short 	format;				// PCM=1
	short 	channels_rxd;	
	int 	sample_rate;	
	int		byte_rate;	
	short 	block_align;		
	short 	bits_per_sample;		
};

typedef struct {
   sem_t 				sema;
   uint32_t 			num_buffers;
   uint32_t 			bytes_per_sample;
} AUDIOPLAY_STATE_T;


struct DEVICE_CAPS* DEVICE_ComposeAudioCapabilitiesStructure( );    //    data structure only - no hardware interaction
//void callback(void *ptr, AudioQueueRef queue, AudioQueueBufferRef buf_ref);      // Playback callback buffer done playing.
void InitFormatParameters   (  BYTE mChannels, short mSampleRate );    //    data structure only - no hardware interaction
void AUDIO_init_hdr         ( BYTE mChannels, short mSampleRate  );          //    data structure only - no hardware interaction
void AUDIO_SetupPlayer      ( BYTE mChannels, short mSampleRate );
void AUDIO_StartPlay        ();
BOOL AUDIO_QueueBuffer      ( char* mBuffer, int mSize );
void AUDIO_ClosePlayer      ();

/*void record_callback( void *ptr,
                     AudioQueueRef 						rqueue,
                     AudioQueueBufferRef 				buf_ref,
                     const AudioTimeStamp               *inStartTime,
                     UInt32                              inNumberPacketDescriptions,
                     const AudioStreamPacketDescription *inPacketDescs  ); */
void AUDIO_SetupRecorder(int mSampleRate);
void AUDIO_StartRecord();
void AUDIO_CloseRecorder();

void fillBuffer( short* mBuffer, int mNumSamps, double freq );



//static const char *audio_dest[] = {"local", "hdmi"};
//void play_waveform( Wave* mWave, int dest );
//void play_api_test(int samplerate, int bitdepth, int nchannels, int dest);
enum eAudioDirection {
    PLAY,
    RECORD,
    BOTH
};


// BK:
int32_t  audio_setup ( int dest, int samplerate, int channels, int mBuffNumberOfSamples, int mDirection=(int)PLAY );
uint8_t* audio_add_play_buffer( short* mBuffer, int length, int samplerate );
void	 audio_close		  ( );


#endif
