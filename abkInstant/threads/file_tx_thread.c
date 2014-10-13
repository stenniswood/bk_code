/**********************************************************
 This file is a helper utility for sending: a file, a directory,
 or sub-directories of files.   Can be recursively or not.

 Another abkInstant will receive (with file_thread.c)
 
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
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <ifaddrs.h>
#include <time.h> 
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h> 
#include <stdio.h> 

#include "bk_system_defs.h"
#include "devices.h"
#include "thread_control.h"
#include "file_thread.h"

using namespace std;



static int		packet_count=0;
static int  	sockfd=0;

static FILE* 	fd = NULL;
static char 	SendPath[128];		static int FilePathEnd=0;
static char 	SendFilename[128];	static int FileNameEnd=0;
static char 	SendHeader[255];	static int SendHeaderEnd=0;
static char 	FileSize[16];		static int FileSizeEnd=0;
static char 	LocalPath[] = "./media/";


// The file size will be a 6 byte number.  Ie. 4gigs * 65535 = tera bytes
static long int 		file_size 		  = 0;
static int				port;

#define OUTPUT_BUFFER_SIZE 8192
static byte 		oBuff[OUTPUT_BUFFER_SIZE];
struct stat 		file_info;

void transfer_file(char* mFilePath, char* mFileName)
{
	int 		plength = strlen(mFilePath);
	int 		flength = strlen(mFileName);
	int 		slength;
	long int 	file_size = 0;
	char 		FileSize[20];
	long int 	chunk_size = 0;
	string		full_name = mFilePath;
	full_name   += mFileName;

		// Send File Info (Path, name, filesize)
		printf("full_name.c_str(): |%s|\n", full_name.c_str() );
		int result = stat((char*)full_name.c_str(), &file_info);
		int errsv = errno;
		perror("fstat");
		sprintf( FileSize, "%d\n", file_info.st_size );

		//slength = strlen(FileSize);

	/* Okay mFilePath needs a "\n" on the end.  The receiving side needs it 
	for parsing. So mFilePath has to be an array big enough to hold 1 more character.
	And SendPath which is what's passed in (only 1 caller)  
	definitely is such an array. */

	strcpy (SendHeader, mFilePath );
	strcat (SendHeader, mFileName );		// yes, the receive thread looks for filename 2x
	plength = strlen(SendHeader);
	SendHeader[plength] = '\n'; 		SendHeader[plength+1] = 0;
	strcat (SendHeader, mFileName );		// filename here also!
	slength = strlen(SendHeader);
	SendHeader[slength] = '\n';			SendHeader[slength+1] = 0;
	strcat (SendHeader, FileSize );
	slength = strlen(SendHeader);
	SendHeader[slength] = '\n';			SendHeader[slength+1] = 0;
	printf ( "%s\n", SendHeader );

	// Append \n for the receiving end to parse.
//	mFilePath[plength]   = '\n'; 	plength++;
//	mFilePath[plength]   = 0   ;
//	mFileName[flength]   = '\n';	flength++;
//	mFileName[flength]   = 0   ; 

		// send fullpath
		write( sockfd, SendHeader, slength );

		// Remove the trailing \n for the next time.
		//mFilePath[plength-1]   = 0;
		// send filename
		// write(sockfd, mFileName, flength );
		// Remove the trailing \n for the next time.
		//write(sockfd, FileSize, slength ); 

	FILE* fd   = fopen(full_name.c_str(), "rb");
	while ((feof(fd)==0) && (chunk_size != -1))
	{
		// Read buffer's worth:
		chunk_size = fread( oBuff, 1, OUTPUT_BUFFER_SIZE, fd);
		// Send buffer:
		write(sockfd, oBuff, chunk_size); 	
	}	
	fclose(fd);
}

vector<string> Filelist;

void retrieve_name_list( char* mPath, bool include_hidden_files )
{
	DIR           *d;
	struct dirent *dir;
	d 	=   opendir( (const char*)mPath );
	int errsv = errno;
	perror("opendir");
	string 	Pathfilename;
	bool	is_hidden_file = false;
	if (d) 
	{
		// Go thru all FILES:
		while ((dir = readdir(d)) != NULL)
		{			
			if (dir->d_type==DT_REG)
			{
				is_hidden_file = (dir->d_name[0]=='.')?true:false;
				if ((include_hidden_files==false) && (is_hidden_file))
				{
					continue;		// skip!
				}
				Pathfilename = dir->d_name;
				printf("Adding =%s\n", Pathfilename.c_str() );
				Filelist.push_back( Pathfilename );
			}
		}
		closedir(d);
	}
}

