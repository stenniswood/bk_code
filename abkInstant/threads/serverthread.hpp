#ifndef _SERVER_THREAD_H_
#define _SERVER_THREAD_H_

#include <netinet/in.h>
#include <time.h>
#include <vector>
#include <string> 
#include <list>
#include "global.h"
#include "server_handler.hpp"


extern char* REQUESTED_client_ip;
extern BYTE  REQUEST_client_connect_to_robot;
void* 		 connection_handler ( void* mh );

extern bool ShutDownConnections;



/* Ah, this may even hold frames of 1080i video, so... let's make it big: 5MB  */
void update_ipc_status				( struct sockaddr_in* sa  );
void update_ipc_status_no_connection( );

void transmit_queued_entities       ( ServerHandler* mh );



#endif
