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
#include "bk_system_defs.h"
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
void Init_IMAGE()
{
}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Print_IMAGE_Acknowledgement(UINT mToken)
{

}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
BOOL Process_IMAGE_Telegrams( UINT mToken, byte* mheader, int DataLength, int mconnfd )
{
		/* The need for an FILE telegram is questionable.  We could have the client
			send us the image as a file.  Then push some HTML code which this would 
			open a window and display.  Since a "live" image would really be a live
			video.  What we might be looking for is:  requesting the robot to send us
			a snapshot.  We send a message to the robot's server side with a token.
			The server responds by sending us the picture and html.

			Now take the perspective that this is the robot.  We need to accept a token 
			REQ_FILE_LIVE and respond with CMD_FILE_TRANSFER (picture & html), 
			CMD_SHOW_HTML_FILE.
			
			Notice this wouldn't work with audio/video since it is continual.
		*/
}

