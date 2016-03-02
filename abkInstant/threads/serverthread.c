/*  WARNING: This serverthread.c is Very different from the one in abkInstant
	The one in ../core/wifi/serverthread.c is Token based
	This one is simple NLP words based and is the current choice.
*/
#include <sys/select.h>
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
#include <sys/ioctl.h>
#include "bk_system_defs.h"
#include "CAN_base.h"
#include "CAN_memory.h"
#include "CAN_util.h"
#include "AUDIO_interface.h"
#include "protocol.h"

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
#include "client_memory.hpp"

 
char	 		socket_buffer[MAX_SENTENCE_LENGTH];
char 			broadcast_addr[16];		// 
static char 	ip_addr[16]; 			// for user feedback
int 	 		listenfd = 0, connfd = 0;    
int 			bytes_txd;
int 			bytes_rxd=0;

byte  REQUEST_client_connect_to_robot = FALSE;
char* REQUESTED_client_ip    = NULL;
int   connection_established = FALSE;

// USED IN SERVER THREAD:
struct sockaddr_in serv_addr;   
struct sockaddr_in client_addr;		// will receive the clients IP on accept()

static void 	exit1() {	while (1==1) {  }; }

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

            printf("%s IP Address %s \n", ifa->ifa_name, addressBuffer );
            int result = strcmp(ifa->ifa_name, "en1");
            if (result==0) 
            {
        		struct sockaddr * sa2 = ifa->ifa_broadaddr;
            	strcpy ( ip_addr, addressBuffer );
            	            	
            	printf(" Broadcast Address = %s\n", ifa->ifa_broadaddr->sa_data );            	      	
            	// inet_ntop(AF_INET, tmpAddrPtr, broadcast_addr, INET_ADDRSTRLEN);
            	// strcpy ( broadcast_addr, addressBuffer );            	
            }
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
	//printf("Sending: ");
	write(connfd, mBuffer, mSize ); 
}

void update_ipc_status( struct sockaddr_in* sa )
{
	static char msg[80];
	char client_ip[14];	
	strcpy(client_ip, inet_ntoa( sa->sin_addr ) );
	strcpy(msg, "Connected to : ");
	strcat(msg, client_ip);		
	cli_ipc_write_connection_status( msg );
}		

