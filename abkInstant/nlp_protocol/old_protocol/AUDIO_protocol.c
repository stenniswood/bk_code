#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include "protocol.h"
#include "devices.h"
#include "utilities.h"
#include "package_commands.h"


/***********************************************************************
All Incoming Audio will also be saved to the hard-drive on those 
systems with enough capacity.  This allows the user to call back anything
in its memory banks.  Think star trek logs.  "There was an incoming 
message from so and so on October 31 2006".  Can you replay it?  
What was the topic of it?  How long was it?  A smart robot could even
use the info to answer questions.  On Sept 20th, you said you wanted to
ignore all upgrade notices.
***********************************************************************/
char AUDIO_PATH[] = "./AUDIO/";
int  transfer_size = 3844;			// default
int  audio_buffers_received = 0;

/*****************************************************************
Initialize internal data structures for CAN protocol (protocol only, 
not all the CAN driving code):
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Init_AUDIO()
{		
	audio_buffers_received = 0;
}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Print_AUDIO_Acknowledgement(UINT mToken)
{
	switch( mToken )
	{
		case CMD_AUDIO_START	:	printf(" COMMAND OPEN AUDIO "			);	break;
		case CMD_AUDIO_DATA		:	printf(" COMMAND AUDIO DATA " 			);	break;
		case CMD_AUDIO_END 		:	printf(" COMMAND AUDIO END " 			);	break;
		case REQ_AUDIO_CAPABILITIES: printf(" REQUEST AUDIO CAPABILITIES "	);	break;
		case REQ_AUDIO_LIVE		:	printf(" REQUEST LIVE AUDIO "			);	break;		
		case REQ_AUDIO_STOP		:	printf(" REQUEST LIVE AUDIO STOP "		);	break;		
		case REQ_AUDIO_TIMESTAMP:	printf(" REQUEST AUDIO FROM TIMESTAMP"	);	break;		
		case REQ_AUDIO_FILE		:	printf(" REQUEST AUDIO FROM FILE "		);	break;
		default:	break;
	}
}
/*****************************************************************
Show contents of buffer on the console.  Usefull for debugging.
*****************************************************************/
void DumpBuffer( BYTE* mbuff, int mlength)
{
	int i;
	printf("\nBuffer: %d bytes", mlength);
	for (i=0; i<mlength; i++)
	{
		if ((i%8)==0)
			printf("\n");
		printf(" %2x", *(mbuff+i) );
	}
	printf("\n");
}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
BOOL Process_AUDIO_Telegrams( UINT  mToken, byte* mheader, int DataLength, int mconnfd )
{
		struct DEVICE_CAPS* dc;
		BOOL retval = FALSE;
		int samples = 0;
		float seconds = 0.0;
		int   osize = 0;
		char* fn = NULL;
		char* Name = NULL;
		
		switch (mToken)
		{
			case CMD_AUDIO_START:
				//printf(" Incoming Audio Header: %d \n", DataLength );
				AUDIO_init_hdr( *((WORD*)(mheader+10)), *((int*)(mheader+12)) );
/*				audio_hdr.format = 1;
				audio_hdr.channels_rxd = *((WORD*)(mheader+10));
				audio_hdr.block_align  = audio_hdr.channels_rxd * 2;
				audio_hdr.sample_rate  = *((int*)(mheader+12));
				audio_hdr.byte_rate    = audio_hdr.sample_rate * 2; */
				transfer_size = *((WORD*)(mheader+22));		// size of chunks
				printf("\tChannels= %d\n", audio_hdr.channels_rxd );
				printf("\tSampleRate= %d\n", audio_hdr.sample_rate  );
				printf("\tTransfer Size (bytes)= %d\n", transfer_size  );
				fn = GetNewAudioFileName(FALSE);
				OpenAudioFile( fn );
				printf("done with OpenAudioFile");
				if ( audio_hardware_enabled == FALSE )
					AUDIO_SetupPlayer(audio_hdr.channels_rxd, audio_hdr.sample_rate );
				retval = TRUE;
				audio_buffers_received = 0;
				break;
			case CMD_AUDIO_DATA	:
				samples = DataLength/audio_hdr.channels_rxd/2;
				seconds = ((float)samples) / ((float)audio_hdr.sample_rate);
				//printf("%d samples (%4.2f seconds)\n", samples, seconds );
				audio_buffers_received++;
				AppendAudioData( mheader, DataLength );
				//printf ("done with AUDIO_DATA telegram\n");
	
				// SEND TO AUDIO PLAYBACK HARDWARE:				
				if (audio_hardware_enabled) {
					AUDIO_QueueBuffer( mheader, DataLength );
					if ((audio_buffers_received == 2))
						AUDIO_StartPlay();			
				}
				//printf ("done with AUDIO_DATA telegram\n");
				retval = TRUE;
				break;
			case CMD_AUDIO_END : 
				printf(" Closing Audio file...\n");
				AUDIO_ClosePlayer();
				CloseAudioFile();
				break;				
								
			case REQ_AUDIO_CAPABILITIES : 
				dc = DEVICE_ComposeAudioCapabilitiesStructure();
				retval = TRUE;
				// Send data back to client
				break;
			case REQ_AUDIO_LIVE:			// Client wants to listen to this device.				
				if (RecordStarted)
					AUDIO_CloseRecorder();						
				Name   = GetNewAudioFileName( TRUE );
				SaveFileOpen = OpenAudioFile( Name );

				AUDIO_init_hdr( 1, 22050 );		// tailor to be max capability or save in an .ini preferences file.				
				AUDIO_SetupRecorder( 22050 );
				AUDIO_StartRecord  ( );		

				// Send to client:
				osize = Cmd_Audio_Start( oBuff, &audio_hdr, AUDIO_BUFF_SIZES );
				printf("Sample Rate = %d, Channels=%d\n", audio_hdr.sample_rate, audio_hdr.channels_rxd);
				printf("Audio buffer size %d\n" , AUDIO_BUFF_SIZES );
				SendTelegram( oBuff, osize );
				//DumpBuffer  ( oBuff, osize );
				RemoteListener = TRUE;
				retval = TRUE;
				break;
			case REQ_AUDIO_STOP:			// Client wants to listen to this device.				
				if (RecordStarted)
				{
					AUDIO_CloseRecorder();	
					CloseAudioFile();
					RemoteListener = FALSE;
				} else {
					printf(" Error:  Recording not started! ");
					// Should send a NACK back.
				}
				retval = TRUE;
				break;

			case REQ_AUDIO_TIMESTAMP:		// Client wants to listen to this device.
				retval = TRUE;
				break;
			case REQ_AUDIO_FILE:			// Client wants to listen to this device.
				retval = TRUE;
				break;
			default:  break;
		}
		return retval;
}

