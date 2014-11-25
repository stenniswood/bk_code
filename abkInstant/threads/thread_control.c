#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <list>
#include "bk_system_defs.h"
#include "pican_defines.h"

//#include "ipc_mem_bkinstant.h"
#include "audio_thread.h"
#include "video_thread.h"
#include "can_thread.h"
#include "hmi_thread.h"
#include "file_thread.h"

//#include "audio_tx_thread.h"
//#include "video_tx_thread.h"
//#include "can_tx_thread.h"
//#include "hmi_tx_thread.h"
#include "file_tx_thread.h"

#include "thread_control.h"



 
char 	 ip_addr[16]; 			// for user feedback

void 	exit1() {	while (1==1) {  }; }

std::list<int> AudioPortsInUse;
std::list<int> VideoPortsInUse;
std::list<int> CANPortsInUse;
std::list<int> HMIPortsInUse;
std::list<int> FilePortsInUse;

/***********************************************************************
This file is responsible to establishing and taking down threads 
as requested by the user.  For establishing audio/video/CAN traffic.
Tenniswood - Beyond Kinetics, Inc.  2014
 ***********************************************************************/
/*
	What's displayed here is going to be under the control of bkInstant.
	We'll start by showing the incoming text.
*/
/***********************************************************************/
extern int DisplayNum;
#define Debug 0


pthread_t audio_thread_id;
pthread_t video_thread_id;
pthread_t can_thread_id;
pthread_t hmi_thread_id;
pthread_t file_thread_id;

