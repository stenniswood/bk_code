/* This header is a conglomeration of the *_util.c files functions */
#include "protocol.h"

extern struct WAVE_HEADER audio_hdr;

// from audio_util.c : 
char* GetNewAudioFileName	( BOOL mOutgoing	);
BOOL  OpenAudioFile			( char* mFilename	);
BOOL OpenAudioFileRead( char* mFilename );

void  AppendAudioData		( char* mBuffer, int mSize );
void  CloseAudioFile		(					);
void  FindFile_by_ClosestTimestamp( int mFile );
void  FindFile_by_Name		( int mFile );
WORD* CompressAudioData		( WORD* mAudio );
WORD* UncompressAudioData	( WORD* mAudio );

BOOL  OpenAudioFileRead     ( char* mFilename );
void  read_chunk            ( byte* mBuffer, int mSize );


// from video_util.c : 
void GetNewVideoFileName();
void OpenVideoFile();
