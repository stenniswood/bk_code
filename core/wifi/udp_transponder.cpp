#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>    // Needed for sockets stuff
#include <netinet/in.h>   // Needed for sockets stuff
#include <sys/socket.h>   // Needed for sockets stuff
#include <arpa/inet.h>    // Needed for sockets stuff
#include <fcntl.h>        // Needed for sockets stuff
#include <netdb.h>        // Needed for sockets stuff
#include "udp_transponder.hpp"
#include "bk_system_defs.h" 
#include "client_memory.hpp"

#define Debug 0


//---------------------- CLIENT LIST --------------------------------------------
using namespace std;
struct stBK_Header 
{
	char ip_address[40];	
	char hostname[40];
	char machine_type[40];
	struct in_addr addr;
};

list<struct stBK_Header> beacon_ip_list;


int already_added( struct in_addr* mIP )
{
	struct in_addr tmp;
	list<struct stBK_Header>::iterator i = beacon_ip_list.begin();	
	while ( i != beacon_ip_list.end() )
	{
		tmp = (*i).addr;
				//printf(" IP=%s  \n",  inet_ntoa(*i) );		
		if (tmp.s_addr == mIP->s_addr)
			return 1;
		i++;
	}
	return 0;
}

// Print an informational message of each beacon IP address and port.
void print_beacons()
{
	printf("============= Beacons =====================\n");
	int c=0;
	list<struct stBK_Header>::iterator i = beacon_ip_list.begin();
	while ( i != beacon_ip_list.end() )
	{
		c++;
//		printf("#%d IP=%s  \n", c, inet_ntoa( (*i).addr) );		
		printf("#%d IP=%s; %s  %s  \n", c, (*i).ip_address, i->hostname, i->machine_type );				
		i++;
	}
	printf("==========================================\n");
}

int convert_to_string_array( char* mText )
{
	int count = 1;
	char* ch = strchr( mText, '\n' );

	while (ch != NULL)
	{
		*ch = 0;
		count++;
		ch = strchr( ch+1, '\n' );
	}	
	return count;
}

void  handle_client_list( struct in_addr* client_ip_addr, char in_buf[] )
{
  struct stBK_Header  hdr;
  if ( !already_added( client_ip_addr ) )
  {
  	 printf("Adding IP...\n");  	 
	 int num_strings = convert_to_string_array( in_buf );	  
	 char* ptr = in_buf+strlen(in_buf)+1;		// skip to second string.		 

	  // EXTRACT HOST NAME
	  char* ptr2 = strchr( ptr, '=');
	  strcpy ( hdr.hostname, ptr2+1);

	  // EXTRACT MACHINE TYPE:	  
	  ptr = ptr2+strlen(ptr2)+1;				// skip to 3rd string.
	  ptr2 = strchr(ptr, '=');
	  strcpy( hdr.machine_type, ptr2+1 );
	  
	  // ADD IP ADDRESS:
	  char* ip = inet_ntoa(*client_ip_addr);
	  strcpy ( hdr.ip_address, ip );
	  hdr.addr = *client_ip_addr;
	  
	  
	  // Print an informational message of IP address and port of the client
	  // printf("IP address of client = %s  port = %d \n", inet_ntoa(client_ip_addr),
	  //	 ntohs(client_addr.sin_port));
	 beacon_ip_list.push_back( hdr );
	 update_client_list();	// put into IPC memory
	  print_beacons();
	  // Output the received message
	  printf("Msg: %s \n", in_buf);
  }
}

/* 
	Put the list<string> of clients into the ipc_memory.
//			char* ip_str = inet_ntoa((*iter).addr);	
*/
void update_client_list()
{	
	// Number of clients.
	if (ipc_memory_client)
	{	
		ipc_memory_client->NumberClients    = beacon_ip_list.size();
		list<struct stBK_Header>::iterator iter = beacon_ip_list.begin();
		int byte_array_size = 0;
		struct stClientData* ptr = ipc_memory_client->ClientArray;
		int i=0;
		
		// Now add each client as a string followed by a null terminator.  Hence a string array.
		while(( iter != beacon_ip_list.end()) && (byte_array_size<MAX_CLIENTS))
		{
			
			// Copy 1 client:
			strcpy( ptr[i].address,  iter->ip_address);			
			strcpy( ptr[i].name,  	 iter->hostname );
			strcpy( ptr[i].machine,  iter->machine_type );
			ptr[i].network = 1;		// TCP_IP
			
			// Adjust for next client:
			iter++;
			i++;
		}
	}
}
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
  server_addr.sin_addr.s_addr = inet_addr("255.255.255.255"); // Need this for Broadcast

  // Set socket to use MAC-level broadcast
  iOptVal = 1;
  iOptLen = sizeof(int);
  setsockopt(client_s, SOL_SOCKET, SO_BROADCAST, (char*)&iOptVal, iOptLen);
  if (Debug) printf("UDP Tx: Local IP=%s\tport=%d\n", inet_ntoa(server_addr.sin_addr), 
  									  ntohs(server_addr.sin_port) );
  compose_message();
  while (1)
  {
	  // Now send the message to server:
	  retcode = sendto(client_s, out_buf, (strlen(out_buf) + 1), 0,
					   (struct sockaddr *)&server_addr, sizeof(server_addr));
	  if (retcode < 0)
	  {
		printf("*** ERROR - sendto() failed \n");
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

	  
	  
	  // Now for each string, parse info:
	  //for (int i=0; i<num_strings; i++)
	  {
	  	
	  }
	  	  	  
	  // Copy the four-byte client IP address into an IP address structure
	  memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);
	  handle_client_list( &client_ip_addr, in_buf );

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
