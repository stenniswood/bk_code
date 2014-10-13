/******************************************************************
 This file drives the hardware of the Mac to play incoming audio.
 Mac osX 10 specific.
 *****************************************************************/
#include <stdio.h>
#include <string.h>
#include "bk_system_defs.h"
#include "protocol.h"
#include "devices.h"
//#include "AudioToolbox/AudioToolbox.h"	// Mac specific

/************ VARIABLES & CONSTANTS ****************************/

//#define AUDIO_BUFF_SIZES 3844

#define BUFFERS 	4
#define ONE_SECOND_HIFI_SIZE  (44100*4)
float  mult    		= 1.0;
char   up_down  	= 'u';
double phase    	= 0;
double phase_inc   	= 2 * 3.14159265359 * 450 / 44100;
char*  AudioBuffers[BUFFERS][ONE_SECOND_HIFI_SIZE];
int    intermediate_buff_index    = 0;
int    intermediate_cb_buff_index = 0;
BOOL	PlaybackStarted = FALSE;
BOOL 	audio_hardware_enabled = FALSE;


/*AudioQueueRef 					queue;
AudioQueueBufferRef 			buf_ref[BUFFERS];
AudioStreamBasicDescription 	fmt = { 0 };*/

/*****************************************************************
This function needs to be redone for each different hardware 
(ie. Mac, Rpi, Windows, Android Device, etc)
return  DEVICE_CAPS
*****************************************************************/
struct DEVICE_CAPS* DEVICE_ComposeAudioCapabilitiesStructure( )
{
/*	static struct DEVICE_CAPS retval;
	retval.Recordable_Channels = 2;
	retval.Playable_Channels   = 2;
	retval.Max_SampleRate 	   = 44100;
	return &retval;*/
}


/*****************************************************************/
/**************** AUDIO PLAY FUNCTIONS ***************************/
/*****************************************************************/

/* Can only dispose of a buffer when it's stopped.
   Therefore keep the buffers allocated and enqueued.
   We need two buffers.  Start Play after the first is filled.
   Back to our original strategy - intermediate buffers.
   This time we just don't queue up before start.
*/
  
/******************************************************************
 Called when a buffer is available from the Play Queue (ie. one has 
 just finished playing.
 *****************************************************************/
/*void callback(void *ptr, AudioQueueRef queue, AudioQueueBufferRef buf_ref)
{
  OSStatus status;
  AudioQueueBuffer *buf = buf_ref;
  int    nsamp = buf->mAudioDataByteSize / 2;
  short *samp = buf->mAudioData;
  printf ("Callback! index=%d nbytes: %d\n", intermediate_cb_buff_index, buf->mAudioDataByteSize);

  // FILL DATA HERE:  
  memcpy( samp, &(AudioBuffers[intermediate_cb_buff_index]), buf->mAudioDataByteSize );
  
  //memcpy( samp, &(samp), buf->mAudioDataByteSize );

  intermediate_cb_buff_index++;
  if ((intermediate_cb_buff_index % BUFFERS)==0)
      intermediate_cb_buff_index = 0;	// play back is slower than incoming data (loose a buffer)  
  
  // REQUEUE:  
  status = AudioQueueEnqueueBuffer( queue, buf_ref, 0, NULL );
  printf ("Enqueue status: %d\n", status);
}*/

/***************************************************************
Initialize the Wave Format structure
was a param : AudioStreamBasicDescription* fmt
****************************************************************/
void InitFormatParameters(  BYTE mChannels, short mSampleRate )
{/*
  fmt.mSampleRate 		= mSampleRate;		
  fmt.mFormatID 		= kAudioFormatLinearPCM;
  fmt.mFormatFlags 		= kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
  fmt.mFramesPerPacket 	= 1;
  fmt.mChannelsPerFrame	= mChannels; 	// x2 for stereo
  fmt.mBytesPerPacket 	= 2*mChannels;
  fmt.mBytesPerFrame  	= 2*mChannels; 	// x2 for stereo
  fmt.mBitsPerChannel 	= 16; */
}
/******************************************************
THE STRUCT IS NEEDED because:  this is exactly 
	what's saved into the .WAV file.
AudioStreamBasicDescription* fmt
*******************************************************/
void AUDIO_init_hdr( BYTE mChannels, short mSampleRate  )
{
/*	audio_hdr.format 			= 1;	
	audio_hdr.channels_rxd 		= mChannels;
	audio_hdr.sample_rate 		= mSampleRate;
	audio_hdr.byte_rate 		= mSampleRate * 2 * mChannels;
	audio_hdr.block_align 		= 2 * mChannels;
	audio_hdr.bits_per_sample 	= 16; */
}

