
/*  WARNING: This serverthread.c is Very different from the one in abkInstant
	The one in ../core/wifi/serverthread.c is Token based
	This one is simple NLP words based and is the current choice.
*/
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <time.h> 
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <sys/ioctl.h>
#include "bk_system_defs.h"
#include "CAN_base.h"
#include "CAN_memory.h"
#include "CAN_util.h"
#include "AUDIO_interface.hpp"
#include "protocol.h"

#include "serverthread.hpp"
#include "GENERAL_protocol.hpp"
#include "CAN_protocol.hpp"
#include "HMI_protocol.hpp"
#include "GPIO_protocol.hpp"
#include "FILE_protocol.hpp"
#include "AUDIO_protocol.hpp"
#include "CAMERA_protocol.hpp"
#include "visual_memory.h"
#include "bk_system_defs.h"
#include "client_memory.hpp"


#define Debug 0

static char 	ip_addr[16]; 			// for user feedback

/********************************************************************
Init - 
Retrieves the IP address for en1 into file variable : ip_addr
********************************************************************/
static void get_ip_address()
{
    // DISPLAY THE IP ADDRESS OF THIS MACHINE:
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    getifaddrs(&ifAddrStruct);
    
	// Go thru each (device/connection) IP address (ie. wifi, ethernet - en0, en1, etc.)
    for (ifa = ifAddrStruct; ifa != NULL;  ifa=ifa->ifa_next)
    {
        if (ifa->ifa_addr->sa_family==AF_INET) 	// check it is IP4
        {
            // is a valid IP4 Address
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("%s IP Address %s \n", ifa->ifa_name, addressBuffer );

            // Save the en1 connection : 
            int result = strcmp(ifa->ifa_name, "en1");
            if (result==0) 
            {
        		struct sockaddr*  sa2 = ifa->ifa_broadaddr;
            	strcpy ( ip_addr, addressBuffer );            	            	
            	printf(" Broadcast Address = %s\n", ifa->ifa_broadaddr->sa_data );
            	// inet_ntop(AF_INET, tmpAddrPtr, broadcast_addr, INET_ADDRSTRLEN);
            	// strcpy ( broadcast_addr, addressBuffer );            	
            }
        } else if (ifa->ifa_addr->sa_family==AF_INET6) 	// check it is IP6
        { 
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 	
        } 
    }
    if (ifAddrStruct!=NULL) 
    	freeifaddrs(ifAddrStruct);
 	// END OF DISPLAY IP ADDRESS
}


void establish_connection()
{
	/****************************
	   loop on select() or poll().  These will inform when a connection is pending,
	   then we call accept().  Since select()/Poll() are nonblocking, we can
	   also check for connect_to_robot() client initiated request.
	*/
	dprintf("establish_connection()\n");	
    socklen_t size = (socklen_t)sizeof(client_addr);	
    memset(&client_addr, '0', sizeof(socklen_t) );
	
	struct timeval tv;
	tv.tv_sec = 10;		// want immediate return
	tv.tv_usec = 0;

	int connection_pending = 0;
	//int max_fds = max( listenfd, connfd )+1;
	int max_fds = listenfd+1;

	while (!connection_established)
	{
		FD_ZERO(&rfds);		
		FD_SET(listenfd, &rfds);

		tv.tv_sec  = 0;		// want immediate return
		tv.tv_usec = 0;		
		
		// CHECK FOR AN INCOMING CONNECTION TO ACCEPT:
		int number_active = select( max_fds, &rfds, NULL, NULL, &tv );
		if (number_active==-1)
			printf("%s \n", strerror(errno));

		connection_pending = FD_ISSET(listenfd, &rfds);
		if (number_active>0)
			printf("select() = %d; pending=%d\n", number_active, connection_pending);

		if (connection_pending) {
			connfd = accept(listenfd, (struct sockaddr*)&client_addr, &size ); 
			printf("connection accepted!\n");
			connection_established = TRUE;

				// For Client Status purpose, let them know we are connected.
				//strcpy (NLP_Response, "Connected, welcome.");
				//printf("RESPONSE: %s\n", NLP_Response );
				//Send_Reply();
		}
		else 
			// ALTERNATELY:  Client memory requested we be the client, initiate connection : 
			if (REQUEST_client_connect_to_robot)
			{
				connection_established = !(connect_to_robot( REQUESTED_client_ip ));
				REQUEST_client_connect_to_robot = FALSE;
				
			}
	}
	//dprintf("Connection established \n");
}