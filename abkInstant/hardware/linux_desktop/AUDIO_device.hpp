/*			RASPBERRY PI AUDIO_device.c 		

// Audio output demo using OpenMAX IL though the ilcient helper library
*/
#ifndef _AUDIO_DEVICE_H_
#define _AUDIO_DEVICE_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define uint32_t unsigned long

//#include "bcm_host.h"
//#include "ilclient.h"

struct WAVE_HEADER 
{
	short 	format;				// PCM=1
	short 	channels_rxd;	
	int 	sample_rate;	
	int		byte_rate;	
	short 	block_align;		
	short 	bits_per_sample;		
};

#define BUFFER_SIZE_SAMPLES 1024

typedef int int32_t;

typedef struct {
   uint32_t 			num_buffers;
   uint32_t 			bytes_per_sample;
} AUDIOPLAY_STATE_T;

int32_t audioplay_create(AUDIOPLAY_STATE_T **handle,
                         uint32_t sample_rate,
                         uint32_t num_channels,
                         uint32_t bit_depth,
                         uint32_t num_buffers,
                         uint32_t buffer_size);

int32_t audioplay_delete(AUDIOPLAY_STATE_T *st);

uint8_t *audioplay_get_buffer(AUDIOPLAY_STATE_T *st);

int32_t audioplay_play_buffer(AUDIOPLAY_STATE_T *st,
                              uint8_t *buffer,
                              uint32_t length);
int32_t audioplay_set_dest(AUDIOPLAY_STATE_T *st, const char *name);

uint32_t audioplay_get_latency(AUDIOPLAY_STATE_T *st);

#define CTTW_SLEEP_TIME 10
#define MIN_LATENCY_TIME 20

//static const char *audio_dest[] = {"local", "hdmi"};

void test_function(int id);
//void play_waveform( Wave* mWave, int dest );
//void play_api_test(int samplerate, int bitdepth, int nchannels, int dest);


// BK:
int32_t audio_setup ( int dest, int samplerate, int channels, int mBufferSamples );
uint8_t* audio_add_play_buffer( short* mBuffer, int length, int samplerate );
void	 audio_close		  ( );

#ifdef  __cplusplus
}
#endif

#endif
