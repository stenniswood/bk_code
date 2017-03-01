#ifndef _SERVER_HANDLER_H_
#define _SERVER_HANDLER_H_

#include <netinet/in.h>
#include <time.h>
#include <vector>
#include <string> 
#include <list>
#include "global.h"

#define 	 MAX_SENTENCE_LENGTH  5*1024*1024

class ServerHandler {
public:
    ServerHandler();
    //void* connection_handler ( void* mconnfd );
	
	bool 	parse_credentials  ( std::string mCredentials );
	bool 	validate		   ( std::string mCredentials );
	void	accept			   ( int mlistenfd 			  );
    void    SendTelegram       ( BYTE* mBuffer, int mSize );
    void    form_response      ( const char* mTextToSend  );
    void    append_response    ( const char* mTextToAdd   );
    void    Send_Reply         (  );
    void    print_response     (  );
    void    close_connection   (  );
 	bool 	establish_route	   ( ServerHandler* mDest );
 	bool 	remove_route	   ( ServerHandler* mDest ); 	
 	bool	route_traffic	   ( char* mText, int mLength );
 	
    char	 	socket_buffer[MAX_SENTENCE_LENGTH];
    int 	 	connfd;
    struct	 	sockaddr 	client_addr;
	pthread_t	server_thread_id;

	bool			m_credentials_validated;
	std::string		m_login_name;
	std::string		m_login_password;
	std::string		m_login_devicename;	
	size_t			m_user_index;		// see user_list.cpp
	std::list<ServerHandler*> routes;	// communications will be transferred
	
	
    bool     done;
    bool     nlp_reply_formulated;
    std::string   NLP_Response;
    bool     keep_open;
    time_t 	 ticks;
    size_t   bytes_available;
    size_t   bytes_rxd;
    size_t   bytes_txd;
};

extern std::vector<ServerHandler*>  server_handlers;

void test_server_handler();

#endif
