#ifdef  __cplusplus
extern "C" {
#endif

#define IPC_AUDIO_KEY 1235

extern char* 	audio_shared_memory;
extern int 		audio_segment_id;
extern struct   audio_ipc_memory_map* ipc_memory_aud;

#define MAX_CLIENT_ARRAY_SIZE 2048
typedef short sample[2];		// stereo
#define ONE_SECOND  44180

char* get_audio_text    ( struct wav_header* Wave_hdr );
void  print_audio_header( struct wav_header* Wave_hdr );

/************************** WAVE HEADER ************************
Size of this struct is:  (for some reason sizeof() gives +2 bytes)
3x4 = 12
5x2 = 10 shorts
4x4 = 16 longs
= 38 *.
*/
typedef struct wav_header 
{
    char 		chunk_id[4];
    long int 	chunk_size;			// sjt added long 
    char 		format[4];
    char 		subchunk1_id[4];
    long int 	subchunk1_size;		// sjt added long 
    short int 	audio_format;
    short int 	num_channels;
    long int 	sample_rate;		// sjt added long
    long int 	byte_rate;
    short int 	block_align;
    short int 	bits_per_sample;
    short int 	extra_param_size;	// used for size of data packets
} wav_header;

/******************** AUDIO MEMORY MAP *****************/
struct audio_ipc_memory_map
{
	long int 		StatusCounter;			//
	char	 		ConnectionStatus[64];	// idle, receiving, connecting.
	long int 		UpdateCounter;			// for each buffer
	long int 		AcknowledgedCounter;	// for each buffer
	long int 		update_samples;			// Number of samples last added.
	char			name[512];				// 
	char			description[512];		//
	struct wav_header audio_header;			// 
	sample	 		audio_data[ONE_SECOND];	// Stereo audio
};

/*********************************************************/
//void dump_ipc				();
void audio_save_segment_id	(char* mFilename);
int  audio_read_segment_id	(char* mFilename);

int  audio_allocate_memory	();
void audio_deallocate_memory();

int  audio_attach_memory	();
void audio_reattach_memory	();
void audio_detach_memory	();

int  audio_get_segment_size	();
void audio_fill_memory		();

void audio_ipc_write_header( struct wav_header mAudio_header  );
void audio_ipc_write_buffer( short* mBuffer, long int mLength );

void audio_ipc_write_audio_connection_status( char* mStatus );
void audio_ipc_write_command_text 		  ( char* mSentence );

BOOL audio_is_new_rxbuffer();
void audio_ack_new_rxbuffer();
void audio_wait_for_ack_new_rxbuffer();

BOOL is_audio_ipc_memory_available();
int audio_connect_shared_memory(char mAllocate);

#ifdef  __cplusplus
}
#endif