/*****************************************************************
Initialize the Wave Format structure
****************************************************************/
void AUDIO_SetupPlayer( BYTE mChannels, short mSampleRate )
{
/*  OSStatus 	status;
  int 		i;
  InitFormatParameters( mChannels, mSampleRate );
  AUDIO_init_hdr( mChannels, mSampleRate );
  printf("AUDIO_SetupPlayer: done with AUDIO_init_hdr\n");
    
  // CONNECT TO OUTPUT DEVICE:
  status = AudioQueueNewOutput(&fmt, callback, &phase, NULL,  //CFRunLoopGetCurrent(),
			                   kCFRunLoopCommonModes, 0, &queue);

  if (status == kAudioFormatUnsupportedDataFormatError) puts ("oops!");
  else printf("NewOutput status: %d\n", status);

  // We'll start by just enqueue 1 buffer.  The rest will get enqueue by the callback
  intermediate_buff_index    = 0;
  intermediate_cb_buff_index = 0;
  
  for (i=0; i<BUFFERS; i++)
	  fillBuffer( (short*)&(AudioBuffers[i]), ONE_SECOND_HIFI_SIZE/2, 450 );
  
  status = AudioQueueSetParameter (queue, kAudioQueueParam_Volume, 1.0);
  printf("Volume status: %d\n", status);
 
    // THIS ONLY NEEDS TO BE CALLED ONCE!  YEA!
      CFRunLoopRunInMode (
        kCFRunLoopDefaultMode,
        0.25, // seconds
        false // don't return after source handled
	  );
  audio_hardware_enabled = TRUE;
  printf("--Mac AUDIO HARDWARE PREPARED!\n");  */
}
/*****************************************************************
Initialize the Wave Format structure
****************************************************************/
void AUDIO_StartPlay()
{
/*  OSStatus 	status;
  status = AudioQueueStart( queue, NULL );
  printf( "Start status: %d\n", status  );

  PlaybackStarted = TRUE;
  printf("--AUDIO_StartPlay()\n"); */
} 
/*****************************************************************
 First buffer is from the Socket.
 These is an intermediate buffer.  
 Last buffer is to the Mac AudioQueueBuffers.
 Initialize the Wave Format structure
****************************************************************/
BOOL AUDIO_QueueBuffer( char* mBuffer, int mSize )
{
	// Hold the buffer internally and give to Hardware on the next callback.    
	// status = AudioQueueAllocateBuffer( queue, AUDIO_BUFF_SIZES, &buf_ref[i] );    
	// AudioQueueBuffer *buf = buf_ref[intermediate_buff_index];	
/*	OSStatus 	status;
	
    if ((intermediate_buff_index < BUFFERS) && 
        (AudioBuffers[intermediate_buff_index] != NULL ))
    {
		status      = AudioQueueAllocateBuffer( queue, mSize, &buf_ref[intermediate_buff_index] );
		short *samp = buf_ref[intermediate_buff_index]->mAudioData;
		
	    printf("AUDIO_QueueBuffer: index=%d buff=%x inbuff=%x  bytes=%d ", 
									intermediate_buff_index, 
									AudioBuffers[intermediate_buff_index], 
									mBuffer, mSize ); 

		memcpy( &(AudioBuffers[intermediate_buff_index]),  mBuffer,  mSize );
		buf_ref[intermediate_buff_index]->mAudioDataByteSize = mSize;  		// AUDIO_BUFF_SIZES;

		printf("copied!");
		status = AudioQueueEnqueueBuffer( queue, buf_ref[intermediate_buff_index], 0, NULL );
		printf( "\nEnqueue status: %d\n", status );
	} else {
		int index = (intermediate_buff_index % BUFFERS);
	    memcpy( &(AudioBuffers[index]), mBuffer, mSize );
	}
   	intermediate_buff_index++;   		
	printf("\n"); */
}

/************************************************
Close Player - 
************************************************/
void AUDIO_ClosePlayer()
{
/*  int i=0;
  AudioQueueStop( queue, TRUE );
  for (i=0; i<BUFFERS; i++)
	  AudioQueueFreeBuffer( queue, buf_ref[i] );
  AudioQueueDispose( queue, TRUE );
  
  audio_hardware_enabled = FALSE;
  PlaybackStarted 		 = FALSE;
  printf("--Mac AUDIO HARDWARE SHUTDOWN!\n");  */
}

/*******************************************************************/
/**************** AUDIO RECORD FUNCTIONS ***************************/
/*******************************************************************/
//AudioQueueRef 	rqueue;
BOOL 	RecordStarted 	= FALSE;
BOOL 	RemoteListener = FALSE;
//char*  	rAudioBuffers[BUFFERS][ONE_SECOND_HIFI_SIZE]; 

/******************************************************************
 Called when a buffer is available from the Record Queue 
 *****************************************************************/
