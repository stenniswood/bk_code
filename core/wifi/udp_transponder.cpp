#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>    // Needed for sockets stuff
#include <netinet/in.h>   // Needed for sockets stuff
#include <sys/socket.h>   // Needed for sockets stuff
#include <arpa/inet.h>    // Needed for sockets stuff
#include <fcntl.h>        // Needed for sockets stuff
#include <netdb.h>        // Needed for sockets stuff

#include "pican_defines.h"
#include "bk_system_defs.h" 
#include "client_memory.hpp"
#include "udp_transponder.hpp"


#define Debug 0
using namespace std;

// Move this to string_util.c !
/* Deliminator:  */
int convert_to_string_array( char* mText, char mDelim )
{
	int count = 1;
	char* ch = strchr( mText, mDelim );

	while (ch != NULL)
	{
		*ch = 0;
		count++;
		ch = strchr( ch+1, mDelim );
	}	
	return count;
}

//---------------------- CLIENT LIST --------------------------------------------
/* TODO:
	A)  Need timeouts on the beacons.  If they stop responding, for x amount of time
		(maybe 1 minute), then we have to clear them from the list - or set a bit 
		indicating inactive.
	B)  Let's only keep 1 list.  either the list immediately below, or the client memory 
		list, and the client_list_object.  So we have 3 lists right now!
	

*/

// Remove this:
//list<struct stBK_Header> beacon_ip_list;

/*  
Find if in the ipc_memory_client->ClientArray[]
	// deprecated beacon_ip_list:

Return TRUE => Already added.
	   FALSE => Not added.
*/
int is_already_added( char* mIP_str )
{
	struct in_addr tmp;
	if (ipc_memory_client)
	for (int c=0; c<ipc_memory_client->NumberClients; c++)
	{
		int result = strcmp (ipc_memory_client->ClientArray[c].address, mIP_str );		
		if (result==0)
			return 1;
	}
	return 0;
}

// Print an informational message of each beacon IP address and port.
void print_beacons()
{
	printf("============= Beacons =====================\n");
	if (ipc_memory_client)
	for (int c=0; c<ipc_memory_client->NumberClients; c++)
	{
		c++;
		printf("#%d IP=%s; %s  %s  \n", c, ipc_memory_client->ClientArray[c].address, 
							ipc_memory_client->ClientArray[c].name, 
							ipc_memory_client->ClientArray[c].machine );
	}
	printf("==========================================\n");
}

/*
	Extract info from the beacon text.
*/
struct stClientData*  extract_beacon_text( char* mip, char in_buf[] )
{
	static struct stClientData hdr;

  	 printf("Adding IP...\n");  	 
	 int num_strings = convert_to_string_array( in_buf );	  
	 char* ptr = in_buf+strlen(in_buf)+1;		// skip to second string.		 

	  // EXTRACT HOST NAME
	  char* ptr2 = strchr( ptr, '=');
	  strcpy ( hdr.name, ptr2+1);

	  // EXTRACT MACHINE TYPE:	  
	  ptr = ptr2+strlen(ptr2)+1;				// skip to 3rd string.
	  ptr2 = strchr(ptr, '=');
	  strcpy( hdr.machine, ptr2+1 );
	  
	  // ADD IP ADDRESS:
	  strcpy ( hdr.address, mip );
	  
	  return &hdr;
}

/* 
	Put the list<string> of clients into the ipc_memory.
//			char* ip_str = inet_ntoa((*iter).addr);	

void update_client_list()
{
	if (ipc_memory_client)
	{	
		cli_reset_client_list();
		list<struct stBK_Header>::iterator iter = beacon_ip_list.begin();
		int byte_array_size = 0;
		struct stClientData tmp;
		
		// Now add each client as a string followed by a null terminator.  Hence a string array.
		while(( iter != beacon_ip_list.end()) && (byte_array_size<MAX_CLIENTS))
		{			
			// Copy 1 client:
			strcpy( tmp.address,  iter->ip_address);
			strcpy( tmp.name,  	  iter->hostname  );
			strcpy( tmp.machine,  iter->machine_type );			
			strcpy( tmp.status,   "idle");				// want connected/notconnected with multi clients.
			
			tmp.network = 1;		// TCP_IP
			cli_ipc_add_new_client( &tmp );						
			iter++;
		}
	}
}
*/

//--------------------- END CLIENT LIST ---------------------------------------
//=============================================================================

//----- Defines ---------------------------------------------------------------
#define  PORT_NUM           1050  // Port number used

char  out_buf[4096];   		// Output buffer for data

void compose_message()
{
	// Assign a message to buffer out_buf
	char   name[255];
	gethostname(name, 255);	
	sprintf(out_buf, "APP_NAME=BK robot server\nHOST_NAME=%s\nMACHINE_TYPE=%s\n", name, MACHINE_TYPE );	
}


#include "serverthread.h"

