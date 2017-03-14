#ifndef _CLIENT_MEMORY_HPP_
#define _CLIENT_MEMORY_HPP_

    
/************************************************************
	Data Flow - Client List from UDP received to avisual 
	
	abkInstant receives the UDP beacon and stores it ___[where]___
	Then puts it into this shared memory.
	avisual polls 
 ************************************************************/
#include "global.h"

#define IPC_KEY_CLI  0x04D3   // 1236 in decimal.
#define MAX_CLIENTS  300

struct stClientData {
	char	name[40];		// hostname
	int		network;		// ie. CAN, TCP/IP, private
	char	address[80];	// ip address
	char	machine[40];	// connected
	char	status[80];	// ip address	
};

/******************** CLIENT MEMORY MAP *****************/
struct client_ipc_memory_map
{
	char	 OurBeaconName[128];
	char	 ipAddress[40];
    
    char	 ConnectionStatus[64];
	Uint32_t StatusCounter;
	Uint32_t StatusAcknowledgedCounter;

    char	 Sentence[255];								// verbal commands (NLP) activate connection requests.
	Uint32_t UpdateCounter;								// Incremented on change to any of below:
	Uint32_t AcknowledgedCounter;

    char	 Response[255];								// verbal commands (NLP) activate connection requests.
	Uint32_t ResponseCounter;							// Incremented on change to any of below:
	Uint32_t ResponseAcknowledgedCounter;				// Incremented on change to any of below:
    char     ResponderName[100];                        // "instant", "simulator", etc

	Uint32_t NewClientUpdateCounter;					// Incremented on change to any of below:
	Uint32_t NewClientAcknowledgedCounter;
	int		 NumberClients;	
	struct stClientData ClientArray[MAX_CLIENTS];		// String array (dimension of NumberClients)
};


extern char* 	client_shared_memory;
extern int 		client_segment_id;
extern struct   client_ipc_memory_map* ipc_memory_client;

/*********************************************************/
/******************** CLIENT MEMORY MAP  *****************/
/*********************************************************/

void cli_dump_ipc			( );
void cli_save_segment_id	( char* mFilename );
int  cli_read_segment_id	( char* mFilename );

bool is_client_ipc_memory_available();
int  connect_shared_client_memory( char mAllocate );
int  cli_allocate_memory	( );
void cli_deallocate_memory	( int msegment_id );
int  cli_attach_memory		( );
void cli_detach_memory		( );
void cli_reattach_memory	( );
long int  cli_get_segment_size   ( );
void cli_fill_memory		( );


void cli_ipc_write_connection_status( char* mStatus   );
void cli_ipc_write_sentence	 	    ( char* mSentence );
//void cli_ipc_write_response	 	    ( char* mSentence );        // don't want duplicates, but
void cli_ipc_write_response         ( char* mSentence, char* mResponderName);

void cli_wait_for_ack_update();
bool is_new_response      ();
void cli_wait_for_response();
void cli_ack_response	  ();

// CLIENT LIST:
void cli_reset_client_list	   (  );
//void cli_ipc_add_new_client  ( struct in_addr mbeacon_ip_list );
void cli_ipc_add_new_client	   ( struct stClientData* mEntry );
bool cli_is_new_client			( );
void cli_ack_new_client			( );

void cli_print_clients		   ( );
int  cli_find_name(char*);

/************************************************************/

/************************************************************/

char*       cli_get_connection_status		();
char*       cli_get_sentence				();
long int    cli_read_status_counter         ();
long int    cli_read_sentence_counter		();
bool        cli_is_new_connection_status	();
bool        cli_is_new_update				();

void        cli_ack_connection_status	();
void        cli_ack_update_status		();

/************************************************************/


#endif
