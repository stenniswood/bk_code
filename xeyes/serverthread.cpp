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
    cli_ipc_write_connection_status( msg );
}
void update_ipc_status_no_connection( )
{
    static char msg[80];
    strcpy(msg, " Not Connected!");
    cli_ipc_write_connection_status( msg );
}

ServerHandler::ServerHandler()
{
    memset( socket_buffer,0,MAX_SENTENCE_LENGTH );
    NLP_Response    = "";
    connfd          = 0;
    bytes_rxd       = 0;
    bytes_txd       = 0;
    bytes_available = 0;
    done            = false;
    ticks           = 0;
    keep_open       = FALSE;
}

//void* connection_handler( void* mconnfd );
//void ServerHandler::SendTelegram( BYTE* mBuffer, int mSize );
//void ServerHandler::close_connection();

void ServerHandler::SendTelegram( unsigned char* mBuffer, int mSize)
{
    write(connfd, mBuffer, mSize );
}
void ServerHandler::close_connection()
{
    close(connfd);
    //connection_established = FALSE;
    //done = TRUE;
}

void ServerHandler::form_response(const char* mTextToSend)
{
    NLP_Response = "VR:";
    NLP_Response += mTextToSend;
    nlp_reply_formulated = TRUE;
}

void ServerHandler::append_response(const char* mTextToAdd)
{
    NLP_Response += mTextToAdd;
    nlp_reply_formulated = TRUE;    
}

// Send nlp response:
void ServerHandler::Send_Reply()
{
    printf("RESPONSE:|%s|\t\n", NLP_Response.c_str());
    bytes_txd = write( connfd, NLP_Response.c_str(), (int)NLP_Response.length() );
    if (keep_open==FALSE)
        close_connection();
    
    printf("Reply sent.\n");
}

void ServerHandler::print_response(  )
{
    //printf( "VR:");
    printf("%s\t\n\n", NLP_Response.c_str() );
}


/******************************************************
 * Could rename as nlp_server_thread()
 * return true 	- The telegram was handled.
 *        false  - Not known token
 ******************************************************/
void* connection_handler( void* mconnfd )
{
    ServerHandler* h = new ServerHandler();
    h->connfd = *((int*)mconnfd);
    
    while (!h->done)
    {
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
            h->done = TRUE;
        }
        else if (h->bytes_rxd == -1)
        {
            perror("NLP Server thread error: Received -1 bytes;");
            exit1();
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
                next_telegram_ptr   = Parse_Statement( next_telegram_ptr, h );
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
    h->done = FALSE;
    
    // SEND Timestamp:
    h->ticks = time(NULL);
    strcpy(NLP_Response, "Closing this connection.");
    h->Send_Reply( );
    printf( " %s\n", NLP_Response );
    
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
    can_interface  (mh);				// Send CAN data if waiting...
    audio_interface(mh);				// Send audio if enabled and data avail
    video_interface();				// Send audio if enabled and data avail
    sequence_interface();			// Send audio if enabled and data avail
}

