#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <time.h> 
#include "bk_system_defs.h"
#include "protocol.h"
#include "devices.h"
#include "package_commands.h"
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include "udp_transponder.h"
#include "serverthread.h"

#define MAX_USERS     100
#define MAX_LISTENERS 12



char	 header[8];		// 4 byte token + 4 byte length
BYTE*    buff = NULL;	/* used for incomming dir name, and outgoing data */
int 	 sd;
int		 sd_current;		// To be stored in UserList for each user.
int		 client_list[MAX_USERS];
int		 cc, fromlen, tolen;
int 	 addrlen;
char 	 ip_addr[16]; 			// for user feedback
struct   sockaddr_in s_in;
struct   sockaddr_in p_in;	// To be stored in UserList for each user.
fd_set	 socks;
char	 Error_str[128];
int 	 listenfd = 0, connfd = 0;    

#define OUTPUT_BUFFER_SIZE 65535
BYTE 	oBuff[OUTPUT_BUFFER_SIZE];
void 	exit1() {	while (1==1) {  }; }

/********************************************************************
Init - Initialize all submodules
********************************************************************/
void init_server()
{
	Init_AUDIO();
	Init_GPIO();
	Init_VIDEO();
	Init_CAN_protocol();

    // DISPLAY THE IP ADDRESS OF THIS MACHINE:
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            int result = strcmp(ifa->ifa_name, "en1");
            printf("%s IP Address %s \n", ifa->ifa_name, addressBuffer );
            if (result==0)
            	strcpy ( ip_addr, addressBuffer );
        } else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 	
        } 
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct); 
	
 	// END OF DISPLAY IP ADDRESS
}

/********************************************************************
GENERAL Print_Msg_Acknowledgement()  Handles miscellaneous commands.
********************************************************************/
void Print_General_Msg_Acknowledgement(UINT mToken)
{
	switch (mToken)
	{
		case BK_PING 		:	printf(" BK PING "	 			);			break;
		case BK_KEY_PRESSED	:	printf(" BK KEY PRESSED "		);			break;
		case BK_POINTING_DEV:	printf(" BK POINTING DEVICE (ie MOUSE) ");	break;
		case BK_TEXT_MESSAGE:	printf(" BK TEXT MESSAGE "		);			break;

		case CMD_SHOW_HTML_FILE		:	printf(" CMD SHOW HTML FILE " );		break;
		case CMD_FILE_TRANSFER_START:	printf(" CMD FILE TRANSFER START"	);	break;
		case CMD_FILE_TRANSFER_DATA:	printf(" CMD FILE TRANSFER DATA"	);	break;
		case CMD_FILE_TRANSFER_END:		printf(" CMD FILE TRANSFER END"		);	break;

		case REQ_MESSAGE	:	printf(" REQUEST MESSAGES "		);		break;
		case REQ_STATUS		:	printf(" REQUEST REQUEST "		);		break;
		case REQ_HISTORY	:	printf(" REQUEST HISTORY "		);		break;
		case REQ_SHUT_DOWN	:	printf(" REQUEST REQ_SHUT_DOWN ");		break;
		default: break;
	}
	//printf("\n");
}

/*****************************************************************
MAIN Print_Msg_Acknowledgement().  Calls all sub functions
(ie. for GPIO, AUDIO, IMAGE, VIDEO, etc)
*****************************************************************/
void Print_Msg_Acknowledgement(UINT mToken)
{
	printf("Token=%x : ", mToken);
	// The sub print functions will return with no effect if the Token is not "in their vocabulary".
	Print_General_Msg_Acknowledgement(mToken);
	Print_GPIO_Acknowledgement		 (mToken);
	Print_CAN_Acknowledgement		 (mToken);
	Print_AUDIO_Acknowledgement		 (mToken);
	Print_IMAGE_Acknowledgement		 (mToken);
	Print_VIDEO_Acknowledgement		 (mToken);
	//Print_FILE_Acknowledgement	 (mToken);
}

