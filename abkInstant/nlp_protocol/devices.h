#ifdef  __cplusplus
extern "C" {
#endif


struct DEVICE_CAPS {
	int Recordable_Channels ;
	int Playable_Channels   ;
	int Max_SampleRate 	    ;
};

struct DEVICE_CAPS* DEVICE_ComposeAudioCapabilitiesStructure( );

/*struct WAVE_HEADER 
{
	short 	format;				// PCM=1
	short 	channels_rxd;	
	int 	sample_rate;	
	int		byte_rate;	
	short 	block_align;		
	short 	bits_per_sample;		
};*/

void AUDIO_init_hdr( byte mChannels, short mSampleRate );


// include protocol.h before this file to pick up BOOL definition
extern struct WAVE_HEADER audio_hdr;
extern BOOL audio_hardware_enabled;
extern BOOL RecordStarted;
extern BOOL RemoteListener;

struct DEVICE_CAPS* DEVICE_ComposeAudioCapabilitiesStructure( );
#define AUDIO_BUFF_SIZES 20000

void AUDIO_SetupPlayer( byte mChannels, short mSampleRate );
void AUDIO_StartPlay();
BOOL AUDIO_QueueBuffer( char* mBuffer, int mSize );
void AUDIO_ClosePlayer();

extern BOOL   SaveFileOpen;
void AUDIO_SetupRecorder(int mSampleRate);
void AUDIO_StartRecord();
void AUDIO_CloseRecorder();

void fillBuffer( short* mBuffer, int mNumSamps, double freq );  // make sin tone.

//void AUDIO_init_hdr( AudioStreamBasicDescription* fmt );

#ifdef  __cplusplus
}
#endif
