#ifdef  __cplusplus
extern "C" {
#endif


extern BOOL  CAMERA_tcpip_WatchingOn;		
extern BOOL  CAMERA_tcpip_SendingOn;
extern BOOL  CAMERA_tcpip_SendingMuted;		// we send zerod out CAMERA
extern BOOL  CAMERA_tcpip_ListeningSilenced;		// we do not play any incoming CAMERA.
extern BOOL  CAMERA_save_requested;			// incoming or outgoing? 
extern FILE* sending_camera_playback_file_fd;		// prerecorded cam (robot's history)


void Init_Camera_Protocol();
int Parse_Camera_Statement( const char* mSentence, ServerHandler* mh );
//BOOL Perform_Camera_actions( char* mAction, char* mAdjectives, char* mObjects );


/* Action Initiators */
void send_camera_file ( char* mFilename );
void send_camera();
void camera_watch(BOOL Save=FALSE);
void camera_two_way();
void camera_cancel();



#ifdef  __cplusplus
}
#endif

