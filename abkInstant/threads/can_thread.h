

/**************************************************************

		FILE IS DEPRECATED!!  ALL DONE IN SERVERTHREAD.C NOW.
		SINGLE THREAD MULTI-DATA.  SINGLE SOCKET, MULTI DATA.

***************************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

#define UINT unsigned int
#define WORD unsigned short
#define BYTE unsigned char

extern BOOL	can_terminate_requested;

void Print_General_Msg_Acknowledgement(UINT mToken);
void Print_Msg_Acknowledgement(UINT mToken);
BOOL Process_GeneralPurpose_Telegram( UINT  mToken, char* mMessage, int DataLength, int mconnfd );
BOOL Process_Telegram( UINT  mToken, char* mMessage, WORD mDataLength, int mconnfd );

void* CAN_server_thread(void*);




#ifdef  __cplusplus
}
#endif

