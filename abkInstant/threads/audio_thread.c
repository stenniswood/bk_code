/**********************************************************
 This thread is intended to receive incoming audio & 
	either [play it | save it] or both 
 This was taken from serverthread.c 
 Listens on Port:  

Tenniswood - 2014
**********************************************************/
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

#include "bk_system_defs.h"
#include "devices.h"
#include "thread_control.h"
#include "audio_thread.h"
#include "utilities.h"


#define MAX_USERS     10
#define MAX_LISTENERS 12

#define AUDIO_BUFFER_SIZE 16384
static char	 buffer[AUDIO_BUFFER_SIZE];		// 4 byte token + 4 byte length
static int 	 bytes_rxd=0;

static BOOL 	save_requested = FALSE;
static BOOL 	play_requested = FALSE;
static BOOL 	send_requested = FALSE;
BOOL	audio_terminate_requested = FALSE;
static int   	port	= 6000;

static struct   sockaddr_in s_in;
static struct   sockaddr_in p_in;	// To be stored in UserList for each user.
static fd_set	 socks;
static int 	 listenfd = 0, connfd = 0;

#define OUTPUT_BUFFER_SIZE 65535
static byte 	oBuff[OUTPUT_BUFFER_SIZE];
BOOL 	start_new_file = TRUE;

/******************************************************
* Process 
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
BOOL handle_audio_data( )
{
	printf("Inside handle_audio_data()\n");
	BOOL retval = TRUE;

	//if (save_requested)
		// WRITE AUDIO DATA TO FILE.
	if (start_new_file)
	{
		char* fn = GetNewAudioFileName( FALSE );
		OpenAudioFile( fn );
		start_new_file = FALSE;
	} else {
		AppendAudioData( buffer, bytes_rxd );
	}

	//if (play_requested)
		// SEND OUT ON CAN CONTROLLER	
	//if (send_requested)
		//
	return retval;
}

/* It's possible to also send audio to other end with the same socket. */
static void SendTelegram( byte* mBuffer, int mSize)
{
	printf("Sending: ");
	write(connfd, mBuffer, mSize ); 
}

static void parse_thread_params(char* mMsg)
{
	printf("parse_thread_params:%s|\n", mMsg );
	char* delim = strchr(mMsg, ':');
	if (delim) *delim = 0;
	if (strcmp(mMsg, "play") == 0)
	{
		// init audio play
	}
	*delim = ':';		// restore
	
	char* delim2 = strchr(delim+1, ':');
	if (delim2) *delim2 = 0;
	if (strcmp(delim+1, "save") == 0)
	{
		// init audio save
	}
	*delim2 = ':';
	
	port 	     = atoi( delim2+1 );
	printf("Recevied Port=%d\n", port );

//	char* delim3 = strchr(delim2+1, ':');
//	if (delim3) *delim3 = 0;
	//if (strcmp(delim2+1, "send") == 0)
	{
		// init microphone and send.	
		// maybe we need another thread b/c synchronizing.
	}
}

/******************************************************
* main
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
void* audio_server_thread(void* msg)
{
    int readsocks = 0;				/* number of updated sockets */
    struct sockaddr_in serv_addr;  
    char sendBuff[1025];
    time_t ticks;     
        
    parse_thread_params( (char*)msg );
	init_server		   ( 	 );

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family 		= AF_INET;
    serv_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
    serv_addr.sin_port 			= htons(port); 

    bind  (listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);
	//char* ip_addr = inet_ntoa( serv_addr.sin_addr );
	printf( "\n BK Audio thread: waiting for connection on Port=%d\n\n", port );

	/* wait for a client to talk to us */
    BOOL done = FALSE;
    while(1)
    {
    	// WAIT FOR A NEW SESSION:
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		printf("=== Audio Socket opened ===\n");

		// REPEAT UNTIL REQ_SHUT_DOWN is encountered:
		while (!done)
		{
			/* Get HEADER from the client */
			bytes_rxd = read(connfd, buffer, 8);
			//DumpBuffer( buffer, bytes_rxd );			

			if (bytes_rxd == 0)
			{
			}
			else if (bytes_rxd == -1) 
			{
				printf("Received -1 byte - error \n");
				perror("recv");
				exit1();
			}
			else 	// DATA ARRIVED, HANDLE:
			{
				buffer[bytes_rxd] = 0;
				printf("hdr bytes=%d \n", bytes_rxd );
				printf("Command: %s \n",  buffer);

				// Either save or play or both:
				handle_audio_data();

				// This ram variable will be modified by main thread.
				if (audio_terminate_requested)
					done = TRUE;
			}
		}
		done = FALSE;

		// SEND Timestamp:	
        ticks = time(NULL);
        //snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        //write(connfd, sendBuff, strlen(sendBuff)); 

		printf(" Terminated audio connection.\n");
        close(connfd);
        sleep(0.25);
     }	// WAIT FOR ANOTHER CONNECT     
}



