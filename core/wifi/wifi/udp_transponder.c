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
#include "udp_transponder.h"

#define Debug 0

//----- Defines ---------------------------------------------------------------
#define  PORT_NUM           1050  // Port number used

void* udp_transponder_function(void* msg)
{ 
  int                  client_s;        // Client socket descriptor
  struct sockaddr_in   server_addr;     // Server Internet address
  int                  addr_len;        // Internet address length
  char                 out_buf[4096];   // Output buffer for data
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

  // Assign a message to buffer out_buf
  char name[255];
  gethostname(name, 255);
  if (Debug) sprintf(out_buf, "BK robot server:IP Address=%s\n", name);

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
  char                 in_buf [4096];    // Input buffer for data
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
	
	  // Filter out our own broadcast - not interested in it!
	  char name[255];
	  gethostname(name, 255);	  
	  //if (name == )
	  
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