void* udp_transponder_function(void* msg)
{ 
  int                  client_s;        // Client socket descriptor
  struct sockaddr_in   server_addr;     // Server Internet address
  int                  addr_len;        // Internet address length

  char                 in_buf[4096];    // Input buffer for data
  int                  retcode;         // Return code
  int                  iOptVal;         // Socket option value
  int                  iOptLen;         // Socket option length

  // Create a socket
  client_s = socket(AF_INET, SOCK_DGRAM, 0);
  if (client_s < 0)
  {
    printf("*** ERROR - socket() failed \n");
    exit(-1);
  }

  // Fill-in server socket's address information
  server_addr.sin_family = AF_INET;                 // Address family to use
  server_addr.sin_port   = htons(PORT_NUM);           // Port num to use
//server_addr.sin_addr.s_addr = inet_addr("255.255.255.255"); // Need this for Broadcast
  server_addr.sin_addr.s_addr = inet_addr("192.168.2.255"); // Need this for Broadcast
  printf ("Broadcast Address=%s\n", broadcast_addr );
  
  // Set socket to use MAC-level broadcast
  iOptVal = 1;
  iOptLen = sizeof(int);
  setsockopt(client_s, SOL_SOCKET, SO_BROADCAST, (char*)&iOptVal, iOptLen);
  if (Debug) printf("UDP Tx: Local IP=%s\tport=%d\n", 
  									inet_ntoa(server_addr.sin_addr), 
  									ntohs(server_addr.sin_port) );
  compose_message();
  while (1)
  {
	  // Now send the message to server:
	  retcode = sendto(client_s, out_buf, (strlen(out_buf) + 1), 0,
					   (struct sockaddr *)&server_addr, sizeof(server_addr));
	  int error = errno;
	  if (retcode < 0)
	  {
		printf("*** ERROR - sendto() failed %s \n", strerror(error));
		exit(-1);
	  }
	  sleep(1);
  }
  
  // Close all open sockets
  retcode = close(client_s);
  if (retcode < 0)
  {
    printf("*** ERROR - close() failed \n");
    exit(-1);
  }
  return NULL;
}


void* udp_receive_function(void* msg)
{
  int                  server_s;        // Server socket descriptor
  struct sockaddr_in   server_addr;     // Server Internet address
  struct sockaddr_in   client_addr;     // Client Internet address
  struct in_addr       client_ip_addr;  // Client IP address
  int                  addr_len;        // Internet address length
  char                 out_buf[4096];   // Output buffer for data
  char                 in_buf [4096];   // Input buffer for data
  int                  retcode;         // Return code

  //   - AF_INET is Address Family Internet and SOCK_DGRAM is datagram
  server_s = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_s < 0)
  {
    printf("*** ERROR - socket() failed \n");
    exit(-1);
  }

  // Fill-in my socket's address information
  server_addr.sin_family = AF_INET;                 // Address family to use
  server_addr.sin_port   = htons(PORT_NUM);         // Port number to use
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen on any IP address
  retcode = bind(server_s, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (retcode < 0)
  {
    printf("*** ERROR - bind() failed \n");
    exit(-1);
  }
  if (Debug) printf("UDP Rx: Local IP=%s\tport=%d \n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

  // Wait to receive a message from client
  while(1)
  {
	  //printf("Waiting for recvfrom() to complete... \n");
	  addr_len = sizeof(client_addr);
	  retcode  = recvfrom(server_s, in_buf, sizeof(in_buf), 0,
						  (struct sockaddr *)&client_addr, (socklen_t*)&addr_len);
	  if (retcode < 0)
	  {
		printf("*** ERROR - recvfrom() failed \n");
		exit(-1);
	  }
	  	  	  
	  // Copy the four-byte client IP address into an IP address structure
	  memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);
	  char* ip = inet_ntoa(client_ip_addr);	  
	  if ( !is_already_added( ip ) )
//	  if ( !is_already_added( &client_ip_addr ) )
	  {		
		struct stClientData*  hdr = extract_beacon_text( ip, in_buf );
		cli_ipc_add_new_client(  hdr );		
		//update_client_list();	// put into IPC memory
		print_beacons();
		// Output the received message
		printf("Msg: %s \n", in_buf);
	  }
	  	
	  // Filter out our own broadcast - not interested in it!
	  char name[255];
	  gethostname(name, 255);

	  // Print an informational message of IP address and port of the client
	  if (Debug) printf("UDP Rx: Client IP = %s\tport = %d) \n", inet_ntoa(client_ip_addr),
			 ntohs(client_addr.sin_port));
	
	  // Output the received message
	  if (Debug) printf("UDP Rx: Msg=%s \n", in_buf);
  }

  retcode = close(server_s);
  if (retcode < 0)
  {
    printf("*** ERROR - close() failed \n");
    exit(-1);
  }
  return NULL;  
}
