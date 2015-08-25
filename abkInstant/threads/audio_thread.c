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
#include <semaphore.h>
#include "bk_system_defs.h"
//#include "devices.h"
#include "thread_control.h"
#include "audio_thread.h"
#include "utilities.h"
#include "audio_memory.h"
#include "AUDIO_device.h"


#define MAX_USERS     10
#define MAX_LISTENERS 12

#define AUDIO_BUFFER_SIZE 16384
static char	 buffer[AUDIO_BUFFER_SIZE];		// 4 byte token + 4 byte length
static int 	 bytes_rxd=0;

static BOOL 	save_requested = FALSE;
static BOOL 	play_requested = FALSE;
static BOOL 	play_started   = FALSE;
static BOOL 	send_requested = FALSE;
BOOL			audio_terminate_requested = FALSE;
static int   	port	= 6000;

static struct   sockaddr_in s_in;
static struct   sockaddr_in p_in;	// To be stored in UserList for each user.
static fd_set	socks;
static int 	 	listenfd = 0, connfd = 0;


byte		 	audio_socket_buffer[AUDIO_OUTPUT_BUFFER_SIZE+100];
BOOL 			start_new_file = TRUE;

struct wav_header wave_header;		// first packet.

/******************************************************
* Process 
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
BOOL  header_received = FALSE;
char* header_position = (char*)&wave_header;	// start at the top

BOOL handle_audio_data( )
{
	//printf("Inside handle_audio_data()\n");
	BOOL retval = TRUE;
	if (header_received==FALSE)
	{
		// first packet sent over should be the AUDIO_header:
				// for some reason sizeof below was giving 2 bytes extra!
		long int header_length = 38; //sizeof(struct wav_header);
		//printf("receving header...Expecting %d bytes\n", header_length); 
		memcpy( (char*)header_position, buffer, bytes_rxd ); 
		header_position += bytes_rxd;
		char* end_of_header = (char*)(&wave_header);
		end_of_header += header_length;		// points to 1 after the end!
		if ( header_position > end_of_header ) 	// need to wait for more data.
		{
			header_received = TRUE;
			// shift left overs:
			int shift = (header_position-end_of_header);
			memcpy(buffer, buffer+shift, bytes_rxd-shift);
			print_audio_header( &wave_header );
			
		} else if ( header_position == end_of_header ) 	// need to wait for more data.
		{
			// shift left overs:
			print_audio_header( &wave_header );			
			header_received = TRUE;
			return TRUE;
		} else {
			print_audio_header( &wave_header );
			return TRUE;
		}
	}

	if (save_requested)
	{
		// WRITE AUDIO DATA TO FILE:
		if (start_new_file)
		{
			char* fn = GetNewAudioFileName( FALSE );
			printf("saving audio data as %s...\n", fn);	
			OpenAudioFile( fn );
			start_new_file = FALSE;
		} else {
			AppendAudioData( buffer, bytes_rxd );
		}
	}
	
	// Put into audio_memory (for avisual or other app)
	if (ipc_memory_aud != NULL)
	{
		printf(".");	
		ipc_write_buffer( (short*)buffer, bytes_rxd>>1 );
	}

	if (play_requested)
	{
		int audio_dest = 0;
		if ((header_received) && (play_started==FALSE))		
		{
			audio_setup_and_play(   audio_dest, 
									wave_header.sample_rate, 
									wave_header.num_channels, 
									wave_header.bits_per_sample );
			printf("Audio Setup completed\n");
			play_started = TRUE;
		}
		else if (play_started==TRUE)
		{
			printf("Audio sending buffer.\n");
			audio_add_play_buffer( (short*)buffer, bytes_rxd, wave_header.sample_rate );			
		}
	}	
	return retval;
}

/* It's possible to also send audio to other end with the same socket. */
void SendAudioTelegram( BYTE* mBuffer, int mSize)
{
	//printf("Sending: ");
	write(connfd, mBuffer, mSize ); 
}