/*void record_callback(void *ptr, AudioQueueRef rqueue, AudioQueueBufferRef buf_ref)
{
  OSStatus status;
  AudioQueueBuffer *buf = buf_ref;
  int	 osize = 0;
  int    nsamp = buf->mAudioDataByteSize / 2;
  short *samp  = buf->mAudioData;
  printf ("Callback! index=%d nbytes: %d  ", intermediate_cb_buff_index, buf->mAudioDataByteSize);

  // FILL DATA HERE:    
  // Only reason we'd record is to send it out over network.  
  // No.  We might get a request to record and save to HD only.  
  memcpy( &(rAudioBuffers[intermediate_cb_buff_index]), samp, buf->mAudioDataByteSize );
  if (SaveFileOpen)
  	AppendAudioData( &(rAudioBuffers[intermediate_cb_buff_index]), buf->mAudioDataByteSize );
  
  if (RemoteListener) {
	osize = Cmd_Audio_Data( oBuff, &(rAudioBuffers[intermediate_cb_buff_index]), buf->mAudioDataByteSize );		// Send to Client
	SendTelegram( oBuff, osize );
  	//printf ("Sending CMD_AUDIO_DATA: size=%d osize=%d\n", buf->mAudioDataByteSize, osize );
  }

  intermediate_cb_buff_index++;
  if ((intermediate_cb_buff_index % BUFFERS)==0)
      intermediate_cb_buff_index = 0;	// play back is slower than incoming data (loose a buffer)  

  // REQUEUE:
  status = AudioQueueEnqueueBuffer( rqueue, buf_ref, 0, NULL );
  if (status>0) printf ("Enqueue error status: %d\n", status ); 
}*/

/************************************************
 AUDIO_SetupRecorder()
 ************************************************/
void AUDIO_SetupRecorder(int mSampleRate)
{
/*	OSStatus 		status;
	int 			i;
	InitFormatParameters( 1, mSampleRate );
	AUDIO_init_hdr      ( 1, mSampleRate );
	int buff_size  = (mSampleRate * 2) / 10;

	// CONNECT TO OUTPUT DEVICE:
	status = AudioQueueNewInput(&fmt, record_callback, &phase, NULL,  
								 kCFRunLoopCommonModes, 0, &rqueue);

	for (i=0; i<BUFFERS; i++)
	{
		status      = AudioQueueAllocateBuffer( rqueue, buff_size, &buf_ref[i] );
		short *samp = buf_ref[i]->mAudioData;
		buf_ref[i]->mAudioDataByteSize = buff_size;

		printf("AUDIO_SetupRecorder: Buffer #%d \n", i);
		status = AudioQueueEnqueueBuffer( rqueue, buf_ref[i], 0, NULL );
		if (status>0)  printf( "Enqueue status: %d", status );

		if (status == kAudioFormatUnsupportedDataFormatError) puts ("oops - UnsupportedDataFormatError opening recorder!");
		else if (status>0) printf("NewInput status: %d\n", status);
	}

	// THIS ONLY NEEDS TO BE CALLED ONCE!  YEA!
	CFRunLoopRunInMode (
		kCFRunLoopDefaultMode,
		0.25, // seconds
		false // don't return after source handled
	); */
}

/*****************************************************************
Initialize the Wave Format structure
****************************************************************/
void AUDIO_StartRecord()
{
/*  OSStatus 	status;
  status = AudioQueueStart( rqueue, NULL );
  RecordStarted = TRUE;
  printf("--AUDIO_StartRecord: ");
  if (status>0)  printf(" error status=%d", status);
  printf("\n"); */
}

/************************************************
Close Recorder()
************************************************/
void AUDIO_CloseRecorder()
{
/*	int i=0;
	AudioQueueStop( rqueue, TRUE );
	for (i=0; i<BUFFERS; i++)
		AudioQueueFreeBuffer( rqueue, buf_ref[i] );
	AudioQueueDispose( rqueue, TRUE );
	
	audio_hardware_enabled = FALSE;
	RecordStarted 		   = FALSE;
	int osize = Cmd_Audio_End( oBuff );
	SendTelegram( oBuff, osize );
	printf("--Mac AUDIO HARDWARE SHUTDOWN!\n");  */
}


/*******************************************************************/
/************** MISCELLANEOUS FUNCTIONS ****************************/
/*******************************************************************/
void fillBuffer( short* mBuffer, int mNumSamps, double freq )
{
  // FILL DATA HERE:
/*  int ii;
  for (ii=0; ii<mNumSamps; ii++)
  {
    mBuffer[ii] = (int) ( 30000.0 * sin(freq) );
    freq += phase_inc;
  } */
}

/******************************************************************
 Called when a buffer is available from the Play Queue (ie. one has 
 just finished playing.
 *****************************************************************
void callback_tones(void *ptr, AudioQueueRef queue, AudioQueueBufferRef buf_ref)
{
  OSStatus status;
  //PhaseBlah *p = ptr;
  AudioQueueBuffer *buf = buf_ref;
  int    nsamp = buf->mAudioDataByteSize / 2;
  short  *samp = buf->mAudioData;
  printf ("Callback! nsamp: %d\n", nsamp);

  // FREQUENCY WITH HYSTERSIS:
  if (up_down=='u')  {
  	mult *= 1.25;
  	if (mult>10)  { up_down = 'd';  mult *= 0.9; }
  }
  else {
	  mult *= 0.9;
	  if (mult<1) { up_down = 'u';  mult *= 0.9; }
  }

  // FILL DATA HERE:
  int ii;
  for (ii = 0; ii < nsamp; ii++) {
    samp[ii] = (int) (30000.0 * sin(phase * mult));
    phase += phase_inc;
  }
  
  // QUEUE:
  status = AudioQueueEnqueueBuffer (queue, buf_ref, 0, NULL);
  printf("Enqueue status: %d\n", status);  
}*/

