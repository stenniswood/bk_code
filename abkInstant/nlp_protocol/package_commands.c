#include <stdio.h>
#include <string.h>
#include <stdint.h>
#if (PLATFORM==RPI)
#include "bcm_host.h"
#include "ilclient.h"
#endif

#include "bk_system_defs.h"
#include "protocol.h"
#include "devices.h"
#include "AUDIO_device.hpp"
#include "package_commands.h"


#define CAN_AVAILABLE 
#ifdef CAN_AVAILABLE
#include "CAN_Interface.h"
#endif


int package_short( BYTE* mBuffer, int mOffset, short mValue )
{
	mBuffer[mOffset  ] = (mValue & 0x00FF);
	mBuffer[mOffset+1] = (mValue & 0xFF00) >> 8;
}

int package_int( BYTE* mBuffer, int mOffset, int mValue )
{
	mBuffer[mOffset]   = (mValue & 0x000000FF);
	mBuffer[mOffset+1] = (mValue & 0x0000FF00) >> 8;
	mBuffer[mOffset+2] = (mValue & 0x00FF0000) >> 16;
	mBuffer[mOffset+3] = (mValue & 0xFF000000) >> 24;
}

int Cmd_GPIO( BYTE* mBuffer, BYTE mPin, boolean mHigh				)
{	
	// SETUP OBUFF:
	package_int( mBuffer, 0, CMD_GPIO); // Token
	package_int( mBuffer, 4, 2);		// DataLength
	*(mBuffer+8) = mPin; 
	*(mBuffer+9) = mHigh;
	return 10;
}

int Cmd_Shutdown( BYTE* mBuffer )
{	
	// SETUP OBUFF:
	package_int( mBuffer, 0, REQ_SHUT_DOWN); 	// Token
	package_int( mBuffer, 4, 0);		// DataLength
	return 8;
}

// COMMANDS:
int Cmd_CAN( BYTE* mBuffer, struct sCAN* Msg )
{
	package_int  ( mBuffer, 0, CMD_CAN_MSG );		// Token
	int length = 4 + Msg->header.DLC;
	package_int  ( mBuffer, 4, length );			// Length
	package_short( mBuffer, 8, Msg->id.group.id );
	mBuffer[10]= Msg->id.group.instance;
	mBuffer[11]= (Msg->header.DLC) | ((Msg->header.rtr)<<4);
	for (int i=0; i<Msg->header.DLC; i++)
		mBuffer[i+12] = Msg->data[i];
	return (8+length);	
}

int Cmd_CAN_Start( BYTE* mBuffer )	
{
	package_int( mBuffer, 0, CMD_CAN_START );	// Token
	package_int( mBuffer, 4,  0 );				// Length
	return 8;
}

int Cmd_Picture( BYTE* mBuffer, BYTE* mImage, int mWidth, int mHeight )
{}


/*************************************************************************
* Start of Audio data transfer telegram.  
* mBuffer	- the telegram output buffer.
* mHdr		- the wave header structure for the audio format
* mTransferBuffSize - The buffer sizes that will be coming on CMD_AUDIO_DATA telegrams.
***************************************************************************/
int Cmd_Audio_Start( BYTE* mBuffer, void* mHdr, short mTransferBuffSize ) 
{
	int swh = sizeof(struct WAVE_HEADER);

	package_int( mBuffer, 0, CMD_AUDIO_START );		// Token	
	package_int( mBuffer, 4, 12+swh );				// Length
	*((char*)(mBuffer+8))  = 'f';	*((char*)(mBuffer+9))  = 'm';	// 'fmt '
	*((char*)(mBuffer+10)) = 't';	*((char*)(mBuffer+11)) = ' ';
	package_int( mBuffer, 12, 16);				// chunk size
	memcpy( (mBuffer+16), mHdr, swh );			// wave header 
	package_int( mBuffer, 16+swh, mTransferBuffSize);					// future buffer transfer sizes	
	return 20+swh;
}
/*************************************************************************
* Audio data transfer telegram.  New Audio data in.
* mBuffer	- the telegram output buffer.
* mData		- the wave header structure for the audio format
* mLengthBytes - The buffer sizes that will be coming on CMD_AUDIO_DATA telegrams.
***************************************************************************/
int Cmd_Audio_Data( BYTE* mBuffer, short* mData, int mLengthBytes ) 
{
	package_int( mBuffer, 0,     CMD_AUDIO_DATA);		// Token
	package_int( mBuffer, 4,     mLengthBytes );		// Length
	memcpy( (mBuffer+8),  mData, mLengthBytes );		// 
	return mLengthBytes+8;
}

/*************************************************************************
* End of Audio data transfer telegram.  Done.
* mBuffer	- the telegram output buffer.
***************************************************************************/
int Cmd_Audio_End( BYTE* mBuffer )
{
	package_int( mBuffer, 0, CMD_AUDIO_END);	// Token 	
	package_int( mBuffer, 4,  0 );				// Length 
	return 8;
}

int Cmd_Text_Msg( BYTE* mBuffer, char*  mMessage							 )
{}



/*************************************************************************
 ***  REQUESTS:
 ************************************************************************/
int Req_GPIO_cmd( BYTE* mBuffer, BYTE mPin, boolean mHigh)
{
	// SETUP OBUFF:
	package_int( mBuffer, 0, REQ_GPIO );	// Token
	package_int( mBuffer, 4,  1 );			// DataLength
	*(mBuffer+4) = mPin; 
	return 9;	
}


int Req_CAN_Listen( BYTE* mBuffer )
{
	package_int( mBuffer, 0, REQ_CAN_LISTEN );	// Token
	package_int( mBuffer, 4, 0 );				// Length
	return 8;
}

int Req_CAN( BYTE* mBuffer, BYTE mPin, boolean mHigh				)
{}
int Req_CAN_mask( BYTE* mBuffer, BYTE mPin, boolean mHigh			)	
{}
int Req_Picture( BYTE* mBuffer, BYTE* mImage, int mWidth, int mHeight )
{}
int Req_Audio   ( BYTE* mBuffer, short* mData, int mChannels, int mLength )
{}
int Req_Text_Msg( BYTE* mBuffer, char*  mMessage					)
{}

