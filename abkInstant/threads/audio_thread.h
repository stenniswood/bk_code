
#ifdef  __cplusplus
extern "C" {
#endif

#define UINT unsigned int
#define WORD unsigned short
#define BYTE unsigned char

/************************* WAVE HEADER ******************************
typedef struct wav_header 
{
    char 		chunk_id[4];
    int 		chunk_size;
    char 		format[4];
    char 		subchunk1_id[4];
    int 		subchunk1_size;
    short int 	audio_format;
    short int 	num_channels;
    int 		sample_rate;
    int 		byte_rate;
    short int 	block_align;
    short int 	bits_per_sample;
    short int 	extra_param_size;
    char 		subchunk2_id[4];
    int 		subchunk2_size;
};
********************************************************************/
#define AUDIO_OUTPUT_BUFFER_SIZE 65535
extern	BOOL		audio_terminate_requested;
extern byte		 	audio_socket_buffer[AUDIO_OUTPUT_BUFFER_SIZE+100];

void Print_General_Msg_Acknowledgement(UINT mToken);
void Print_Msg_Acknowledgement(UINT mToken);
BOOL Process_GeneralPurpose_Telegram( UINT  mToken, char* mMessage, int DataLength, int mconnfd );
BOOL Process_Telegram( UINT  mToken, char* mMessage, WORD mDataLength, int mconnfd );

void SendAudioTelegram( BYTE* mBuffer, int mSize);

void* audio_server_thread(void* msg);


#ifdef  __cplusplus
}
#endif

