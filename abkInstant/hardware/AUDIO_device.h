/*			RASPBERRY PI AUDIO_device.c 		

// Audio output demo using OpenMAX IL though the ilcient helper library
*/

//#include "bcm_host.h"
//#include "ilclient.h"


#define BUFFER_SIZE_SAMPLES 1024

typedef int int32_t;

typedef struct {
   sem_t sema;
   ILCLIENT_T *client;
   COMPONENT_T *audio_render;
   COMPONENT_T *list[2];
   OMX_BUFFERHEADERTYPE *user_buffer_list; // buffers owned by the client
   uint32_t num_buffers;
   uint32_t bytes_per_sample;
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

static const char *audio_dest[] = {"local", "hdmi"};
void play_api_test(int samplerate, int bitdepth, int nchannels, int dest);
int audio_setup_and_play(int argc, char **argv);

