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
//#include "visual_memory.h"
#include "GENERAL_protocol.hpp"

#include "simulator_memory.h"
#include "sql_users.hpp"
#include "user_list.hpp"		// for RAM binding of sockets.


std::vector<ServerHandler*>  server_handlers;

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
	
	if (count < 3)
		return retval;
		
	SuperString devname = str.m_split_words[2];	// device name
	devname.split(':');
	m_login_devicename = devname.m_split_words[1];
	printf("\tdevname=%s\n", m_login_devicename.c_str() );
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

// All data received on this connfd, will be passed along to the list of routes.
// This function adds one to the list.
bool ServerHandler::establish_route( ServerHandler* mDest )
{
	routes.push_back( mDest );
}
bool ServerHandler::remove_route ( ServerHandler* mDest )
{
	routes.remove( mDest );
}
bool ServerHandler::route_traffic( char* mText, int mLength )
{
	std::list<ServerHandler*>::iterator iter = routes.begin();
	while (iter != routes.end() )
	{
		// Send telegram:
		printf("ROUTING RESPONSE:|%s|\t\n", mText );
		bytes_txd = write( (*iter)->connfd, mText, mLength );
		iter++;
	}
}

void test_server_handler()
{
	string credentials = "Login:stenniswood;Password:blahblah77;\n";
	ServerHandler sh;	
	sh.parse_credentials( credentials );	
}