void update_ipc_status_no_connection( )
{
	static char msg[80];
	strcpy(msg, " Not Connected!");
	cli_ipc_write_connection_status( msg );
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
	char buffer[255];
	strcpy (buffer, "VR:");
	strcat (buffer, NLP_Response);	
	printf("RESPONSE:|%s|\t", buffer);
	
	int length = strlen(buffer);
	if (length > MAX_SENTENCE_LENGTH)
		length = MAX_SENTENCE_LENGTH;
	bytes_txd = write(connfd, buffer, length );
	printf("Reply sent.\n");	
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


/* Return 1 => Error
		  0 => Okay */
int connect_to_robot(char *ip_address )
{
	printf("====Connect_to_robot...ip=%s\n", ip_address);    
    if((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&client_addr, '0', sizeof(client_addr)); 
    client_addr.sin_family = AF_INET;
    client_addr.sin_port   = htons(BK_MEDIA_PORT);

    if(inet_pton(AF_INET, ip_address, &client_addr.sin_addr)<=0)
    {
        printf("\n inet_aton error occured\n");
        return 1;
    } 

	printf("ROBOT CLIENT Connecting to : %s:%d\n", ip_address, BK_MEDIA_PORT );
    if( connect(connfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 
	printf("ROBOT CLIENT Connected\n");
	
   return 0;
}

fd_set rfds;
	
void establish_connection()
{
	/****************************
	   loop on select() or poll().  These will inform when a connection is pending,
	   then we call accept().  Since select()/Poll() are nonblocking, we can
	   also check for connect_to_robot() client initiated request.
	*/
	printf("establish_connection()\n");	
    socklen_t size = (socklen_t)sizeof(client_addr);	
    memset(&client_addr, '0', sizeof(socklen_t) );
	
	struct timeval tv;
	tv.tv_sec = 10;		// want immediate return
	tv.tv_usec = 0;

	int connection_pending = 0;
	//int max_fds = max( listenfd, connfd )+1;
	int max_fds = listenfd+1;

	while (!connection_established)
	{
		FD_ZERO(&rfds);		
		FD_SET(listenfd, &rfds);

		tv.tv_sec  = 0;		// want immediate return
		tv.tv_usec = 0;		
		
		// CHECK FOR AN INCOMING CONNECTION TO ACCEPT:
		int number_active = select( max_fds, &rfds, NULL, NULL, &tv );
		if (number_active==-1)
			printf("%s \n", strerror(errno));

		connection_pending = FD_ISSET(listenfd, &rfds);
		if (number_active>0)
			printf("select() = %d; pending=%d\n", number_active, connection_pending);

		if (connection_pending) {
			connfd = accept(listenfd, (struct sockaddr*)&client_addr, &size ); 
			printf("connection accepted!\n");
			connection_established = TRUE;

				// For Client Status purpose, let them know we are connected.
				//strcpy (NLP_Response, "Connected, welcome.");
				//printf("RESPONSE: %s\n", NLP_Response );
				//Send_Reply();
		}
		else 
			// ALTERNATELY:  Client memory requested we be the client, initiate connection : 
			if (REQUEST_client_connect_to_robot)
			{
				connection_established = !(connect_to_robot( REQUESTED_client_ip ));
				REQUEST_client_connect_to_robot = FALSE;
				
			}
	}
	printf("Connection established \n");
}

/******************************************************
* Could rename as nlp_server_thread() 
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
void* server_thread(void*)
{
    time_t 	ticks; 
	init_server        ( );		// Get "ip_addr" of this machine.
	Init_NLP_word_lists( );		// parser

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family 		= AF_INET;
    serv_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
    serv_addr.sin_port 			= htons(BK_MEDIA_PORT);

    bind  (listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);

	char* ip_addr = inet_ntoa( serv_addr.sin_addr );
	printf( "\n Welcome to BK Media Control  :  IP=%s:%d \n\n", ip_addr, BK_MEDIA_PORT );

	/* wait for a client to talk to us */
    BOOL done = FALSE;
    int  bytes_available 		 = 0;
 	long int telegram_end_marker = 0;	// a position within the buffer[] 

    while(1)
    {
    	// WAIT FOR A NEW SESSION:
		update_ipc_status_no_connection();
    	establish_connection();			// either incoming connection or signal client to robot request.
		printf("=== Socket opened ===\n");
		update_ipc_status( &client_addr ); 
		
		// REPEAT UNTIL REQ_SHUT_DOWN is encountered:
		while (!done)
		{
			ioctl(connfd, FIONREAD, &bytes_available);
			if (bytes_available==0) {
				transmit_queued_entities();	// any and all outgoing data!
				continue;		 // top of loop.
			}
			//printf("bytes_available=%d\n", bytes_available);
			
			/* Get data from the client. For streaming data, remember multiple 'telegrams'
				may come in a single read.  
			 */
			bytes_rxd = read(connfd, socket_buffer, bytes_available);
			int errsv = errno;
			if (bytes_rxd == 0)
			{
				printf("Server thread Received Close (0 bytes) \n");
				done = TRUE;
			}
			else if (bytes_rxd == -1) 
			{
				//printf("NLP Server thread error: Received -1 bytes; errno=%d:%s\n", errno, strerror(errno) );
				perror("NLP Server thread error: Received -1 bytes;");
				exit1();
			}
			else 	// DATA ARRIVED, HANDLE:
			{
				socket_buffer[bytes_rxd]       = 0;
				char* next_telegram_ptr = socket_buffer;
				
				while ( (next_telegram_ptr-socket_buffer) < bytes_rxd )
				{
					int buff_index = (next_telegram_ptr-socket_buffer);
					if (buff_index > 10) 
					{
						for (int c=buff_index-3; c<buff_index+26; c++)
							if (socket_buffer[c]==0)
								printf("|");
							else
								printf("%c", socket_buffer[c]);
						printf( ":  NextString = %s\n", next_telegram_ptr );
					}
					printf(" Start parsing. buff_index=%d of bytes_rxd=%d; \n", buff_index, bytes_rxd);
					next_telegram_ptr   = Parse_Statement( next_telegram_ptr );					
					// next the problem of split packages - which will occur! 
				}
				//printf("Done parsing. %d\n", connfd);

				if (nlp_reply_formulated)
				{	
					Send_Reply();
					nlp_reply_formulated = false;
				}
				printf("\n");
				//ipc_write_command_text( socket_buffer );
				//done = Parse_done(socket_buffer);
			}
		}  // end while()		
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

void video_interface()
{

}
void sequence_interface()
{

}


// Any and all outgoing data!
void transmit_queued_entities()
{
	can_interface();				// Send CAN data if waiting...
	audio_interface();				// Send audio if enabled and data avail
	video_interface();				// Send audio if enabled and data avail
	sequence_interface();			// Send audio if enabled and data avail
	
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
