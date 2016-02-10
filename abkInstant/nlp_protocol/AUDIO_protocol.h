#ifdef  __cplusplus
extern "C" {
#endif



extern BOOL  AUDIO_tcpip_ListeningOn;
extern BOOL  AUDIO_tcpip_SendingOn;
extern BOOL  AUDIO_tcpip_SendingMuted;			// we send zerod out audio
extern BOOL  AUDIO_tcpip_ListeningSilenced;		// we do not play any incoming audio.
extern BOOL  AUDIO_save_requested;

extern FILE* sending_audio_file_fd;


/* Action Initiators */
void send_audio_file( char* mFilename );
void send_audio		( );
void audio_listen	(BOOL Save=FALSE);
void audio_two_way	( );
void audio_cancel	( );


void 	DumpBuffer( BYTE* mbuff, int mlength);
void 	Init_Audio_Protocol();
BOOL 	Parse_Audio_Statement( char* mSentence );



#ifdef  __cplusplus
}
#endif

