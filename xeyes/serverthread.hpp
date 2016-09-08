#ifndef _SERVER_THREAD_H_
#define _SERVER_THREAD_H_


#define MAX_SENTENCE_LENGTH 5*1024*1024

#define UINT unsigned int
#define WORD unsigned short
#define BYTE unsigned char
//#define BOOL unsigned char

#include <time.h>
#include <string> 

void* connection_handler( void* mconnfd );

    
class ServerHandler {
public:
    ServerHandler();
    
    //void* connection_handler ( void* mconnfd );
    void    SendTelegram       ( BYTE* mBuffer, int mSize );
    void    form_response      ( const char* mTextToSend  );
    void    append_response    ( const char* mTextToAdd   );
    void    Send_Reply         (  );
    void    print_response     (  );
    void    close_connection   (  );
 
    char	 socket_buffer[MAX_SENTENCE_LENGTH];
    int 	 connfd;
    bool     done;
    bool     nlp_reply_formulated;
    std::string   NLP_Response;
    bool     keep_open;
    time_t 	 ticks;
    size_t   bytes_available;
    size_t   bytes_rxd;
    size_t   bytes_txd;
};


/* Ah, this may even hold frames of 1080i video, so... let's make it big: 5MB  */
void update_ipc_status				( struct sockaddr_in* sa  );
void update_ipc_status_no_connection( );


void transmit_queued_entities       ( ServerHandler* mh );



#endif
