/**********************************************************
 This thread is intended to receive incoming hmi &
	either [play it | save it] or both
 This was taken from file_thread.c
 	hmi includes:  dual touch info (2 mouse)
 					keyboard
 
Tenniswood 2014
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
#include <fcntl.h>
#include <vector>
#include <string>
#include "bk_system_defs.h"
//#include "devices.h"
#include "thread_control.h"
#include "file_thread.h"
#include "hmi_thread.h"


// RECEIVE VARIABLES:
#define  	 	FILE_BUFFER_SIZE 			8192
static char	 	buffer[FILE_BUFFER_SIZE];		// 4 byte token + 4 byte length
static int 	 	bytes_rxd 			   	= 0;


// The file size will be a 6 byte number.  Ie. 4gigs * 65535 = tera bytes
BOOL	 		hmi_terminate_requested = FALSE;
static int   	port			  		 = 6000;

static struct   sockaddr_in s_in;
static struct   sockaddr_in p_in;			// To be stored in UserList for each user.
static fd_set	socks;
static int 	 	listenfd = 0, connfd = 0;    


// SEND VARIABLES:
#define OUTPUT_BUFFER_SIZE 65535
static byte 	oBuff[OUTPUT_BUFFER_SIZE];


static void SendTelegram( byte* mBuffer, int mSize)
{
	printf("Sending: ");
	write(connfd, mBuffer, mSize ); 
}


static void dump_buff()
{
	int length = bytes_rxd;
	printf("Bytes Rxd=%d\n", bytes_rxd);
	for (int i=0; i<length; i++)
	{
		if ((i%16)==0)
			printf("\n");
		printf("%2x ", buffer[i] );
	}	
	printf("\n");
} 

/******************************************************
* Process 
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
// What happens if data is split across buffers (ie read() )?

//char tmp_str[255];		// holds file path/name between block reads.
//byte tmp_str_end=0;

BOOL handle_hmi_data( long mOffset, long BytesRemaining )		// starting offset into the buffer.
{
	BOOL    retval 		= TRUE;
	char*   ptr 		= &(buffer[mOffset]);
	char*	f_end = 0;

	return retval;
}

static void parse_thread_params(char* mMsg)
{
	printf("parse_thread_params:%s|\n", mMsg );

	char* delim = strchr(mMsg, ':');
	if (delim==NULL)  return;
	
	*delim = 0;
	if (strcmp(mMsg, "play") == 0)
	{
		// init display 
	}
	*delim = ':';		// restore

	char* delim2 = strchr(delim+1, ':');
	if (delim2==NULL) return;	
	*delim2 = 0;
	if (strcmp(delim+1, "save") == 0)
	{
		// init audio save
	}
	*delim2 = ':';
	

	// EXTRACT - Port
	char* delim3 = strchr(delim2+1, '\n');
	if (delim3) *delim3 = 0;
	port = atoi( delim2+1 );
	printf("Recevied Port=%d\n", port );
	if (delim3==NULL) return;

	///////////////////////////////////////////////////////////
	// Output (File client) side parameters:
	// User may send an entire directory, recursively adding 
	// all sub directories. 

	
	
	
//	char* delim3 = strchr(delim2+1, ':');
//	if (delim3) *delim3 = 0;
//	if (strcmp(delim2+1, "send") == 0)
//	{
//		 init microphone and send.	
//		 maybe we need another thread b/c synchronizing.
//	}
}



/* Consider:  everything is the same.
	variable options specifiy the operation.
	
	But:  sending files means reading a directory content, transferring file,
		and repeating.
		
	Sending CAN messages means, ISR to receive CAN, send packet.  wait for next.
	
	So lp would say, send a header each time.  ie TYPE="CAN";  or TYPE="FILE";
	Advantages:
		Just one thread does everything.
	
	Disadvantages:	Stronger.
		But if steaming live audio, and suddenly a file goes thru... not good at all!
		(a lower priority thread would ensure audio receives.
		
*/
/******************************************************
* thread function
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
static int packet_count=0;

void* hmi_server_thread(void* msg)
{ 
    int readsocks = 0;				/* number of updated sockets */
    struct sockaddr_in serv_addr;  
    time_t ticks; 
    
    parse_thread_params( (char*)msg );
	init_server( );

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
   // memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family 	  = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port 		  = htons(port);

    bind  ( listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen( listenfd, 10);
	printf( "\n BK HMI Transfer Thread  :  IP=%s:%d \n\n", ip_addr, port );

	/* To send & receive files simulataneously, need to set the socket to non-blocking.
	   when not receiving, send, etc.		*/

	/* wait for a client to talk to us */
    BOOL done = FALSE;
    while(1)
    {
    	// WAIT FOR A NEW SESSION:
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		printf("=== HMI Transfer Socket opened ===\n");

		//long int start_time_ticks = time(NULL);
		// REPEAT UNTIL REQ_SHUT_DOWN is encountered:
		while (!done)
		{
			/* We should:
				A) Send the filesize as 4 bytes at start of telegram. 
				b) And compute a checksum on the file.
			    c) Automatically close the file when done.
			    
			Need to think about multiple files:
				How do we separate them?  ie the new filename & path.
				Do they go to the nlp socket, or right here?  right here.
				advantages/disadvantages?
					Let's keep it nlp is things you'd want the user to know about.

			Need to think about Directories:
			*/

			/* Get HEADER from the client */
			printf(".");
			bytes_rxd = read(connfd, buffer, FILE_BUFFER_SIZE);			
			//DumpBuffer( buffer, bytes_rxd )
			packet_count++;
			if ((packet_count%16)==0)	printf("\n");			
			printf(":%d: ", bytes_rxd );
			
			if (bytes_rxd == 0)
			{		
				done = TRUE;	
				//fclose(connfd);
			}
			else if (bytes_rxd == -1) 
			{				
				perror("recv: bytes_rxd");
			}
			else 	// DATA ARRIVED, HANDLE:
			{				
				//printf("hdr bytes=%d Token=%x Datalength=%d\n", bytes_rxd, Token, DataLength );
				handle_hmi_data( 0, bytes_rxd );
				if (hmi_terminate_requested)
					done = TRUE;
			}
			//handle_file_send();
		}
		done = FALSE;

		// SEND Timestamp:	
        //ticks = time(NULL);
        //snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        //write(connfd, sendBuff, strlen(sendBuff)); 

		printf(" Closing socket.\n");
        close(connfd);
        sleep(0.25);
     }	// WAIT FOR ANOTHER CONNECT     
}


