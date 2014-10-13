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
#ifdef RPI_GPIO
#include <wiringPi.h>
#else
#define INPUT 0
#define OUTPUT 1
#define PWM_OUTPUT 2
#define GPIO_CLOCK 3
#define LOW 0
#define HIGH 1
#endif

#include "bk_system_defs.h"
#include "protocol.h"

#define MAX_GPIO_PINS 10
BYTE PinModes[MAX_GPIO_PINS];

/*****************************************************************
Initialize internal data structures:
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Init_GPIO()
{
	int i;
	for (i=0; i<MAX_GPIO_PINS; i++)
		PinModes[i] = INPUT;
}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Print_GPIO_Acknowledgement(UINT mToken)
{
	switch( mToken )
	{
		case CMD_GPIO:	printf(" COMMAND GPIO "	  	 	);	break;
		case CMD_DDR :	printf(" COMMAND DataDirection ");	break;
		case REQ_GPIO:	printf(" REQUEST GPIO "			);	break;
		default:	break;
	}
}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
BOOL Process_GPIO_Telegrams( UINT  mToken, byte* mheader, int DataLength, int mconnfd )
{
	BYTE Pin;
	BYTE Value;
	BOOL retval = FALSE;
		switch (mToken)
		{
			case CMD_GPIO:	// Change the output value for the pin
					  Pin   = *(mheader);
					  Value = *(mheader+1);
					  printf(" PIN %d=%d", Pin, Value );
					  #ifdef RPI_GPIO					  
						  if (PinModes[Pin]==OUTPUT)		// if not configured, do nothing.
						  	digitalWrite( Pin, Value);
					  #endif
					  retval = TRUE;
					  break;
			case CMD_DDR:	// Change Data Direction for the pin
					  Pin   = *(mheader);
					  Value = *(mheader+1);
					  if (Value==0)	{
					  	printf(" PIN %d : INPUT\n", Pin, Value );
						#ifdef RPI_GPIO
						  pinMode( Pin, INPUT);
						#endif
					  } else {
						printf(" PIN %d : OUTPUT\n", Pin, Value );
						#ifdef RPI_GPIO
						  pinMode( Pin, OUTPUT);						
						#endif						
					  }
					  retval = TRUE;
					  break;
			case REQ_GPIO:	// Reads an input GPIO
					  Pin = *(mheader);  
					  printf(" REQ PIN %d=%d\n", Pin, Value );
					  if (PinModes[Pin]==INPUT)		// if not configured, do nothing.					  
					  #ifdef RPI_GPIO					  
					    Value = digitalRead( Pin );
					  #endif
					 // Now we have to transmit the result back.
					 // ********************
					  retval = TRUE;
					 break;
			default:  break;
		}
	return retval;
}

//jesus page has a lot more punishing me to do.  he's not done pounding yet.
// the more he pounds the worse it gets it seems.
// he don't like the way i'm avoiding mommy either.
// he don't like me sleeping in.  
// he don't like me not applying for a job.  tells me it will make me poor.
// it's hard to like jesus page.
