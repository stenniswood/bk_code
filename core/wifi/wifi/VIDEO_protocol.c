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

/***********************************************************************
All Incoming Video will also be saved to the hard-drive on those 
systems with enough capacity.  This allows the user to call back anything
in its memory banks.  Think star trek logs.  "There were two people outside
your side door yesterday...    Can you replay it?  
What was the topic of it?  How long was it?  A smart robot could even
use the info to answer questions.  On Sept 20th, you said you wanted to
ignore all upgrade notices.
***********************************************************************/

/*****************************************************************
Initialize internal data structures for CAN protocol (protocol only, 
not all the CAN driving code):
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Init_VIDEO()
{
}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Print_VIDEO_Acknowledgement(UINT mToken)
{
	switch( mToken )
	{
		case CMD_VIDEO_START	:	printf(" COMMAND VIDEO START "		);		break;
		case CMD_VIDEO_DATA		:	printf(" COMMAND VIDEO DATA "		);		break;		
		case CMD_VIDEO_END		:	printf(" COMMAND VIDEO END  "		);		break;
		
		case REQ_VIDEO_LIVE		:	printf(" REQUEST VIDEO LIVE "		);		break;
		case REQ_VIDEO_TIMESTAMP:	printf(" REQUEST VIDEO TIMESTAMP "	);		break;
		case REQ_VIDEO_FILE		:	printf(" REQUEST VIDEO FILE "		);		break;
		default:	break;
	}	
}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
BOOL Process_VIDEO_Telegrams( UINT  mToken, byte* mheader, int DataLength, int mconnfd )
{
		switch (mToken)
		{
			case CMD_VIDEO_START:				
				printf("%s \n", (char*)mheader);
				break;
			case CMD_VIDEO_DATA	:
				break;
			case REQ_VIDEO_LIVE	:
				break;
			case REQ_VIDEO_TIMESTAMP:			
				break;
			case REQ_VIDEO_FILE :
				break;
			default:  return FALSE; break;
		}
		return TRUE;
}

