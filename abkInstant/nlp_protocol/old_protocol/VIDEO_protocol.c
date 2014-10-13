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
void Parse_VIDEO_Acknowledgement(UINT mToken)
{

}

/*****************************************************************
Do the work of the Telegram :
We might have multiple actions such as:  start and record
So:   list<char*> mAction

Adjectives may be black & white.  Low res, high res, HDMI,

return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
BOOL Perform_VIDEO_actions( char* mAction, char* mAdjectives, char* mObjects )
{
	if (strcmp(mAction, "show") == 0)
	{
		// If it's [your] display/screen/tv/etc, 
		//if (strcmp(mObject, "HDMI"))		
		
		// all others will need to send over socket.
		
	}
	else if (strcmp(mAction, "record") == 0)
	{
		// file name expected	
	} else if (strcmp(mAction, "play") == 0)
	{
		// file name expected
	}
	else if (strcmp(mAction, "stop") == 0)	// synonym cancel
	{
	
	}
	
}

