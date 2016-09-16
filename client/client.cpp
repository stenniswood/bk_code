/************************************************************************	
This establishes a connection to another bkInstant media center. via the 
nlp port.  This allows sending voice commands to a "robot".  

In test right now is : 
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
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

#define BOOL unsigned char
#define byte unsigned char
#define TRUE 1
#define FALSE 0

//#include "bk_system_defs.h"
#include "package_commands.h"
#include "client.h"

//#include "protocol.h"  // for dumpbuffer()

using namespace std;


//#define BK_MEDIA_PORT 8080
//
#define BK_MEDIA_PORT 5000

#define OUTPUT_BUFFER_SIZE 255
byte 	coBuff[OUTPUT_BUFFER_SIZE];
byte 	i;
int 	bytes;

// Turn CAN rebroadcasting ON/OFF with this var
BOOL 	TransportCAN = TRUE;
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

int  	sockfd=0;
char 	recvBuff[1024];

/* Return 1 => Error
		  0 => Okay */
// using the nlp port.  
int connect_to_robot(char *ip_address )
{
	//printf("\n\nConnecting to %s\n", ip_address);
    struct sockaddr_in	serv_addr; 
	struct in_addr      addr;
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(BK_MEDIA_PORT);

    if(inet_pton(AF_INET, ip_address, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_aton error occured\n");
        return 1;
    } 

	printf("Connecting to : %s:%d\n", ip_address, BK_MEDIA_PORT );
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 
	printf("ROBOT CLIENT Connected\n");
	
	// Set All LEDs
    return 0;
}

int send_file_transmit_request()
{
	printf("send_file_transmit_request:  \n");
	string msg = "incoming file for you from 192.168.1.144:";
	bytes = write(sockfd, msg.c_str(), msg.length() );	
	printf("%d bytes written\n",bytes);
}

int Cmd_client_CAN_Start( )
{
	printf("Cmd_client_CAN_Start:  Transporting CAN message...\n");
	int length = Cmd_CAN_Start( coBuff );
	//DumpBuffer( coBuff, length );
	bytes = write(sockfd, coBuff, length );
	printf("Cmd_client_CAN_Start:  %d bytes written\n", bytes );
	return bytes;
}

int Cmd_client_send_CAN( struct sCAN* Msg )
{
	printf("Cmd_client_send_CAN:  Transporting CAN message...\n");	
	int length = Cmd_CAN( coBuff, Msg );
	//DumpBuffer( coBuff, length );

	bytes = write(sockfd, coBuff, length );
	printf("Cmd_client_send_CAN:  %d bytes written\n", bytes);	
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
