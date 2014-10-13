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
#include <string>
#include <list>
#include "protocol.h"
#include "devices.h"
#include "GENERAL_protocol.h"
#include "GPIO_protocol.h"
#include "GPIO_device.h"


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
void Init_GPIO()
{

}

int extract_pin_number( std::string* mSentence )
{
	// find first [0..9] after either "GPIO" or "pin"	
}
			
/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Parse_GPIO_Acknowledgement( sObject* mSubject, sObject* mVerb, 
								 sObject* mObject, std::string* mSentence )
{
	int pin = extract_pin_number( mSentence );
	
	if (strcmp( mSubject->name.c_str(), "GPIO")==0)
	{
		if (strcmp(mVerb->name.c_str(), "set") ==0)
		{
			if (strcmp(mObject->name.c_str(), "input") ==0)
			{
				set_GPIO_DDR( pin, TRUE );
			}
			if (strcmp(mObject->name.c_str(), "output") ==0)
			{
				set_GPIO_DDR( pin, FALSE );
			}
			if ((strcmp(mObject->name.c_str(), "1") ==0)
			||  (strcmp(mObject->name.c_str(), "high") ==0)
			||  (strcmp(mObject->name.c_str(), "on") ==0))			
			{
				set_GPIO( pin, 1 );
			}
			if ((strcmp(mObject->name.c_str(), "0") ==0)
			||  (strcmp(mObject->name.c_str(), "low") ==0)
			||  (strcmp(mObject->name.c_str(), "off") ==0))			
			{
				set_GPIO( pin, 0 );			
			}
		}
		if ((strcmp(mVerb->name.c_str(), "read")==0)
		||  (strcmp(mVerb->name.c_str(), "what is")==0))
		{
			int value = read_GPIO( pin );
		}
	}
}

/*****************************************************************
Do the work of the Telegram :
We might have multiple actions such as:  start and record
So:   list<char*> mAction

Adjectives may be black & white.  Low res, high res, HDMI,

return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/

