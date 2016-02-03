
#ifdef  __cplusplus
extern "C" {
#endif

#define UINT unsigned int
#define WORD unsigned short
#define BYTE unsigned char
//#define BOOL unsigned char

extern BYTE  REQUEST_client_connect_to_robot;
extern char* REQUESTED_client_ip;
extern int   connection_established;

//void init_server();
void Print_General_Msg_Acknowledgement(UINT mToken);
void Print_Msg_Acknowledgement(UINT mToken);
BOOL Process_GeneralPurpose_Telegram( UINT  mToken, char* mMessage, int DataLength, int mconnfd );
BOOL Process_Telegram( UINT  mToken, char* mMessage, WORD mDataLength, int mconnfd );

void SendTelegram( BYTE* mBuffer, int mSize); 

void update_ipc_status				( struct sockaddr_in* sa );
void update_ipc_status_no_connection( );


void* server_thread(void*);



void transmit_queued_entities();


extern BOOL 	nlp_reply_formulated;
extern char		NLP_Response[255];
extern char 	broadcast_addr[16];	

#ifdef  __cplusplus
}
#endif