BOOL Process_GeneralPurpose_Telegram( UINT  mToken, byte* mMessage, int DataLength, int mconnfd )
{
	int i=0;
	short size=0;

	// Mac is Little endian:  LSB first
	switch (mToken)
	{	
		case BK_KEY_PRESSED	:	/* broad cast to system */		break;
		case BK_POINTING_DEV:	/* broad cast to system */		break;				
		case BK_TEXT_MESSAGE: 
			printf("Msg (%d bytes) : %s \n", DataLength, mMessage );
			break;
		case CMD_SHOW_HTML_FILE :
			// The robot may send us html interface to view and respond to a situation
			// This contains the filename of the html (which was already transfered) 
			// and we are to open it into a browser.
			break;
		case CMD_FILE_TRANSFER_START:	
			printf("Msg (%d bytes) : %s \n", DataLength, mMessage );
			// Everything will be under a main "downloads" folder.
			// Create the directory beneath "downloads" if necessary.
			
			// The number of files transferred could be huge in a large system.
			// Use this to transfer HTML files or any other.
			break;
		case CMD_FILE_TRANSFER_DATA:	
			printf("Msg (%d bytes) : %s \n", DataLength, mMessage );
			// Everything will be under a main "downloads" folder.
			// Create the directory beneath "downloads" if necessary.
			
			// The number of files transferred could be huge in a large system.
			// Use this to transfer HTML files or any other.
			break;
		case CMD_FILE_TRANSFER_END:	
			printf("Msg (%d bytes) : %s \n", DataLength, mMessage );
			// Everything will be under a main "downloads" folder.
			// Create the directory beneath "downloads" if necessary.
			
			// The number of files transferred could be huge in a large system.
			// Use this to transfer HTML files or any other.
			break;

		case REQ_MESSAGE	:
			break;
		case REQ_STATUS		:
			break;
		case REQ_HISTORY	:
			break;
		case REQ_SHUT_DOWN	:	return TRUE;
			break;
		default:
			break;
	}
	return FALSE;
	printf("\n");
}
/******************************************************
* Process 
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
BOOL Process_Telegram( UINT  mToken, byte* mMessage, WORD mDataLength, int mconnfd )
{
	printf("Inside Process_Telegram()\n");
	BOOL  retval 	 = FALSE;			// true if handled	
	retval = Process_GeneralPurpose_Telegram( mToken, mMessage, mDataLength, mconnfd );
	if (retval==FALSE)
		retval = Process_GPIO_Telegrams	( mToken, mMessage, mDataLength, mconnfd );
	if (retval==FALSE)
		retval = Process_CAN_Telegrams	( mToken, mMessage, mDataLength, mconnfd );
	if (retval==FALSE)
		retval = Process_AUDIO_Telegrams( mToken, mMessage, mDataLength, mconnfd );
	if (retval==FALSE)
		retval = Process_IMAGE_Telegrams( mToken, mMessage, mDataLength, mconnfd );
	if (retval==FALSE)
		retval = Process_VIDEO_Telegrams( mToken, mMessage, mDataLength, mconnfd );
	return retval;
}

void SendTelegram( BYTE* mBuffer, int mSize)
{
	printf("Sending: ");
	short Token = *((UINT*)mBuffer);
	Print_Msg_Acknowledgement( Token );  printf("\n");
	write(connfd, mBuffer, mSize ); 
}


/******************************************************
* main
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
void* server_thread(void*)
{
    int readsocks = 0;				/* number of updated sockets */
    struct sockaddr_in serv_addr;  
    char sendBuff[1025];
    time_t ticks; 
    
	init_server( );

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(BK_MEDIA_PORT); 

    bind  (listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);
	//char* ip_addr = inet_ntoa( serv_addr.sin_addr );
	printf( "\n Welcome to BK Media Control  :  IP=%s:%d \n\n", ip_addr, BK_MEDIA_PORT );

	/* wait for a client to talk to us */
    BOOL done = FALSE;
    while(1)
    {
    	// WAIT FOR A NEW SESSION:
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		printf("=== Socket opened ===\n");

		// REPEAT UNTIL REQ_SHUT_DOWN is encountered:
		while (!done)
		{
			/* Get HEADER from the client */
			int bytes_rxd = read(connfd, header, 8);
			//DumpBuffer( header, bytes_rxd );			
			
			if (bytes_rxd == 0)
			{
			}
			else if (bytes_rxd == -1) 
			{
				perror("recv");
				exit1();
			}
			else 	// DATA ARRIVED, HANDLE:
			{
				UINT  Token 	 = *((UINT*)header);
				UINT  DataLength = *((UINT*)(header+4));		// Datalength (4 byte)
				printf("hdr bytes=%d Token=%x Datalength=%d\n", bytes_rxd, Token, DataLength );

				// WAIT till we have all the data!
				byte* Message    = new byte[DataLength+1];
				int	  accumulated_bytes = 0;
				while ((accumulated_bytes < DataLength))
				{
					 bytes_rxd = read( connfd, (Message+accumulated_bytes), (DataLength-accumulated_bytes) );
					 DumpBuffer( (BYTE*)(Message+accumulated_bytes), bytes_rxd );			
					 accumulated_bytes += bytes_rxd; 
					 //printf("accumulating bytes rxd: %d/%d allocated\n", accumulated_bytes, mDataLength );
				}
				printf("bytes: %d/%d\n", accumulated_bytes, DataLength );

				printf( "Received: " );
				Print_Msg_Acknowledgement( Token );
				Process_Telegram( Token, Message, DataLength, connfd );
				free(Message);
				done = (Token == REQ_SHUT_DOWN);
			}
			
			//sleep(1);
		}
		done = FALSE;

		// SEND Timestamp:	
        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(connfd, sendBuff, strlen(sendBuff)); 

		printf(" Closing socket.\n");
        close(connfd);
        sleep(0.25);
     }	// WAIT FOR ANOTHER CONNECT     
}


int server_thread_old()
{
/*    int listenfd = 0, connfd = 0;
    int readsocks = 0;				 number of updated sockets 
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    listen(listenfd, 10); 

	 wait for a client to talk to us 

    BOOL done = FALSE;	
    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		printf(" Socket opened.\n");

			while (!done)
			{
				 Get HEADER from the client 
				int bytes_rxd = read(connfd, header, 4);
				if (bytes_rxd == 0) {	// other end closed
					break;
					
				} else if (bytes_rxd == -1) {
					perror("recv");
					close(connfd);
					exit1();
				} else
				done = Process_Telegram( header, 4 );
			}
			if (done)
			{
				done = FALSE;
					ticks = time(NULL);
					snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
					write(connfd, sendBuff, strlen(sendBuff)); 
			}
			printf(" Closing socket.\n");
			close(connfd);
			sleep(0.25);
	 } */
} 