string Directory;
void loop_directory_files(char* mDirectory)
{
  DIR           *d;
  struct dirent *dir;
  d = opendir(mDirectory);  
  if (d)
  {
	    // First, go thru all files:
	    retrieve_name_list( mDirectory, false );

		while ((dir=readdir(d)) != NULL)
		{	
			// Next go thru all directories:
			// if a directory, then recurse:
			if (dir->d_type==DT_DIR)  {
				//printf("%s\n", 		dir->d_name);
				Directory = mDirectory;
				Directory += dir->d_name;
				loop_directory_files( (char*)Directory.c_str());
			}
		}
		closedir(d);
	}
}


// The Filelist is already populated.
/*#define SEND_BUFFER_SIZE 8192
using namespace std;

int FileSendIndex = 0;
FILE* send_fd 		= 0;
int FileSendState = SEND_IDLE;
int send_buffer[SEND_BUFFER_SIZE];
vector<string> Filelist;
long int bytes_to_send=0;
void handle_file_send()		// Asynchronous Method.  Not using!
{
	switch(FileSendState)
	{
	case SEND_IDLE:
		if (FileSendIndex < Filelist.size())	
			FileSendState = SEND_NEW_FILE;
		break;
	case SEND_NEW_FILE:
		send_fd = fopen( Filelist[FileSendIndex].c_str(), "rb" );
		if (send_fd)
			FileSendState = SEND_ONE_CHUNK;
		break;
		
	case SEND_ONE_CHUNK:
		// Read, then write data:
		bytes_to_send = fread( send_buffer, 1, SEND_BUFFER_SIZE, send_fd ); 
		write(connfd, send_buffer, bytes_to_send); 
		if (feof(send_fd))
			FileSendState = SEND_QUE_NEXT;
		break;
		
	case SEND_QUE_NEXT:	
		fclose(send_fd);
		FileSendIndex++;
		if (FileSendIndex<Filelist.size())
			FileSendState = SEND_NEW_FILE;
		else
			FileSendState = SEND_ALL_DONE;
		break;

	case SEND_ALL_DONE:	
		break;
	default:
		break;	
	}
}
*/
char ip_address[20];

 void parse_thread_params(char* mMsg)
{
	printf("Parse_thread_params:\n" );
	char* delim = strchr(mMsg, '\n');
	if (delim==NULL)  return;	
	*delim = 0;
	strcpy( ip_address, mMsg );
	*delim = '\n';		// restore
	printf("Destination IP=%s\n", ip_address );

	char* delim2 = strchr(delim+1, '\n');
	if (delim2==NULL) return;
	*delim2 = 0;
	strcpy( SendPath, delim+1 );
	*delim2 = ':';
	printf("Local Path    =%s\n", SendPath );

	// EXTRACT - Port
	char* delim3 = strchr(delim2+1, '\n');
	if (delim3==NULL) return;
	*delim3 = 0;
	strcpy( SendFilename, delim2+1 );	
	*delim3 = ':';
	printf("Filename      =%s\n", SendFilename );
	
	// EXTRACT - Port
	char* delim4 = strchr(delim3+1, '\n');
	if (delim4==NULL) return;
	*delim4 = 0;
	port = atoi( delim3+1 );
	printf("Port	      =%d\n", port );	
	
	///////////////////////////////////////////////////////////
	// Output (File client) side parameters:
	// User may send an entire directory, recursively adding 
	// all sub directories. 
}

/******************************************************
* thread function - Here we are the client!
* return 1 => Error
*        0 => Okay
******************************************************/
void* file_transmit_thread( void* msg )
{
	parse_thread_params( (char*)msg      );
	retrieve_name_list ( SendPath, false );

	printf("====file_transmit_thread...\n");
    struct sockaddr_in	serv_addr;
	struct in_addr      addr;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return (void*)1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(port);

    if(inet_pton(AF_INET, ip_address, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_aton error occured\n");
        return (void*)1;
    }

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return (void*)1;
    } 
	printf("ROBOT CLIENT Connected to : %s:%d\n", ip_address, port );
		
	int length = Filelist.size();
	for (int i=0; i<length; i++)
	{		
		printf("Transmitting File #%d/%d : %s\n", i,length, Filelist[i].c_str() );
		transfer_file( SendPath, (char*)Filelist[i].c_str() );
	}
	close (sockfd);
    return (void*)0;
}