/********************************************************************
Init - Initialize all submodules
********************************************************************/
void init_server()
{
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

/*****************************
The message can be:
"receive&play"
"receive&save"
"receive&play&save"
*******************************/
void create_audio_thread( BOOL mPlay, BOOL mSave, int Port )
{	
	static int first_time = TRUE;
	if (first_time)
	{
		first_time = FALSE;
	// FORM THE PARAMETER LIST:
	static char message[] = "play:none:600001   ";	
	if (mPlay)
		memcpy (message, "play", 4);
	if (mSave)
		memcpy (message+5, "save", 4);
	sprintf(message+10, "%d", Port);
	printf("audio thread args:  |%s|\n", message);

	// CREATE THRE THREAD:			
	int iret1 = pthread_create( &audio_thread_id, NULL, audio_server_thread, (void*) message);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
	}
}
void terminate_audio_thread( )
{	
	audio_terminate_requested = TRUE;
}

/*****************************
The message can be:
"receive&play"
"receive&save"
"receive&play&save"
*******************************/
void create_video_thread( BOOL mPlay, BOOL mSave, int Port )
{	
	// FORM THE PARAMETER LIST:
	static char message[] = "none:none:600001";	
	if (mPlay)
		memcpy (message, "play", 4);
	if (mSave)
		memcpy (message+5, "save", 4);
	sprintf(message+10, "%d", Port);

	int iret1 = pthread_create( &video_thread_id, NULL, video_server_thread, (void*) message);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}

void terminate_video_thread( )
{	 
	video_terminate_requested = TRUE;
}


/*****************************
The message can be:
"receive&play"
"receive&save"
"receive&play&save"
*******************************/
void create_CAN_thread( BOOL mPlay, BOOL mSave, int Port )
{	
	// FORM THE PARAMETER LIST:
	static char message[] = "none:none:600001";	
	if (mPlay)
		memcpy (message, "play", 4);		// play on piCAN
	if (mSave)
		memcpy (message+5, "send", 4);
	sprintf(message+10, "%d", Port);

	int iret1 = pthread_create( &can_thread_id, NULL, CAN_server_thread, (void*) message);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}

void terminate_CAN_thread(  )
{	
	can_terminate_requested = TRUE;
}

void create_HMI_thread( BOOL mPlay, BOOL mSave, int Port  )
{	
	// FORM THE PARAMETER LIST:
	static char message[] = "none:none:600001";	
	if (mPlay)
		memcpy (message, "play", 4);		// play on piCAN
	if (mSave)
		memcpy (message+5, "send", 4);
	sprintf(message+10, "%d", Port);

	int iret1 = pthread_create( &hmi_thread_id, NULL, hmi_server_thread, (void*) message);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}
void terminate_HMI_thread( 		)
{	
	hmi_terminate_requested = TRUE;
}


/*****************************
The message can be:
"receive&play"
"receive&save"
"receive&play&save"
*******************************/
void create_file_thread( BOOL mPlay, BOOL mSave, int Port)
{	
	// FORM THE PARAMETER LIST:
	static char message[] = "none:none:600001";	
	if (mPlay)
		memcpy( message, "play", 4 );
	if (mSave)
		memcpy( message+5, "save", 4 );
	sprintf(message+10, "%d", Port);
	printf("file_thread.message=%s\n", message);

	// CREATE THRE THREAD:			
	int iret1 = pthread_create( &file_thread_id, NULL, file_server_thread, (void*) message);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}
void terminate_file_thread( )
{	
	file_terminate_requested = TRUE;
}


/***********************************************************************
						CLIENT THREADS
***********************************************************************/

pthread_t file_tx_thread_id;
pthread_t audio_tx_thread_id;

/*****************************
The message can be:
"receive&play"
"receive&save"
"receive&play&save"
*******************************/
void create_file_tx_thread( char* mDest_IP, char* mLocalPath, char* mFilename, int Port )
{
	// FORM THE PARAMETER LIST:
	int ip_length   = strlen(mDest_IP);
	int path_length = strlen(mLocalPath);
	int file_length = strlen(mFilename);
	int port_length = 6;
	int total_length = ip_length + path_length + file_length + port_length;

	char* message = new char[total_length];
	char*  ptr = message;
	strcpy(ptr, mDest_IP);			ptr += ip_length;	*ptr = '\n';	ptr++;
	strcpy(ptr, mLocalPath);		ptr += path_length; *ptr = '\n';	ptr++;
	strcpy(ptr, mFilename);			ptr += file_length; *ptr = '\n';	ptr++;
	sprintf( ptr, "%d\n", Port);
	//printf("create_file_tx_thread: message=%s\n", message );

	// CREATE THREAD:
	int iret1 = pthread_create( &file_tx_thread_id, NULL, file_transmit_thread, (void*) message);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}

void terminate_file_tx_thread( )
{	
	file_terminate_requested = TRUE;
}


/*****************************************************************
The audio transmit thread is intended to send live stereo output
such as microphone to a receiving loud speaker.  This allows 
intercom like functionality.  However since the receiving rpi
can also save to hard drive it could also be used for recording meetings
ease dropping on children when away from home.  

The client could be easily modified to send a wave file.  For remote
sound effects/playing music.  

One could also make a hardware that would convert radio signals to 
digital and send.  or TV audio.

The message can be:
"receive&play"
"receive&save"
"receive&play&save"
*******************************/
void create_audio_tx_thread( char* mHeader, int Port )
{	
	// FORM THE PARAMETER LIST:
	int port_length = 6;	
	int total_length = 6;
	
	char* message = new char[total_length];
	printf("audio_thread.message=%s\n", message);

	// CREATE THREAD:
	//int iret1 = pthread_create( &audio_tx_thread_id, NULL, audio_transmit_thread, (void*) message);
	//if (iret1)
	{
	//	fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}
void terminate_audio_tx_thread( )
{	
	audio_terminate_requested = TRUE;
}

void create_video_tx_thread		( BOOL mPlay, BOOL mSave, int Port )
{	
	// FORM THE PARAMETER LIST:
	int port_length = 6;	
	int total_length = 6;
	
	char* message = new char[total_length];
	printf("video_thread.message=%s\n", message);

	// CREATE THREAD:
	//int iret1 = pthread_create( &video_tx_thread_id, NULL, video_transmit_thread, (void*) message);
	//if (iret1)
	{
	//	fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}
void terminate_video_tx_thread(  )
{	
	video_terminate_requested = TRUE;
}

void create_CAN_tx_thread	  	( BOOL mPlay, BOOL mSave, int Port )
{	
	// FORM THE PARAMETER LIST:
	int port_length = 6;	
	int total_length = 6;
	
	char* message = new char[total_length];
	printf("can_thread.message=%s\n", message);

	// CREATE THREAD:
	//int iret1 = pthread_create( &can_tx_thread_id, NULL, can_transmit_thread, (void*) message);
	//if (iret1)
	{
	//	fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}
void terminate_CAN_tx_thread( 	)
{	
	can_terminate_requested = TRUE;
}


void create_HMI_tx_thread( BOOL mPlay, BOOL mSave, int Port )
{	
	// FORM THE PARAMETER LIST:
	int port_length = 6;	
	int total_length = 6;
	
	char* message = new char[total_length];
	printf("hmi_thread.message=%s\n", message);

	// CREATE THREAD:
	//int iret1 = pthread_create( &hmi_tx_thread_id, NULL, hmi_transmit_thread, (void*) message);
	//if (iret1)
	{
	//	fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
}
void terminate_HMI_tx_thread 	( 	)
{	
	hmi_terminate_requested = TRUE;
}
