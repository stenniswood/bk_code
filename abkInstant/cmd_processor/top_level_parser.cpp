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

/* Sample sentences bkInstant would be able to respond to :
	What's your host name? what's your IP address?
	Start [your] camera show on [your display].
	Start [your] camera show on [my phone].
	
	Start [the] camera show on [my phone].
	Record [your] camera.
	Send camera video to harddrive.



	We're going to start simple.  Each command will be statements as above.
	Not the numerous other ways of saying something:
		
	Let me see your camera on my phone.
	Send your camera over to the basement TV.
	Robot watch the camera in the bed room.  Notify me As soon as he wakes up.
	

*/

/*
		case CMD_VIDEO_START	:	printf(" COMMAND VIDEO START "		);		break;
		case CMD_VIDEO_DATA		:	printf(" COMMAND VIDEO DATA "		);		break;		
		case CMD_VIDEO_END		:	printf(" COMMAND VIDEO END  "		);		break;
		
		case REQ_VIDEO_LIVE		:	printf(" REQUEST VIDEO LIVE "		);		break;
		case REQ_VIDEO_TIMESTAMP:	printf(" REQUEST VIDEO TIMESTAMP "	);		break;
		case REQ_VIDEO_FILE		:	printf(" REQUEST VIDEO FILE "		);		break;

		case CMD_GPIO:	printf(" COMMAND GPIO "	  	 	);	break;
		case CMD_DDR :	printf(" COMMAND DataDirection ");	break;
		case REQ_GPIO:	printf(" REQUEST GPIO "			);	break;


		case CMD_CAN_START	:	printf(" COMMAND CAN START " );		break;
		case CMD_CAN_MSG	:   printf(" COMMAND CAN MSG   " );		break;
		case REQ_CAN_LISTEN	:	printf(" REQUEST CAN 	   " );		break;	





*/

