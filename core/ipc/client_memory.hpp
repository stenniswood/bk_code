#ifdef  __cplusplus
//extern "C" {
#endif

#include <list>

#define IPC_KEY_CLI 1274
#define MAX_CLIENT_ARRAY_SIZE 2048

/******************** CLIENT MEMORY MAP *****************/
struct client_ipc_memory_map
{
	long int StatusCounter;
	char	 ConnectionStatus[64];
	char	 OurBeaconName[128];
	char	 ipAddress[40];	
	
	long int RequestCount;
	char	 Sentence        [128];						// verbal commands (NLP) activate connection requests.	
	long int AcknowledgedCount;
	
	int		 NumberClients;
	char	 ClientArray[MAX_CLIENT_ARRAY_SIZE];		// String array (dimension of NumberClients)
	//short	 ScreenNumber;								// Which screen is being displayed.  voice commands can change.  Simplistic for now a single number per page.	
};

extern char* 	client_shared_memory;
extern int 		client_segment_id;
extern struct   client_ipc_memory_map* ipc_memory_client;

/******************** CLIENT MEMORY MAP *****************/
/*********************************************************/

void cli_dump_ipc			();
void cli_save_segment_id	(char* mFilename);
int  cli_read_segment_id	(char* mFilename);

int  cli_allocate_memory	();
void cli_deallocate_memory	(int msegment_id);
int  cli_attach_memory		();
void cli_detach_memory		();
void cli_reattach_memory	();
int  cli_get_segment_size();
void cli_fill_memory		();


void cli_ipc_write_connection_status( char* mStatus   );
void cli_ipc_write_command_text 	( char* mSentence );

// for instant to fill in the shared mem buffer:
void cli_ipc_add_new_client( std::list<struct in_addr> mbeacon_ip_list );
void print_clients			();

/************************************************************/

int connect_shared_client_memory( char mAllocate=FALSE );
char* get_connection_status		();
char* get_sentence				();
int   read_status_counter		();
int   read_sentence_counter		();
BOOL  is_new_connection_status	();
BOOL  is_new_sentence			();

/************************************************************/


#ifdef  __cplusplus
//}
#endif
