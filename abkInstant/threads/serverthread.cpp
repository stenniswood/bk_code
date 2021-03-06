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
//#include "visual_memory.hpp"
#include "GENERAL_protocol.hpp"
#include "simulator_memory.h"
#include "sql_users.hpp"
#include "sql_device.hpp"
#include "user_list.hpp"		// for RAM binding of sockets.
#include "protocol.h"
#include "top_level_protocol.hpp"
#include "route_protocol.hpp"


using namespace std;
#define Debug 1

bool  ShutDownConnections 	 = false;
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
	printf("Sentence:|%s|\n", mCreds);
	bool ok = h->parse_credentials( credentials );
	if (ok)
		h->validate( credentials );
	else 		
		return;

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
	
	// Look up Device ID and Device Preferred Name: 
	if (h->m_credentials_validated==true)
	{
		// First check if it already exists in the database of users:
		// (user_id, preferred_name, device_type, hostname) must all match or it gets added.
		int result = sql_devices.sql_find( h->m_user_id, h->m_login_hostname );
		if (result) 
		{
			h->m_device_id = sql_devices._id;
			if (sql_devices.m_row) {
				printf("sql_devices.m_row[5]= %s\n", sql_devices.m_row[5] );
				h->m_dev_preferred_name = std::string((sql_devices.m_row)[5]);
			}
		} else 
			h->m_device_id = -1;	
	}
	
}


/* 
Return :  number of bytes to advance text pointer.
*/
int handle_telegram( ServerHandler* h, char* mTelegram )
{
	/* The telegram is handled 1 of 3 ways currently:
			a) As login credentials,
			b) As a routing request relay (if setup)
			c) As NLP question. (will set up routes)
	*/
	char key_word[40];
	char* ptr = strchr(mTelegram, ';');
	if (ptr) {
		*ptr = 0;	strcpy( key_word, mTelegram );		*ptr = ';';
	}
	
	if (h->m_credentials_validated==false) 
	{
		if (strcmp(key_word, "Login")==0)  
		{
			printf("Authenticating\n");
			handle_login( h, mTelegram );
			return strlen(mTelegram)+1;
		}
		return 0;
	} else {
		if (h->routes.size()) 
		{
			int procssed = Parse_Cancel_Routing_Statement( mTelegram, h ); 
			//return (next_ptr - mTelegram)+1;
			if (procssed==-1)
			{			
				int len = strlen(mTelegram);
				h->route_traffic( (char*)mTelegram, len );
				return len+1;
			} 
			else return 1;
		} else {
			//	General Protocol : 
			char* next_ptr = Parse_top_level_protocol( mTelegram, h ); 
			return (next_ptr - mTelegram)+1;
		}
	}
}

bool telegram_delim_found( char* mTelegram)
{
	char* ptr = strchr(mTelegram, 0);
	if (ptr==NULL)
		return false;
	//*ptr = 0;
	return true;	
}

bool speak_locally = false;
void send_response( ServerHandler* h )
{
	string cmd;
	if (h->nlp_reply_formulated)
	{
		if (speak_locally)
		{
			cmd = "echo \""; 
			cmd += h->NLP_Response;
			cmd += "\"; | festival -tts";
			system(cmd.c_str() );
		}
		h->Send_Reply();
		h->nlp_reply_formulated = false;
	}
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
	        next_telegram_ptr = h->socket_buffer;
            next_telegram_ptr[h->bytes_rxd] = 0;
            bool delim_found = true; // telegram_delim_found(next_telegram_ptr);
			long buff_index = (next_telegram_ptr - h->socket_buffer);
			//printf(" Start parsing. buff_index=%ld of bytes_rxd=%ld; \n", buff_index, h->bytes_rxd);                
            
			if (delim_found==false)
			{
				printf("No delim found... getting another read...\n");
				// for the problem of split packages - which will occur!
				next_telegram_ptr += h->bytes_rxd;
				continue;		// skip to next read (need more data!)
			} else {
				//printf("Delim found, |%s|\n", next_telegram_ptr );
				printf("Delim found, ");

				// Loop thru all telegrams received (multiple telegrams sent quickly will arrive in a big batch)
				while (((next_telegram_ptr - h->socket_buffer) <= MAX_SENTENCE_LENGTH ) && 
						(delim_found))	// b/c if no more telegram it will be null
				{
					next_telegram_ptr += handle_telegram( h, next_telegram_ptr );	
					delim_found = false; //telegram_delim_found(next_telegram_ptr);
				}
				next_telegram_ptr = h->socket_buffer;	// reset 	
				printf("Done parsing. %d\n", h->connfd);
			}

			send_response(h);			
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

#include<errno.h> //For errno - the error number
#include<netdb.h> //hostent
#include<arpa/inet.h>
#include<sys/socket.h>


/*  Get ip from domain name */
int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;    
    if ( (he = gethostbyname( hostname ) ) == NULL) 
    {
        herror("gethostbyname");
        return 1;
    }
    addr_list = (struct in_addr **) he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        printf("host=%s;  ip=%s;\n", hostname, ip );
        return 0;
    }    
    return 1;
}

