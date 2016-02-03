#ifndef _CLIENT_MEMORY_HPP_
#define _CLIENT_MEMORY_HPP_

/************************************************************
	Data Flow - Client List from UDP received to avisual 
	
	abkInstant receives the UDP beacon and stores it ___[where]___
	Then puts it into this shared memory.
	avisual polls 
 ************************************************************/

#include <list>

#define IPC_KEY_CLI  1274 
#define MAX_CLIENTS  300


struct stClientData {
	char	name[40];		// hostname
	int		network;		// ie. CAN, TCP/IP, private
	char	address[80];	// ip address
	char	machine[40];	// connected
};

/******************** CLIENT MEMORY MAP *****************/
struct client_ipc_memory_map
{
	long int StatusCounter;
	char	 ConnectionStatus[64];
	char	 OurBeaconName[128];
	char	 ipAddress[40];	
	long int StatusAcknowledgedCounter;

	long int UpdateCounter;					// Incremented on change to any of below:
	long int AcknowledgedCounter; 
	
	char	 Sentence[128];					// verbal commands (NLP) activate connection requests.	
	int		 NumberClients;
	struct stClientData ClientArray[MAX_CLIENTS];		// String array (dimension of NumberClients)
	//short	 ScreenNumber;								// Which screen is being displayed.  voice commands can change.  Simplistic for now a single number per page.	
};

extern char* 	client_shared_memory;
extern int 		client_segment_id;
extern struct   client_ipc_memory_map* ipc_memory_client;

/******************** CLIENT MEMORY MAP *****************/
/*********************************************************/

void cli_dump_ipc			( );
void cli_save_segment_id	( char* mFilename );
int  cli_read_segment_id	( char* mFilename );

int  cli_allocate_memory	( );
void cli_deallocate_memory	( int msegment_id );
int  cli_attach_memory		( );
void cli_detach_memory		( );
void cli_reattach_memory	( );
int  cli_get_segment_size   ( );
void cli_fill_memory		( );

void cli_ipc_write_connection_status( char* mStatus   );
void cli_ipc_write_sentence	 	    ( char* mSentence );

// for instant to fill in the shared mem buffer:
void cli_ipc_add_new_client	   ( struct in_addr mbeacon_ip_list );
void cli_ipc_add_new_clients   ( std::list<struct in_addr> mbeacon_ip_list );
void cli_print_clients		   ( );

// new:
int cli_find_name(char*);
void cli_wait_for_ack_update();

/************************************************************/

/************************************************************/
int connect_shared_client_memory( char mAllocate=FALSE );
char* get_connection_status		();
char* get_sentence				();
int   read_status_counter		();
int   read_sentence_counter		();
BOOL  cli_is_new_connection_status	();
BOOL  cli_is_new_update				();

void cli_ack_connection_status	();
void cli_ack_update_status		();
/************************************************************/


#endif
