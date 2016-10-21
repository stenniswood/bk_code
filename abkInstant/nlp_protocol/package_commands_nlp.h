
#ifdef  __cplusplus
extern "C" {
#endif

#include "global.h"

/*#define BYTE unsigned char
#define WORD unsigned short
#define boolean BYTE*/


/* defined in devices.h
struct WAVE_HEADER
{
	short 	format;				// PCM=1
	short 	channels_rxd;	
	int 	sample_rate;	
	int		byte_rate;	
	short 	block_align;		
	short 	bits_per_sample;		
};*/

// Commands:
int Cmd_GPIO    ( BYTE* mBuffer, BYTE mPin, boolean mHigh				);
int Cmd_Shutdown( BYTE* mBuffer );
int Cmd_CAN      ( BYTE* mBuffer, struct sCAN* Msg						);
int Cmd_CAN_Start( BYTE* mBuffer );
int Cmd_Picture  ( BYTE* mBuffer, BYTE* mImage, int mWidth, int mHeight 	);
int Cmd_Audio_Start( BYTE* mBuffer, void* mHdr, short mTransferBuffSize );
int Cmd_Audio_Data ( BYTE* mBuffer, short* mData, int mLengthBytes 		);
int Cmd_Audio_End  ( BYTE* mBuffer 								   		);
int Cmd_Text_Msg( BYTE* mBuffer, char*  mMessage						 );

// REQUESTS:
int Req_CAN_Listen( BYTE* mBuffer );
int Req_CAN	 	( BYTE* mBuffer, BYTE mPin, boolean mHigh				 );
int Req_CAN_mask( BYTE* mBuffer, BYTE mPin, boolean mHigh				 );	
int Req_Picture ( BYTE* mBuffer, BYTE* mImage, int mWidth, int mHeight 	 );
int Req_Audio   ( BYTE* mBuffer, short* mData, int mChannels, int mLength);
int Req_Text_Msg( BYTE* mBuffer, char*  mMessage						 );
void SendTelegram( BYTE* mBuffer, int mSize );


#ifdef  __cplusplus
}
#endif

