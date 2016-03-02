#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bk_system_defs.h"
#include "protocol.h"
#include "devices.h"
#include "AUDIO_interface.hpp"
#include "AUDIO_protocol.hpp"
#include "AUDIO_device.hpp"
 


/***********************************************************************
All Incoming Audio will also be saved to the hard-drive on those 
systems with enough capacity.  This allows the user to call back anything
in its memory banks.  Think star trek logs.  "There was an incoming 
message from so and so on October 31 2006".  Can you replay it?  
What was the topic of it?  How long was it?  A smart robot could even
use the info to answer questions.  On Sept 20th, you said you wanted to
ignore all upgrade notices.
***********************************************************************/
//char   base[] = "/Users/stephentenniswood/SocketServer/BK_MEDIA_";

char   path[] = "./audio/";
char   base[] = "BK_MEDIA_";
char   AudioFilename[255];
//struct WAVE_HEADER audio_hdr;
FILE*  wave_file;
long   TotalAudioDataLengthBytes = 0;	// Holds total accumulated over all frames received.
BOOL   SaveFileOpen = FALSE;

/************************************************
Generate Audio File name based on the timestamp
BOOL mIncoming
*************************************************/
char* GetNewAudioFileName( BOOL mOutgoing )
{
	static int ordinal = 1000;	// this needs better management (ie. between sessions)
	ordinal++;					// increment on each call to this function.

	//AudioFilename = malloc( strlen(base)+9+10 );
	char tmp[10];
	snprintf( tmp, 10, "%d", ordinal );

	strcpy( AudioFilename, path );
	strcpy( AudioFilename+strlen(AudioFilename), base );
	if (mOutgoing)	strcpy( AudioFilename+strlen(AudioFilename), "OUT_" );
	strcpy( AudioFilename+strlen(AudioFilename), tmp    );
	strcpy( AudioFilename+strlen(AudioFilename), ".wav" );	
	return AudioFilename;
}

/************************************************
Create and Open Audio File for saving
*************************************************/
BOOL OpenAudioFile( char* mFilename )
{
	// DATA DEFS:
	char riff[] = "RIFF";	
	char qs[]   = "????";
	char wave[] = "WAVE";
	char fmt[]  = "fmt ";
	char data[] = "data";
	int  file_length     = 0x21212121;
	int  fmt_chunk_size  = 16;

	//	FILE* wave_file;
	strcpy( AudioFilename, mFilename );
	wave_file = fopen(mFilename, "wb");
	if (wave_file==NULL) {
		printf(" %s \n", mFilename );
		printf("Error opening file, make sure you are running as root 'sudo ./server' \n");
		return FALSE;
	}
	printf("Opened file: %s \n", mFilename );

	// WRITES:
	fwrite( (void*)riff, 		1, 4, wave_file );
	fwrite( (void*)qs,			1, 4, wave_file );
	fwrite( (void*)wave, 		1, 4, wave_file );
	
	// FORMAT SUB CHUNK :
	fwrite( (void*)fmt,  			1, 4, wave_file );
	fwrite( (void*)&fmt_chunk_size, 1, 4, wave_file );
	fwrite( (void*)&audio_hdr,  	2, 8, wave_file );
				
	// DATA SUB CHUNK (written in AppendAudioData() only)
	fwrite( (void*)data,  		1, 4, wave_file );		// ID
	fwrite( (void*)qs,  		1, 4, wave_file );		// Size unknown at this time.

	// Need to know the Total File size!  When streaming, we don't know at the start.
	// So keep an accumulated byte count and seek to the top of the file and update this
	// later on.	
	//printf("OpenAudioFile: end. \n");
	TotalAudioDataLengthBytes = 0;
	return TRUE;
}

