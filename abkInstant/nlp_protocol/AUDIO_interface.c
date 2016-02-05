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
//#include "thread_control.h"
#include "audio_thread.h"
#include "utilities.h"
#include "audio_memory.h"
#include "AUDIO_device.h"
#include "AUDIO_protocol.h"
#include "AUDIO_file_util.h"
#include "serverthread.h"



#define MAX_USERS     10
#define MAX_LISTENERS 12


#define 		AUDIO_BUFFER_SIZE 16384

// short or byte?!  byte since deals with header ptr data. 
static byte 	buffer[AUDIO_BUFFER_SIZE];		// 4 byte token + 4 byte length
byte			audio_socket_buffer[AUDIO_OUTPUT_BUFFER_SIZE+100];
static int 	 	bytes_rxd=0;

struct wav_header wave_header;						// first packet
BOOL 		start_new_file = TRUE;
BOOL  		header_received = FALSE;
char* 		header_position = (char*)&wave_header;	// start at the top
BOOL		play_started = FALSE;


/******************************************************
* Process 
* return true 	- The telegram was handled.
*        false  - Not known token
******************************************************/
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

	if (AUDIO_save_requested)
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
		audio_ipc_write_buffer( (short*)buffer, bytes_rxd>>1 );
	}

	if (AUDIO_tcpip_ListeningOn)
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


// FIR - Finite Impulse Response
// 	first element represents delay of 0. 2nd=>1 sample; etc.
// Result goes into mSourceData
void audio_subtract( short* mSourceData, short* mDestData, int mLength,
				     short* mFIR, int mFIRLength )
{
    // buffer should be data only!
    for (int delay=0; delay<mFIRLength; delay++)
	for (int i=0; i<mLength; i++)
	    mSourceData[i] -= mDestData[i-delay] * mFIR[delay];
    
}

void MuteBuffer( byte* mData, int mLength )
{
    // make sure not to overwrite a length or other command bytes
    // buffer should be data only!
    memset(mData, 0, mLength );
}

// ECHO CANCELLATION:
bool use_fir = false;		// echo cancellation
short FIR[255];
int FIR_length=255;
//long int file_position = 0;


/* 	Called from server_thread, when there is no incoming data.
	This function is polled to send audio data when available and
	the data is requested.
*/
void audio_interface()
{
	struct sCAN* tmpCAN = NULL;
	int    bufSize = 255;
	unsigned char  buff[255];
	
	if (AUDIO_tcpip_SendingOn)
	{
	    printf("Audio_SendingOn is ON!\n");	    
	    if (sending_audio_file_fd)
	    {
			// file position will be maintained automatically as long as the file descriptor is
			// not destroyed.
			read_chunk     ( audio_socket_buffer, AUDIO_OUTPUT_BUFFER_SIZE );
			if ( AUDIO_tcpip_SendingMuted )
				MuteBuffer( (byte*)audio_socket_buffer, AUDIO_OUTPUT_BUFFER_SIZE/2 );
			SendTelegram( audio_socket_buffer, AUDIO_OUTPUT_BUFFER_SIZE );
	    }
	    else
	    {
			// PUMP AUDIO RECORDING QUEUE:
			BOOL available = audio_is_new_rxbuffer();
				/* the messages will be pulled off of the Received buffer.
				   and stored in Recieved buffer at the other instant end.  */
			if ((available) && (ipc_memory_aud))
			{				
				memcpy( audio_socket_buffer, ipc_memory_aud->audio_data, ipc_memory_aud->update_samples*sizeof(sample) );
				if (AUDIO_tcpip_SendingMuted)
					MuteBuffer( (byte*)audio_socket_buffer, AUDIO_OUTPUT_BUFFER_SIZE/2 );
				else if ((AUDIO_tcpip_ListeningOn) && (use_fir))
					audio_subtract( (short*)audio_socket_buffer, (short*)buffer, AUDIO_OUTPUT_BUFFER_SIZE/2 ,
									FIR, FIR_length);	
				SendTelegram( audio_socket_buffer, AUDIO_OUTPUT_BUFFER_SIZE );
			}
		}
	}
	// the other direction is be done in serverthread Parse_CAN_Statement()!
}

