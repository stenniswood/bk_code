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

//#include "top_level_protocol_sim.hpp"
//#include "vision_memory.h"
#include "simulator_memory.h"
#include "sql_users.hpp"

bool ShutDownConnections = false;

//#include "nlp_vision_general_protocol.hpp"

byte  REQUEST_client_connect_to_robot = FALSE;
char* REQUESTED_client_ip    = NULL;

using namespace std;

#define Debug 1
static void 	exit1() {	while (1==1) {  }; }

void update_ipc_status( struct sockaddr_in* sa )
{
    static char msg[80];
    char client_ip[14];
    strcpy(client_ip, inet_ntoa( sa->sin_addr ) );
    strcpy(msg, "Connected to : ");
    strcat(msg, client_ip);
    sim_ipc_write_connection_status( msg );
}

void update_ipc_status_no_connection( )
{
    static char msg[80];
    strcpy(msg, " Not Connected!");
    sim_ipc_write_connection_status( msg );
}

ServerHandler::ServerHandler()
{
	m_credentials_validated = false;
    memset( socket_buffer,0,MAX_SENTENCE_LENGTH );
    NLP_Response    = "";
    connfd          = 0;
    bytes_rxd       = 0;
    bytes_txd       = 0;
    bytes_available = 0;
    done            = false;
    ticks           = 0;
    keep_open       = true;
}

void ServerHandler::SendTelegram( unsigned char* mBuffer, int mSize)
{
    write(connfd, mBuffer, mSize );
}
void ServerHandler::close_connection()
{
    close(connfd);
}

void ServerHandler::form_response(const char* mTextToSend)
{
    NLP_Response = "VR:";
    NLP_Response += mTextToSend;
    nlp_reply_formulated = true;
}

void ServerHandler::append_response(const char* mTextToAdd)
{
    NLP_Response += mTextToAdd;
    nlp_reply_formulated = true;
}

// Send nlp response:
void ServerHandler::Send_Reply()
{
    printf("RESPONSE:|%s|\t\n", NLP_Response.c_str());
    bytes_txd = write( connfd, NLP_Response.c_str(), (int)NLP_Response.length() );
    if (keep_open==false)
        close_connection();
    //printf("Reply sent.\n");
}

void ServerHandler::print_response(  )
{
    printf("%s\t\n\n", NLP_Response.c_str() );
}

void ServerHandler::accept( int mlistenfd )
{
    socklen_t size = (socklen_t)sizeof(client_addr);	
	connfd = ::accept( mlistenfd, (struct sockaddr*)&(client_addr), &size );
}

/* 
INPUT:  Single line '\n' terminated.  	
	Login:stenniswood;Password:blahblah77;\n
	
OUTPUT: m_login_name & m_login_password 
	m_login_name
	m_login_password
*/
bool ServerHandler::parse_credentials( string mCredentials )
{
	bool retval = true;
	SuperString str = mCredentials;
	
	int count = str.split(';');
	if (count < 2)
		return false;
	SuperString name = str.m_split_words[0];
	SuperString passwd = str.m_split_words[1];
	
	name.split(':');
	m_login_name = name.m_split_words[1];
	passwd.split(':');
	m_login_password = passwd.m_split_words[1];
	
	//printf("Login=%s\t\tpasswd=%s\n", m_login_name.c_str(), m_login_password.c_str() );
	return retval;
}

bool ServerHandler::validate( string mCredentials )
{
	bool ok = parse_credentials( mCredentials );
	if 	(ok)
	{
		int found = sql_users.sql_find( m_login_name );
		if (found)
		{
			printf("Found the username in dbase :\t");
			m_credentials_validated = sql_users.verify_password( m_login_name, m_login_password );
			if (m_credentials_validated)
			{
				form_response( "Welcome" );
			} else {
				form_response( "Bad password" );
			}
		}
		else {
			printf("NOT found in dbase\n");		
			form_response( "Sorry I can't find that username.  Please register." );
		}
		// and verify the password!		
	} else
	{
		form_response( "Sorry there was garbled text in your credentials." );
	}
	return m_credentials_validated;
}

void test_server_handler()
{
	string credentials = "Login:stenniswood;Password:blahblah77;\n";
	ServerHandler sh;	
	sh.parse_credentials( credentials );	
}


/******************************************************
 * Could rename as nlp_server_thread()
 * return true 	- The telegram was handled.
 *        false  - Not known token
 ******************************************************/
void* connection_handler( void* mh )
{
	static long int connection_count = 0;
	connection_count++;
    ServerHandler* h = (ServerHandler*)mh;
	printf("%d - Connection accepted!\n", connection_count);
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
         For streaming data, remember multiple 'telegrams' may come in a single read.
         */
        h->bytes_rxd = read( h->connfd, h->socket_buffer, h->bytes_available );        
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
            h->socket_buffer[h->bytes_rxd] = 0;
            const char* next_telegram_ptr  = h->socket_buffer;

            // Loop thru all telegrams received (multiple telegrams sent quickly will arrive in a big batch)
            while ( (next_telegram_ptr - h->socket_buffer) < h->bytes_rxd )
            {
                long buff_index = (next_telegram_ptr-h->socket_buffer);
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
                // Give the package to which Nlp top levels?
                //		if used in Instant:  check sim memory, sequencer memory, xeyes memory, or avisual memory:
                //		Plus (in house NLPs) such as :  self-identity, ordering, math, etc. etc.
                //	General Protocol : 
				if (h==NULL) {
					printf("h is NULL!!\n");					
				}
				if (h->m_credentials_validated==false)
				{
					std::string credentials = (char*)next_telegram_ptr;
					h->validate( credentials );
					next_telegram_ptr = (next_telegram_ptr+strlen(next_telegram_ptr)+1);
				} else
					next_telegram_ptr = Parse_Statement( (char*)next_telegram_ptr, h );
                // next the problem of split packages - which will occur!
            }
            printf("Done parsing. %d\n", h->connfd);

            if (h)
                if (h->nlp_reply_formulated)
                {
                    h->Send_Reply();
                    h->nlp_reply_formulated = false;
                }
            printf("\n");
        }
    }
    h->done = false;
    
    // SEND Timestamp:
    h->ticks = time(NULL);
    h->NLP_Response = "Closing this connection.";    
    h->Send_Reply( );
    printf( " %s\n", h->NLP_Response.c_str() );
    
    printf("Closing client connection %d\n", h->connfd );
    close(h->connfd);
    sleep(0.25);
    delete h;
    h=NULL;
    return NULL;    
}  // Terminate thread, wait for another connect.

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


//void* connection_handler( void* mconnfd );
//void ServerHandler::SendTelegram( BYTE* mBuffer, int mSize );
//void ServerHandler::close_connection();
//connection_established = FALSE;
//done = TRUE;