/************************************************
Create and Open Audio File for Reading
*************************************************/
BOOL OpenAudioFileRead( char* mFilename )
{
	// DATA DEFS:
	char riff[] = "RIFF";	
	char qs[]   = "????";
	char wave[] = "WAVE";
	char fmt[]  = "fmt ";
	char data[] = "data";
	int  file_length     = 0x21212121;
	int  fmt_chunk_size  = 16;

	//	FILE* wave_file;
	//strcpy( AudioFilename, mFilename );
	//wave_file = fopen(mFilename, "r");
	if (sending_audio_file_fd==NULL) {
		printf(" %s \n", mFilename );
		printf("Error opening file, make sure you are running as root 'sudo ./server' \n");
		return FALSE;
	}
	printf("Opened file: %s \n", mFilename );

	// WRITES:
	fread( (void*)riff, 		1, 4, sending_audio_file_fd );
	fread( (void*)qs,		1, 4, sending_audio_file_fd );
	fread( (void*)wave, 		1, 4, sending_audio_file_fd );
	
	// FORMAT SUB CHUNK :
	fread( (void*)fmt,  			1, 4, sending_audio_file_fd );
	fread( (void*)&fmt_chunk_size, 1, 4, sending_audio_file_fd );
	fread( (void*)&audio_hdr,  	2, 8, sending_audio_file_fd );
				
	// DATA SUB CHUNK (written in AppendAudioData() only)
	fread( (void*)data,  		1, 4, sending_audio_file_fd );		// ID
	fread( (void*)qs,  		1, 4, sending_audio_file_fd );		// Size unknown at this time.

	// Need to know the Total File size!  When streaming, we don't know at the start.
	// So keep an accumulated byte count and seek to the top of the file and update this
	// later on.	
	//printf("OpenAudioFile: end. \n");
	TotalAudioDataLengthBytes = 0;
	return TRUE;
}
void  read_chunk( byte* mBuffer, int mSize )
{
	fread( (void*)mBuffer,  1, mSize, sending_audio_file_fd );
	
}

/************************************************
Append Audio Data
*************************************************/
void AppendAudioData( char* mBuffer, int mSizeBytes )
{
	// SAVE TO FILE:
	if (mBuffer  ==NULL)
		printf("AppendAudioData: buffer is NULL.\n" );
	if (wave_file==NULL)
		printf("AppendAudioData: file handle is NULL.\n" );		
	//printf("AppendAudioData: fp=%d  %d\n", wave_file, mSizeBytes );

	TotalAudioDataLengthBytes += mSizeBytes;
	// 1 size of element.
	// mSize number of elements.
	fwrite( mBuffer, 1, mSizeBytes, wave_file );				// Actual data
}

/************************************************
After the audio has stopped streaming, we
can compute the chunk sizes and update the file
*************************************************/
void UpdateChunkSizes( int mDataChunkSize )
{
	int mRIFFChunkSize = 36 + mDataChunkSize;
	char qs[] = "    ";
	char ones[] = "1111";
	char twos[] = "2222";
		
	// RIFF CHUNK SIZE:
	fseek ( wave_file, 4,  SEEK_SET );
	fwrite( (void*)&mRIFFChunkSize,	1, 4, wave_file );		// File Size 
	//fwrite( (void*)ones,	1, 4, wave_file );				

	// DATA CHUNK SIZE:
	fseek ( wave_file, 40, SEEK_SET );
	fwrite( (void*)&mDataChunkSize,	1, 4, wave_file );		// Data Size 
	//fwrite( (void*)twos,	1, 4, wave_file );				
}

/************************************************
Close Audio File - for convenience
*************************************************/
void CloseAudioFile( )
{
	printf("Closing file:  %s\n", AudioFilename );
	printf("DataChunkSize = %ld bytes\n", TotalAudioDataLengthBytes );
	UpdateChunkSizes( TotalAudioDataLengthBytes );
	fclose( wave_file );
}

void test_save()
{
    printf("Test file save...\n");
	char* fn = GetNewAudioFileName(FALSE);	
	char fn2[] = "/Users/stephentenniswood/SocketServer/TEST.wav";	
	OpenAudioFile( fn );
	CloseAudioFile( );
}

/************************************************
Find File Closest Timestamp
*************************************************/
void FindFile_by_ClosestTimestamp( int mFile )
{
}

/************************************************
Generate File List - creates a string with all
audio files within a range.
*************************************************/
void FindFile_by_Name( int mFile )
{
}

/************************************************
Compress Audio (by unknown means right now)  
Probably call Ogvorbis library.  mp3 still under
patents?
*************************************************/
WORD* CompressAudioData( WORD* mAudio )
{ return 0;
}

/************************************************
Uncompress Audio (by unknown means right now)  
Probably call Ogvorbis library.  mp3 still under
patents?
*************************************************/
WORD* UncompressAudioData( WORD* mAudio )
{ return 0;
}


