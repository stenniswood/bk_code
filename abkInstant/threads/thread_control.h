
#ifdef  __cplusplus
extern "C" {
#endif


#define BASE_NLP_PORT 	5000
#define BASE_AUDIO_PORT 5010
#define BASE_VIDEO_PORT 5020
#define BASE_CAN_PORT   5030
#define BASE_HMI_PORT   5040
#define BASE_FILE_PORT  5050


void create_audio_thread   ( BOOL mPlay, BOOL mSave, int Port = BASE_AUDIO_PORT );
void terminate_audio_thread( 					 );

void create_video_thread	( BOOL mPlay, BOOL mSave, int Port = BASE_VIDEO_PORT );
void terminate_video_thread ( 					  );

void create_CAN_thread	  	( BOOL mPlay, BOOL mSave, int Port = BASE_CAN_PORT );
void terminate_CAN_thread 	( 					  );

void create_HMI_thread	  	( BOOL mPlay, BOOL mSave, int Port = BASE_HMI_PORT );
void terminate_HMI_thread 	( 					  );

void create_file_thread	  	( BOOL mPlay, BOOL mSave, int Port = BASE_FILE_PORT );
void terminate_file_thread	( 					  );

// CLIENT THREADS:
void create_file_tx_thread	 ( char* mDest_IP, char* mLocalPath, char* mFilename, int Port=BASE_FILE_PORT );
void terminate_file_tx_thread( 					  );

void create_audio_tx_thread	   ( char* mHeader, 		int Port = BASE_AUDIO_PORT );
void terminate_audio_tx_thread ( );

void create_video_tx_thread	   ( BOOL mPlay, BOOL mSave, int Port = BASE_VIDEO_PORT );
void terminate_video_tx_thread ( 					  );

void create_CAN_tx_thread	  	( BOOL mPlay, BOOL mSave, int Port = BASE_CAN_PORT );
void terminate_CAN_tx_thread 	( 					  );

void create_HMI_tx_thread	  	( BOOL mPlay, BOOL mSave, int Port = BASE_HMI_PORT );
void terminate_HMI_tx_thread 	( 					  );


void init_server();
extern  char 	 ip_addr[16]; 
void 	exit1();


#ifdef  __cplusplus
}
#endif