char credentials_filename[] = "credentials.txt";
char device_info_filename[] = "device_info.txt";
char device_caps_filename[] = "device_caps.txt";

void send_credentials(int connfd)
{
	char str[1024*4];
	FILE* fd = fopen( credentials_filename, "r");
	if (fd==-1) {
		perror("Cannot open Device Info file ");
		printf("%s\n", device_info_filename );
	}
	int bytes_read = fread( str, 1024, 1, fd );
	write (connfd, str, strlen(str) );	
}
#include <unistd.h>

void send_device_info(int connfd)
{
	char str[1024*4];
	char hostname[80];
	char fulltext[1024*4];
	int ret = gethostname(hostname, 80);
	if (ret==-1)
		perror("gethostname ");
	
	FILE* fd = fopen( device_info_filename, "r");
		
	if (fd==-1) {
		perror("Cannot open Device Info file ");
		printf("%s\n", device_info_filename );
	}

	strcpy(fulltext, "Device Info;\nhostname:");
	strcat(fulltext, hostname);
	strcat(fulltext, ";\n");
	int bytes_read = fread( str, 1024, 1, fd );	
	strcat(fulltext, str);
	write (connfd, fulltext, strlen(fulltext) );	
}

void send_device_capabilities(int connfd)
{
	char str[1024*4];
	FILE* fd = fopen( device_caps_filename, "r");
	if (fd==-1) {
		perror("Cannot open Device Info file ");
		printf("%s\n", device_info_filename );
	}
	int bytes_read = fread( str, 1024, 1, fd );
	write (connfd, str, strlen(str) );
}


int connect_to_viki()
{
	char 	ip[26];
	hostname_to_ip("www.beyond-kinetics.com" , ip );
	int retval = connect_to_robot( ip );
	
	return retval;	
}

/* This will establish a client connection to Viki.
	Further communications will be handled by the connection_handler() thread above.
	
	Return 1 =>Error;	0 =>Okay;
*/
int connect_to_robot(char *ip_address )
{
	int  	sockfd=0;
	printf("====Connect_to_robot...ip=%s\n", ip_address);    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    struct sockaddr_in	serv_addr; 
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(BK_MEDIA_PORT);
    if(inet_pton(AF_INET, ip_address, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_aton error occured\n");
        return 1;
    } 

	printf("ROBOT CLIENT Connecting to : %s:%d\n", ip_address, BK_MEDIA_PORT );
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 
	printf("ROBOT CLIENT Connected\n");
	send_credentials (sockfd);
	sleep(1);
	send_device_info (sockfd);
	sleep(1);
	send_device_capabilities(sockfd);

	ServerHandler* sh = new ServerHandler();	
	sh->m_credentials_validated = true;		// Since we are client and authenticating into viki.
	sh->connfd = sockfd;
	int iret1 = pthread_create( &(sh->server_thread_id), NULL, connection_handler, (void*) sh);
	if (iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n", iret1 );
		printf("Error - pthread_create() return code: %d\n", iret1 );
		delete sh;
	}	
	server_handlers.push_back( sh );		
    return 0;
}



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


/*printf(" Start parsing. buff_index=%ld of bytes_rxd=%ld; \n", buff_index, h->bytes_rxd);                */
