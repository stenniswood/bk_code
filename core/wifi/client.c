/************************************************************************	
This establishes a connection to another bkInstant media center. via the 
nlp port.  This allows sending voice commands to a "robot".  

In test right now is:
incoming file for you from 192.168.1.144:
*************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include "bk_system_defs.h"
#include "package_commands.h"
#include "client.h"
#include "CAN_memory.h"
#include "protocol.h"  // for dumpbuffer()
#include "serverthread.h"


//#define BK_MEDIA_PORT 8080
#define BK_MEDIA_PORT 5000

#define OUTPUT_BUFFER_SIZE 255
BYTE 	coBuff[OUTPUT_BUFFER_SIZE];
BYTE 	i;
int 	bytes;

// Turn CAN rebroadcasting ON/OFF with this var
//BOOL 	TransportCAN = TRUE;
BOOL 	WifiConnected = FALSE;

/* This will establish a connection. The robot using this will thus be a client.
	This could be a TV monitor or an actual robot, or...
	What data is sent depends on : 
		User's intention (human initiated)  - or -
		Autonomous robot (intention)
			This could be :  an emergency situation (detected intruder, fire, cat problem, etc)
			Or daily periodic download of system tasks completed, etc.
			or trouble code diagnostic info,
			or request for info, map update, etc.
			or request contact with another robot for whatever personal reason it has!

	More specific for right now this will be:
		TV monitor app automatically wishes to connect to screenless robot for receiving CAN traffic
		So we establish a connection and insert a message transporter to the CAN_isr()
*/

/* This accessor function will be used in the CAN_isr() */
int are_exporting_CAN()
{
	return (CAN_SendingOn & WifiConnected);
}
int are_importing_CAN()
{
	return (CAN_ListeningOn & WifiConnected);
}

int  	sockfd=0;
char 	recvBuff[1024];


int send_file_transmit_request()
{
	printf("send_file_transmit_request:  \n");
	std::string msg = "incoming file for you from 192.168.1.144:";
	bytes = write(sockfd, msg.c_str(), msg.length() );	
	printf("%d bytes written\n",bytes);
}

int client_send_buffer()
{
	//bytes = write(sockfd, coBuff, length );	
}

int Cmd_client_CAN_Start( )
{
	CAN_SendingOn = TRUE;
	printf("Cmd_client_CAN_Start:  CAN message stream enabled...\n");
	//int length = Cmd_CAN_Start( coBuff ); 
	//DumpBuffer( coBuff, length );
	
	strcpy( (char*)coBuff, "incoming CAN" );
	int length = strlen( (char*)coBuff );
//	bytes = write(sockfd, coBuff, length );
	SendTelegram( coBuff, length);
	printf("Cmd_client_CAN_Start:  %d bytes written\n", bytes );
	return bytes;
}

int Cmd_client_CAN_listen( )
{
	CAN_ListeningOn = TRUE;

	//printf("Cmd_client_CAN_listen:  Rx CAN msg stream...\n");

	strcpy (coBuff, "send CAN");
	int length = strlen(coBuff);
	//DumpBuffer  ( coBuff, length );
	SendTelegram( coBuff, length);
	printf("Cmd_client_CAN_listen:  %d bytes written\n", length );
	return length;
}

int Cmd_client_send_CAN( struct sCAN* Msg )
{
	printf("Cmd_client_send_CAN:  Transporting CAN message...\n");	
	//int length = Cmd_CAN( coBuff, Msg );
	//DumpBuffer( coBuff, length );

	sprint_message( (char*)coBuff, Msg ); 
	int length   = strlen((char*)coBuff);
//	bytes = write(sockfd, coBuff, length );
	SendTelegram( coBuff, length);
	printf("Cmd_client_send_CAN:  %d bytes written\n", bytes);	
	return bytes;
}

int Cmd_client_CAN_Stop( )
{
	printf("Cmd_client_CAN_Stop:  Transporting CAN message...\n");
	//int length = Cmd_CAN_Start( coBuff );		// see ../core/wifi/package_commands.c
	//DumpBuffer( coBuff, length );
	strcpy ((char*)coBuff, "stop CAN");
	int length = strlen( (char*)coBuff );
//	bytes = write(sockfd, coBuff, length );
	SendTelegram( coBuff, length);
	printf("Cmd_client_CAN_Stop:  %d bytes written\n", bytes );
	return bytes;
}

int wait_for_ack()
{
	int n=0;
    memset(recvBuff, '0',sizeof(recvBuff));	
	// WAIT FOR ACK/NACK:
    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    }
    if(n < 0)
    {
        printf("\n Read error \n");
    } 
}

int disconnect_from_robot()
{
	printf("\nShutdown... ");
	Cmd_Shutdown( coBuff );
	bytes = write(sockfd, coBuff, 4 );
	wait_for_ack();
	close(sockfd);
}
