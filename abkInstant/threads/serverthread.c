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
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <list>
#include "protocol.h"
//#include "devices.h"

#include "serverthread.h"
#include "GENERAL_protocol.h"
#include "CAN_protocol.h"
#include "HMI_protocol.h"
#include "GPIO_protocol.h"
#include "FILE_protocol.h"
#include "AUDIO_protocol.h"
#include "CAMERA_protocol.h"
#include "visual_memory.h"
#include "bk_system_defs.h"


#define MAX_SENTENCE_LENGTH 255
char	 		header[MAX_SENTENCE_LENGTH];

static char 	ip_addr[16]; 			// for user feedback
struct   		sockaddr_in s_in;
struct   		sockaddr_in p_in;	// To be stored in UserList for each user.
fd_set	 		socks;
int 	 		listenfd = 0, connfd = 0;    

#define OUTPUT_BUFFER_SIZE 65535
static BYTE 	general_socket_buff[OUTPUT_BUFFER_SIZE];
static void 	exit1() {	while (1==1) {  }; }

int 			bytes_txd;

/********************************************************************
Init - Initialize all submodules
Retrieves the IP address for en1 into file variable : ip_addr
********************************************************************/
static void init_server()
{
    // DISPLAY THE IP ADDRESS OF THIS MACHINE:
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    getifaddrs(&ifAddrStruct);
	// Go thru each (device/connection) IP address (ie. wifi, ethernet - en0, en1, etc.)
    for (ifa = ifAddrStruct; ifa != NULL;  ifa=ifa->ifa_next)
    {
        if (ifa->ifa_addr->sa_family==AF_INET) 	// check it is IP4
        { 	
            // is a valid IP4 Address
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

            int result = strcmp(ifa->ifa_name, "en1");
            printf("%s IP Address %s \n", ifa->ifa_name, addressBuffer );
            if (result==0)
            	strcpy ( ip_addr, addressBuffer );
            	
        } else if (ifa->ifa_addr->sa_family==AF_INET6) 	// check it is IP6
        { 
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 	
        } 
    }
    if (ifAddrStruct!=NULL) 
    	freeifaddrs(ifAddrStruct);
 	// END OF DISPLAY IP ADDRESS
}

void SendTelegram( BYTE* mBuffer, int mSize)
{
	printf("Sending: ");
	short Token = *((UINT*)mBuffer);
	//Print_Msg_Acknowledgement( Token );  printf("\n");
	write(connfd, mBuffer, mSize ); 
}

void update_ipc_status( struct sockaddr_in* sa )
{
	static char msg[80];
	char client_ip[14];	
	strcpy(client_ip, inet_ntoa( sa->sin_addr ) );
	strcpy(msg, "Connected to : ");
	strcat(msg, client_ip);		
	ipc_write_connection_status( msg );
}		

BOOL Parse_done( char* mSentence ) 
{
/* They might say:
	close this connection
	go back to doing whatever you were.
	disconnect [us].
	... after 1 hour.  after the transfer is complete. when noone is present for a while.
*/
	std::string* verb = extract_verb  ( mSentence );
	sObject* object   = extract_object( mSentence );	// direct object of sentence.
	if (object==NULL) return FALSE;

	if ((strcmp(verb->c_str(), "close") ==0)	||	
		(strcmp(verb->c_str(), "disconnect") ==0))				
	{
		if ((strcmp(object->name.c_str(), "this connection") ==0)	||
		    (strcmp(object->name.c_str(), "us") ==0))
		{
			printf("Parser is closing!\n");
			return TRUE;
		}
	}
	return FALSE;
}

// Send nlp response:
void Send_Reply()
{
	printf("RESPONSE:|%s|\n", NLP_Response);

	int length = strlen(NLP_Response);
	if (length > MAX_SENTENCE_LENGTH)
		length = MAX_SENTENCE_LENGTH;
	bytes_txd = write(connfd, NLP_Response, length );
}

void Init_NLP_word_lists()
{
	Init_General_Protocol	();
	Init_HMI_Protocol		();
	Init_FILE_Protocol		();
	Init_CAN_Protocol		();	
	Init_Camera_Protocol	();	
	Init_Audio_Protocol		();	
}


/******************************************************
* Could rename as nlp_server_thread() 
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
void* server_thread(void*)
{
    int 			   readsocks = 0;			/* number of updated sockets */
    struct sockaddr_in serv_addr;   
    struct sockaddr_in client_addr;		// will receive the clients IP on accept()

    char 	sendBuff[1025];
    time_t 	ticks; 

	init_server    ( );			// Get "ip_addr" of this machine.
	Init_NLP_word_lists( );		// parser

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff,   '0', sizeof(sendBuff) );

    serv_addr.sin_family 		= AF_INET;
    serv_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
    serv_addr.sin_port 			= htons(BK_MEDIA_PORT);

    bind  (listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);

	char* ip_addr = inet_ntoa( serv_addr.sin_addr );
	printf( "\n Welcome to BK Media Control  :  IP=%s:%d \n\n", ip_addr, BK_MEDIA_PORT );

	/* wait for a client to talk to us */
    BOOL done = FALSE;
    while(1)
    {
    	// WAIT FOR A NEW SESSION:
    	socklen_t size = (socklen_t)sizeof(client_addr);
        connfd = accept(listenfd, (struct sockaddr*)&client_addr, &size ); 
		printf("=== Socket opened ===\n");
		update_ipc_status( &client_addr ); 
		
		// REPEAT UNTIL REQ_SHUT_DOWN is encountered:
		while (!done)
		{
			/* Get HEADER from the client */
			int bytes_rxd = read(connfd, header, MAX_SENTENCE_LENGTH);
			int errsv = errno;

			if (bytes_rxd == 0)
			{ 
				printf("Server thread Received Close (0 bytes) \n");
				done = TRUE;
			}
			else if (bytes_rxd == -1) 
			{
				printf("NLP Server thread error: Received -1 bytes; errno=%d\n", errno);
				errno = errsv;
				perror("NLP socket");
				exit1();
			}
			else 	// DATA ARRIVED, HANDLE:
			{
				header[bytes_rxd] = 0;
				UINT  DataLength = strlen(header);

				Parse_Statement(header);
				printf("Done parsing.\n");
				
				if (nlp_reply_formulated)				
					Send_Reply();
				printf("Reply sent.\n");	

				ipc_write_command_text( header );
				//done = Parse_done(header);
			}
		}
		done = FALSE;

		// SEND Timestamp:	
        ticks = time(NULL);
		strcpy(NLP_Response, "Closing this connection.");
        Send_Reply( );
		printf( " %s\n", NLP_Response );

        close(connfd);
        sleep(0.25);
     }	// WAIT FOR ANOTHER CONNECT     
}



/*	int i=0;
	short size=0;

	// Mac is Little endian:  LSB first
	switch (mToken)
	{	
		case BK_KEY_PRESSED	:	// broad cast to system 
				break;
		case BK_POINTING_DEV:	// broad cast to system 		
				break;				
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
	}*/

/*	printf("Inside Process_Telegram()\n");
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
	return retval;	*/
/*	switch (mToken)
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
	} */
