/**********************************************************
 This thread is intended to receive incoming audio &
	either [play it | save it] or both
 This was taken from serverthread.c
 
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
#include "devices.h"
#include "thread_control.h"
#include "file_thread.h"



// RECEIVE VARIABLES:
#define  	 	FILE_BUFFER_SIZE 	8192
static char	 	buffer[FILE_BUFFER_SIZE];		// 4 byte token + 4 byte length
static int 	 	bytes_rxd 			= 0;
long int 	 	total_file_bytes_received  		= 0;	// excluding any checksum or header.
long		 	files_transfered 	= 0;
long int 		file_size 		  	= 0;
static FILE* 	fd 					= NULL;
char 		 	FilePath[255];		int FilePathEnd=0;
char 		 	FileName[255];		int FileNameEnd=0;
char 		 	FileSize[16];		int FileSizeEnd=0;
char 		 	LocalPath[] 	= "/home/pi/abkInstant/media/";


// The file size will be a 6 byte number.  Ie. 4gigs * 65535 = tera bytes
BOOL	 		file_terminate_requested = FALSE;
static int   	port			  		 = 6000;

static struct   sockaddr_in s_in;
static struct   sockaddr_in p_in;			// To be stored in UserList for each user.
static fd_set	socks;
static int 	 	listenfd = 0, connfd = 0;    


// SEND VARIABLES:
char 		 	SendPath[255];		int SendPathEnd=0;
char 		 	SendFilename[255];	int SendFilenameEnd=0;
#define OUTPUT_BUFFER_SIZE 65535
static byte 	oBuff[OUTPUT_BUFFER_SIZE];


static void SendTelegram( byte* mBuffer, int mSize)
{
	printf("Sending: ");
	write(connfd, mBuffer, mSize ); 
}

static void save( byte* mdata, long int mLength )
{
	fwrite( mdata, mLength, sizeof(byte), fd );
}

void dump_buff()
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

BOOL handle_file_data( long mOffset, long BytesRemaining )		// starting offset into the buffer.
{
	BOOL    retval 		= TRUE;
	char*   ptr 		= &(buffer[mOffset]);
	static int FileState= BEGIN_OF_NEW_FILE;
	char*	f_end = 0;

	switch( FileState )
	{
	case BEGIN_OF_NEW_FILE:      // GET_FULL_PATHNAME:
		total_file_bytes_received=0;
		printf("\nhandle_file_data: Begin of New File\n");

		// GET THE PATHNAME:
		f_end = strchr(ptr, '\n');
		if (f_end==0) {
			printf("No string terminator found!  Saving %d bytes:\n", BytesRemaining );
			memcpy( FilePath, ptr, BytesRemaining );
			FilePathEnd = BytesRemaining;	//FilePath[BytesRemaining] = 0;
		} else {
			*f_end=0;		f_end++;
			int len = (f_end-ptr);
			memcpy( &(FilePath[FilePathEnd]), ptr, len );
			printf("Filepath=%s\n", FilePath );
			// Setup for next:
			FilePathEnd = 0;
			FileState = GET_FILE_NAME;
			int offset = (f_end-buffer);
			if ((BytesRemaining-len)>0)							
				handle_file_data( offset, (BytesRemaining-len) );	// Recursive call for next item!
		};
		break;
//							  3333333333
//0123456789012345678901234567890123456789
///mnt/sdcard/dcim/100MEDIA/IMAG0150.jpg i  strlen=38 + 1\n = 39

	case GET_FILE_NAME:
		f_end = strchr(ptr, '\n');
		if (f_end==0) {
			printf("No string terminator found!  Saving %d bytes:\n", bytes_rxd );
			memcpy( FileName, ptr, BytesRemaining );
			FileNameEnd = BytesRemaining;						
		} else {
			*f_end=0;			f_end++;
			int len = (f_end-ptr);
			memcpy( &(FileName[FileNameEnd]), ptr, len );
			//FileName[len]=0;
			printf("FileName=%s\n", FileName );
			
			// Setup for next:
			FileNameEnd=0;
			FileState  = GET_FILE_SIZE;
			int offset = (f_end-buffer);
			if ((BytesRemaining-len)>0)							
				handle_file_data( offset, (BytesRemaining-len) );	// Recursive call for next item!
		}
		break;
	case GET_FILE_SIZE:
		f_end = strchr(ptr, '\n');
		if (f_end==0) {
			printf("No string terminator found!  Saving %d bytes:\n", bytes_rxd );
			memcpy( &(FileSize[FileSizeEnd]), ptr, BytesRemaining );
			FileSizeEnd = BytesRemaining;						
		} else { 
			*f_end=0;			f_end++;
			int len = (f_end-ptr);
			memcpy( &(FileSize[FileSizeEnd]), ptr, len );
			FileSize[len]=0;
			//printf("FileName=%s\n", FileSize );
			file_size = atol(ptr);
			printf("File Size=%d\n", file_size );

			// Setup for next:
			FileSizeEnd= 0;
			FileState  = OPEN_FILE;
			int offset = (f_end-buffer);
			if ((BytesRemaining-len)>0)
				handle_file_data( offset, (BytesRemaining-len) );	// Recursive call for next item!
		}
		break;
	case OPEN_FILE:	
		// Extract number of bytes to receive (6 byte number)	
		// From now on, it is file data to be saved:
		strcpy( FilePath, LocalPath );
		strcat( FilePath, FileName  ); 
		fd = fopen( FilePath, "w" );
		FileState  = GET_FILE_BODY;

	case GET_FILE_BODY:	
		// WRITE DATA TO FILE:
		//printf("handle_file_data:%d: %d\t/ %d bytes\n", bytes_rxd, total_file_bytes_received, file_size);
		long int projected_file_rxd = total_file_bytes_received + BytesRemaining;
		if (projected_file_rxd > file_size)
		{
			// error here somewhere!
			// bytes_to_save:
			long bytes_to_save = (file_size - total_file_bytes_received);
			printf("handle_file_data: File Complete + %d bytes\n", (BytesRemaining-bytes_to_save));

			fwrite( ptr, bytes_to_save, 1, fd );
			fclose( fd );
			total_file_bytes_received = file_size;

			// We've reached the end of the file; prepare for next:
			files_transfered++;
			FileState = BEGIN_OF_NEW_FILE;

			// Need to proc rest of buffer 
			BytesRemaining -= bytes_to_save;
			//memcpy( buffer, buffer+bytes_to_save, bytes_rxd );
			handle_file_data( ((ptr+bytes_to_save)-buffer), BytesRemaining);		// recursive call
		}
		else if (projected_file_rxd == file_size)
		{
			fwrite( ptr, BytesRemaining, 1, fd );
			total_file_bytes_received += BytesRemaining;
			fclose( fd );
			
			printf("handle_file_data: File Complete: %d\t/ %d bytes\n", total_file_bytes_received, file_size);

			// We've reached the end of the file; prepare for next:
			files_transfered++;
			FileState = BEGIN_OF_NEW_FILE;
		}
		else if (projected_file_rxd < file_size)
		{
			//printf("handle_file_data: : %d\t/ %d bytes\n",total_file_bytes_received, file_size);
			fwrite( ptr, BytesRemaining, 1, fd );
			total_file_bytes_received += BytesRemaining;
			// Keep going...
		}
		break;
	}
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

	// EXTRACT - Sending Path
	char* sending_path = strchr(delim3+1, '\n');
	if (sending_path==NULL) return;
	
	// WE HAVE FILES TO SEND:	
	*sending_path = 0;
	strcpy( SendPath, delim3+1 );
	*sending_path = '\n';
	
	// EXTRACT - Sending Filename (could be "*")
	char* sending_filename = strchr(sending_path+1, '\n');
	*sending_filename = 0;
	strcpy (SendFilename, sending_path+1);
	if (sending_filename == sending_path+1)		// empty string.
	{
	}
	*sending_filename = '\n';		
	
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
int packet_count=0;

void* file_server_thread(void* msg)
{ 
    int readsocks = 0;				/* number of updated sockets */
    struct sockaddr_in serv_addr;  
    //char sendBuff[1025];
    time_t ticks; 

    // File scope vars:
    total_file_bytes_received = 0;
    files_transfered  = 0;
    
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
	printf( "\n BK File Transfer Thread  :  IP=%s:%d \n\n", ip_addr, port );

	/* To send & receive files simulataneously, need to set the socket to non-blocking.
	   when not receiving, send, etc.		*/

	/* wait for a client to talk to us */
    BOOL done = FALSE;
    //while(1)
    {
    	// WAIT FOR A NEW SESSION:
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		printf("=== File Transfer Socket opened ===\n");		

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
				handle_file_data( 0, bytes_rxd );
				if (file_terminate_requested)
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


/* Wait up to five seconds. 
struct timeval tv;	    
tv.tv_sec = 5;
tv.tv_usec = 0;	
int num_sockets = select(listenfd, &connfd, NULL, NULL, &tv);
printf("=== File Transfer Socket select === retval=%d: readfds=%d\n",num_sockets, connfd);		
*/
/* Nonblocking did not work so good:  abondoning use of it.
	accept() is blocking and so nonblock needs a select() before accept.
	Also didn't work well.
	So we're going to use a separate thread for sending files than for receiving.
	normal way is only 1 anyway.

// Put the socket in non-blocking mode:
//if(fcntl(listenfd, F_SETFL, fcntl(listenfd, F_GETFL) | O_NONBLOCK) < 0)
{
	// handle error
	printf("Cannot set nonblocking mode\n");
}	*/
