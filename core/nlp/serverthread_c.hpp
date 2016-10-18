#ifndef _SERVER_THREAD_H_
#define _SERVER_THREAD_H_


#define UINT unsigned int
#define WORD unsigned short
#define BYTE unsigned char
//#define BOOL unsigned char


/* Ah, this may even hold frames of 1080i video, so... let's make it big: 5MB  */
#define MAX_SENTENCE_LENGTH 5*1024*1024
extern char	 socket_buffer[MAX_SENTENCE_LENGTH];
extern int 	 connfd;    
extern int 	 bytes_rxd;

extern BYTE  REQUEST_client_connect_to_robot;
extern char* REQUESTED_client_ip;
extern int   connection_established;


//void init_server();
void Print_General_Msg_Acknowledgement	(UINT mToken);
void Print_Msg_Acknowledgement			(UINT mToken);
BOOL Process_GeneralPurpose_Telegram	( UINT  mToken, char* mMessage, int DataLength, int mconnfd );
BOOL Process_Telegram					( UINT  mToken, char* mMessage, WORD mDataLength, int mconnfd );
void SendTelegram						( unsigned char* mBuffer, int mSize );

void update_ipc_status				( struct sockaddr_in* sa );
void update_ipc_status_no_connection( );


void* server_thread					( void*);
void  transmit_queued_entities		( );

extern BOOL 	nlp_reply_formulated;
extern char		NLP_Response[255];
extern char 	broadcast_addr[16];	


#endif
