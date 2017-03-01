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
#include "super_string.hpp"
#include "global.h"
#include "serverthread.hpp"
#include "visual_memory.h"
#include "GENERAL_protocol.hpp"
#include "simulator_memory.h"
#include "sql_users.hpp"
#include "user_list.hpp"		// for RAM binding of sockets.


using namespace std;
#define Debug 1

bool ShutDownConnections = false;
byte  REQUEST_client_connect_to_robot = FALSE;
char* REQUESTED_client_ip    = NULL;
static void 	exit1() {	while (1==1) {  }; }

/* simulator shared memory */
void update_ipc_status( struct sockaddr_in* sa )
{
    static char msg[80];
    char client_ip[14];
    strcpy(client_ip, inet_ntoa( sa->sin_addr ) );
    strcpy(msg, "Connected to : ");
    strcat(msg, client_ip);
    sim_ipc_write_connection_status( msg );
}
/* simulator shared memory */
void update_ipc_status_no_connection( )
{
    static char msg[80];
    strcpy(msg, " Not Connected!");
    sim_ipc_write_connection_status( msg );
}

/* This verifies the login credentials against the mysql database of usernames. */
void handle_login(ServerHandler* h, const char* mCreds)
{
	std::string credentials = (char*)mCreds;
	h->validate( credentials );

	// Now look in list of users currently active:
	int user_index = find_user( h->m_login_name );
	if (user_index>=0)
	{
		//printf("Already other connections by this user...\n");
		h->m_user_index = user_index;
		add_connection( h->m_login_name, h );		
	} else
	{
		// To RAM list of actives (for joining/routing if requested later on):
		h->m_user_index = add_user( h->m_login_name );
		add_connection( h->m_login_name, h );		
	}
	print_user_list();
}

/* Return :  number of bytes to advance text pointer.
*/
int handle_telegram( ServerHandler* h, char* mTelegram )
{
	/* The telegram is handled 1 of 3 ways currently:
			a) As login credentials,
			b) As a routing request relay (if setup)
			c) As NLP question. (will set up routes)
	*/
	if (h->m_credentials_validated==false)
	{
		printf("Authenticating\n");
		handle_login(h, mTelegram);
		//next_telegram_ptr = (mTelegram+strlen(mTelegram)+1);
		return strlen(mTelegram)+1;
	} 
	else if (h->routes.size()) 
	{
		int len = strlen(mTelegram);
		h->route_traffic( (char*)mTelegram, len );
		//next_telegram_ptr = (mTelegram+len+1);
		return len+1;
	} else {
		//	General Protocol : 
		char* next_ptr = Parse_Statement( (char*)mTelegram, h );
		return (next_ptr - mTelegram);
	}
}

bool telegram_delim_found( char* mTelegram)
{
	char* ptr = strchr(mTelegram, '\n');
	if (ptr==NULL)
		return false;
	*ptr = 0;
	return true;	
}

/******************************************************
 * Could rename as nlp_server_thread()
 * return true 	- The telegram was handled.
 *        false  - Not known token
 ******************************************************/
void* connection_handler( void* mh )
{
	static long int connection_count = 0;	
    ServerHandler* h = (ServerHandler*)mh;
	char* next_telegram_ptr = h->socket_buffer;	
	printf("%d - Connection accepted!\n", ++connection_count ); 
    while (!h->done)
    {
    	if (ShutDownConnections==true)
    	{	h->done = true; continue;	};
    		
        // CHECK STATUS of CONNECTION :
        ioctl(h->connfd, FIONREAD, &h->bytes_available);
        if (h->bytes_available==0) {
            transmit_queued_entities(h);     // any and all outgoing data!
            continue;                       // top of loop
        }

        /*
         Get data from the client. 
         For streaming data, remember multiple 'telegrams' may come in a single read, or
         	we may only get a partial one in a single read!
         	'\n' is considered the deliminator between telegrams.
         */
        h->bytes_rxd = read( h->connfd, next_telegram_ptr, h->bytes_available );        
        if (h->bytes_rxd == 0)
        {
            printf("Server thread Received Close (0 bytes) \n");
            h->done = true;
        }
        else if (h->bytes_rxd == -1)
        {
            perror("NLP Server error: Received -1 bytes : ");
            //exit1();
        }
        else 	// DATA ARRIVED, HANDLE:
        {
            next_telegram_ptr[h->bytes_rxd] = 0;
            bool delim_found = telegram_delim_found(next_telegram_ptr);
			if (delim_found==false)
			{
				printf("No delim found... getting another read...\n");
				// for the problem of split packages - which will occur!
				next_telegram_ptr += h->bytes_rxd;
				continue;		// skip to next read (need more data!)
			} else {
				printf("Delim found, handling |%s|\n", next_telegram_ptr );

				// Loop thru all telegrams received (multiple telegrams sent quickly will arrive in a big batch)
				while (((next_telegram_ptr - h->socket_buffer) <= MAX_SENTENCE_LENGTH ) && 
						(delim_found))	// b/c if no more telegram it will be null
				{
					next_telegram_ptr += handle_telegram( h, next_telegram_ptr );	
					delim_found = telegram_delim_found(next_telegram_ptr);
				}
				next_telegram_ptr = h->socket_buffer;	// reset 	
				printf("Done parsing. %d\n", h->connfd);
			}
			
			if (h->nlp_reply_formulated)
			{
				h->Send_Reply();
				h->nlp_reply_formulated = false;
			}
            printf("\n");
        }	// end Data Arrived (read handling)        
    }
    h->done = false;
    remove_connection( h->m_login_name, h );
    
    // SEND Timestamp:
    h->ticks = time(NULL);
    h->NLP_Response = "Closing this connection.";    
    h->Send_Reply( );
    
    printf("Closing client connection %d\n", h->connfd );
    close(h->connfd);
    sleep(0.25);
    
    // Remove this ServerHandler from the list:
    for (int i=0; i<server_handlers.size(); i++)
    	if (server_handlers[i] == h)    		
    		server_handlers.erase( server_handlers.begin()+i );  // Need the iterator position
	// Then delete it.
    delete h;
    h=NULL;
    return NULL;    
}  // Terminate thread.





void video_interface()
{
}
void sequence_interface()
{
}

// Any and all outgoing data!
void transmit_queued_entities(ServerHandler* mh)
{
    //can_interface  (mh);				// Send CAN data if waiting...
    //audio_interface(mh);				// Send audio if enabled and data avail
    video_interface();				// Send audio if enabled and data avail
    sequence_interface();			// Send audio if enabled and data avail
}


/*long buff_index = (next_telegram_ptr-h->socket_buffer);
if (buff_index > 10)
{
	for (long c=buff_index-3; c<buff_index+26; c++)
		if (h->socket_buffer[c]==0)
			printf("|");
		else
			printf("%c", h->socket_buffer[c]);
	printf( ":  NextString = %s\n", next_telegram_ptr );
} 
printf(" Start parsing. buff_index=%ld of bytes_rxd=%ld; \n", buff_index, h->bytes_rxd);                
*/

// Give the package to which Nlp top levels?
//		if used in Instant:  check sim memory, sequencer memory, xeyes memory, or avisual memory:
//		Plus (in house NLPs) such as :  self-identity, ordering, math, etc. etc.

