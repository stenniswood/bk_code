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
#include "video_thread.h"


#define MAX_USERS     10
#define MAX_LISTENERS 12


static BOOL 	save_requested = FALSE;
static BOOL 	play_requested = FALSE;
static BOOL	video_terminate_requested = FALSE;
static int   	port	= 6000;

#define  VIDEO_BUFFER_SIZE 65535
static char	 buffer[VIDEO_BUFFER_SIZE];		// 4 byte token + 4 byte length

//char 	 ip_addr[16]; 			// for user feedback
static struct   sockaddr_in s_in;
static struct   sockaddr_in p_in;	// To be stored in UserList for each user.
static fd_set	 socks;
static int 	 listenfd = 0, connfd = 0;    

#define OUTPUT_BUFFER_SIZE 65535
static byte 	oBuff[OUTPUT_BUFFER_SIZE];
//void 	exit1() {	while (1==1) {  }; }

static void SendTelegram( byte* mBuffer, int mSize)
{
	printf("Sending: ");
	write(connfd, mBuffer, mSize ); 
}

/******************************************************
* Process 
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
BOOL handle_video_data( )
{
	printf("Inside handle_video_data()\n");
	BOOL retval = TRUE;
	
	//if (save_requested)
		// WRITE AUDIO DATA TO FILE.
	
	//if (play_requested)
		// SEND OUT ON CAN CONTROLLER
	
	return retval;
}

static void parse_thread_params(char* mMsg)
{
	char* delim = strchr(mMsg, ':');
	*delim = 0;
	char* delim2 = strchr(delim+1, ':');
	*delim2 = 0;

    port = atoi( delim2+1 );
	printf("Recevied Port=%d\n", port );

	if (strcmp(mMsg, "play") == 0)
	{
		// init audio play
	}
	if (strcmp(delim+1, "save") == 0)
	{
		// init audio save
	}
}


/******************************************************
* main
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
void* video_server_thread(void* msg)
{
    int readsocks = 0;				/* number of updated sockets */
    struct sockaddr_in serv_addr;  
    char sendBuff[1025];
    time_t ticks; 
    
    parse_thread_params((char*)msg);
	init_server( );

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port); 

    bind  (listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);
	//char* ip_addr = inet_ntoa( serv_addr.sin_addr );
	printf( "\n BK Video Thread  :  IP=%s:%d \n\n", ip_addr, port );

	/* wait for a client to talk to us */
    BOOL done = FALSE;
    while(1)
    {
    	// WAIT FOR A NEW SESSION:
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		printf("=== Video Socket opened ===\n");

		// REPEAT UNTIL REQ_SHUT_DOWN is encountered:
		while (!done)
		{
			/* Get HEADER from the client */
			int bytes_rxd = read(connfd, buffer, 8);
			//DumpBuffer( buffer, bytes_rxd );			
			
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
				buffer[bytes_rxd] = 0;
				printf("hdr bytes=%d \n", bytes_rxd );

				handle_video_data();
				
				// This ram variable will be modified by main thread:
				if (video_terminate_requested)
					done = TRUE;
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