static void parse_thread_params(char* mMsg)
{
	printf("Audio thread parse_thread_params:%s|\n", mMsg );
	char* delim = strchr(mMsg, ':');
	if (delim) *delim = 0;
	if (strcmp(mMsg, "play") == 0)
	{
		play_requested = TRUE;
		// init audio play
		// not right here because, don't have the header info yet.		
	}
	*delim = ':';		// restore
	
	char* delim2 = strchr(delim+1, ':');
	if (delim2) *delim2 = 0;
	if (strcmp(delim+1, "save") == 0)
	{
		save_requested=TRUE;	// init audio save
	}
	*delim2 = ':';	
	port 	     = atoi( delim2+1 );
}

/******************************************************
* main
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
void* audio_server_thread(void* msg)
{
    int  result = 0;
    int  errsv = 0;
    int  readsocks = 0;					/* number of updated sockets */
    struct sockaddr_in serv_addr;		
    struct sockaddr_in client_addr;		// will receive the clients IP on accept()

    parse_thread_params( (char*)msg );
	init_server		   ( 	 		);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
	memset(&client_addr, '0', sizeof(client_addr));

    serv_addr.sin_family 		= AF_INET;
    serv_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
    serv_addr.sin_port 			= htons(port);

	int optval=1;
	//result = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int) );
	errsv = errno;
	if (result == -1)
    {
    	errno = errsv;
    	perror("setsockopt(SO_REUSEADDR)");
    }
    struct linger mylinger;
    mylinger.l_onoff = TRUE;
    mylinger.l_linger = 2;    
	result = setsockopt(listenfd, SOL_SOCKET, SO_LINGER,    &mylinger, sizeof(mylinger) );	
	errsv = errno;
	if (result == -1)
    {
    	errno = errsv;
    	perror("setsockopt(SO_LINGER)");
    }
    result = bind  (listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    errsv = errno;
    if (result == -1)
    {
    	errno = errsv;
    	perror("Bind");
    }
    result = listen(listenfd, 1);
    if (result == -1)
    {
    	errno = errsv;
    	perror("Listen");
    }
    
	char* ip_addr = inet_ntoa( serv_addr.sin_addr );
	printf( "\n BK Audio thread: waiting for connection %s on Port=%d\n\n", ip_addr, port );

	/* wait for a client to talk to us */
    BOOL done = FALSE;

	while (1) 
	{
    	// WAIT FOR A NEW SESSION : 
    	socklen_t size = (socklen_t)sizeof(client_addr);    	
        connfd    = accept( listenfd, (struct sockaddr*)&client_addr, &size );
		printf("=== Audio Socket opened ===\n");

		// REPEAT UNTIL REQ_SHUT_DOWN is encountered:
		while (!done)
		{
			/* Get HEADER from the client */
			bytes_rxd = read(connfd, buffer, AUDIO_BUFFER_SIZE);
			int errsv = errno;

			if (bytes_rxd == 0)
			{
				printf("Socket Close: bytes_rxd=0;\n");
				//close(connfd);
				//perror("close connfd");
				done = TRUE;
			}
			else if (bytes_rxd == -1) 
			{
				printf("Audio Thread Error: bytes_rxd=-1 Errno=%d\n", errsv);
				errno = errsv;
				perror("recv");
				done = TRUE;
			}
			else 	// DATA ARRIVED, HANDLE:
			{
				// Either save or play or both:
				handle_audio_data();
				// This ram variable will be modified by main thread.
				//if (audio_terminate_requested)
					//done = TRUE;
			}
		}
		done = FALSE;
		close(connfd);
	}
		// SEND Timestamp:	
	    //time_t ticks;
        //ticks = time(NULL);
        //snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        //write(connfd, sendBuff, strlen(sendBuff)); 

        close(connfd);
		perror("close connfd");
        //sleep(0.25);
		printf(" Terminated audio connection.\n");
}



    //char sendBuff[1025];
    //memset(sendBuff,   '0', sizeof(sendBuff));
