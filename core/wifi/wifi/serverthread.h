
#ifdef  __cplusplus
extern "C" {
#endif

#define UINT unsigned int
#define WORD unsigned short
#define BYTE unsigned char


void init_server();
void Print_General_Msg_Acknowledgement(UINT mToken);
void Print_Msg_Acknowledgement(UINT mToken);
BOOL Process_GeneralPurpose_Telegram( UINT  mToken, char* mMessage, int DataLength, int mconnfd );
BOOL Process_Telegram( UINT  mToken, char* mMessage, WORD mDataLength, int mconnfd );

void* server_thread(void*);




#ifdef  __cplusplus
}
#endif

