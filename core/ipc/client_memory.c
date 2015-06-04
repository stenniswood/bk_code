/*********************************************************************
Product of Beyond Kinetics, Inc
------------------------------------------
|			Atmel						 |
------------------------------------------
			|SPI|
------------------------------------------
|   atiltcam   |<==>|  bkInstant (wifi)  |
------------------------------------------

This code handles IPC Shared memory for graphical display:  visual
Intended for the PiCamScan board on RaspberryPi.

WRITES TO SHARED MEMORY:
	The "abkInstant" establishes the memory segment.
	and the avisual may connect to it when it's run.
READS:
	
DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdio.h> 
#include <sys/stat.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <list>

#include "bk_system_defs.h"
#include "interrupt.h"
#include "client_memory.h"


char* 	client_shared_memory;
int 	client_segment_id;
struct  client_ipc_memory_map* ipc_memory_client =NULL;
 
void cli_dump_ipc()
{
	int length = sizeof(struct client_ipc_memory_map);
	for (int i=0; i<length; i++)
	{
		if ((i%32)==0)
			printf("\n");
		printf("%2x ", client_shared_memory[i] );
	}	
}

void cli_save_segment_id(char* mFilename)
{
	FILE* fd = fopen(mFilename, "w");
	//FILE* fd = fopen("client_shared_memseg_id.cfg", "w");
	printf("Segment_id=%d\n", client_segment_id );
	fprintf( fd, "%d", client_segment_id );
	fclose( fd );
}
int cli_read_segment_id(char* mFilename)
{
	FILE* fd = fopen( mFilename, "r" );
	fscanf( fd, "%d", &client_segment_id );
	fclose( fd );
	return client_segment_id;
}
 
int cli_allocate_memory( )
{
	const int 	shared_segment_size = sizeof(struct client_ipc_memory_map);
	printf ("Client shm seg size=%d\n", shared_segment_size );
	
	/* Allocate a shared memory segment. */
	client_segment_id = shmget( IPC_KEY_CLI, shared_segment_size, IPC_CREAT | 0666 );

	// IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	printf ("Client shm segment_id=%d\n", client_segment_id );
	return client_segment_id;
}

int cli_attach_memory()
{
	/* Attach the shared memory segment. */
	client_shared_memory 	 = (char*) shmat (client_segment_id, 0, 0);
	ipc_memory_client		 = (struct client_ipc_memory_map*)client_shared_memory;
	printf ("Client shm attached at address %p\n", client_shared_memory); 	
}

void cli_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	client_shared_memory = (char*) shmat (client_segment_id, (void*) 0x5000000, 0); 
	ipc_memory_client	 = (struct client_ipc_memory_map*)client_shared_memory;	
	printf ("Client shm reattached at address %p\n", client_shared_memory); 
}

void cli_detach_memory()
{
	/* Detach the shared memory segment. */
	shmdt (client_shared_memory);
}

int cli_get_segment_size()
{
	struct 		shmid_ds	shmbuffer;
	/* Determine the segment’s size. */
	shmctl (client_segment_id, IPC_STAT, &shmbuffer);
	int segment_size = shmbuffer.shm_segsz;
	printf ("Client segment size: %d\n", segment_size);
	return segment_size;
}
void cli_fill_memory()
{
	memset(client_shared_memory, 0, cli_get_segment_size());
	for (int i=0; i<1000; i++)
		client_shared_memory[i] = (i%255); 
}

void cli_deallocate_memory(int msegment_id)
{
	/* Deallocate the shared memory segment. */ 
	shmctl (msegment_id, IPC_RMID, 0);
}

/* WRITE IMPLIES TO SHARED MEMORY.  And since we are the abkInstant task,
	the writes will be transfer to avisual
FORMAT:
	INT 		ID
	char[255] 	Buffer
*/
void cli_ipc_write_command_text( char* mSentence )
{
	int length = strlen(mSentence);	
	int MaxAllowedLength = sizeof(ipc_memory_client->Sentence);	
	if (length>MaxAllowedLength)
		length = MaxAllowedLength;
		
	ipc_memory_client->RequestCount++;

	//printf("%d:Copying %d bytes to shared mem.\n", SentenceCounter, length);
	strcpy(ipc_memory_client->Sentence, mSentence);
	printf("|%s|\n", ipc_memory_client->Sentence );
	//dump_ipc();
}

void cli_ipc_write_connection_status( char* mStatus )
{
	int length = strlen(mStatus);	
	int MaxAllowedLength = sizeof(ipc_memory_client->ConnectionStatus);
	if (length > MaxAllowedLength)
		length = MaxAllowedLength;	// do not write into memory of next variable.

	ipc_memory_client->StatusCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", StatusCounter, length );
	strcpy( ipc_memory_client->ConnectionStatus, mStatus);
	printf( "|%s|\n", ipc_memory_client->ConnectionStatus );
}

void cli_ipc_write_active_page( short NewActivePage )
{
//	ipc_memory_avis->ScreenNumber = NewActivePage;
}
/* See udp_transponder for update_client_list()		*/


/*********************************************************************
Product of Beyond Kinetics, Inc
------------------------------------------
|			Atmel						 |
------------------------------------------
			|SPI|
------------------------------------------
|   atiltcam   |<==>|  bkInstant (wifi)  |
------------------------------------------

This code handles IPC Shared memory:  atiltcam
Intended for the PiCamScan board on RaspberryPi.

WRITES TO SHARED MEMORY:
	Analog 	
READS:
	Lowside Outputs	- this app reserves the right to override the outputs based on the application.
	Servo Angles

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/

long int	SentenceCounter=0;
char*		Sentence;
long int	StatusCounter=0;
char*		Status;

int connect_shared_client_memory( )
{
	printf("connect_shared_client_memory()  \t");
	if (cli_allocate_memory( ) == -1)
	{
		printf("Can't find shared memory from abkInstant!\n");
		return 0;
	}
	cli_attach_memory( );
	printf("Sentence = %s\n", ipc_memory_client->Sentence );
	//dump_ipc();
	//printf("Sizeof(int) = %d\n", sizeof(int) );
	return 1;
}

char* get_connection_status()
{
	return (ipc_memory_client->ConnectionStatus);
}
char* get_sentence()
{
	return (ipc_memory_client->Sentence);
}

int read_status_counter()
{
	return ipc_memory_client->StatusCounter;
}
int read_sentence_counter()
{
	return ipc_memory_client->RequestCount;
}

BOOL is_new_connection_status()
{
	int latest_count = read_status_counter();
	if (latest_count > StatusCounter)
	{
		printf("Latest_count = %d/%d\n", latest_count, StatusCounter);			
		StatusCounter = latest_count;
		return TRUE;
	}
	return FALSE;
}

BOOL is_new_sentence()
{
	int latest_count = read_sentence_counter();
	if (latest_count > SentenceCounter)
	{
		printf("Latest_count = %d/%d\n", latest_count, SentenceCounter);			
		SentenceCounter = latest_count;
		return TRUE;
	}
	return FALSE;
}

/*void cli_ipc_add_new_client 	( char* mClientText )
{
	int size  = ipc_memory_client->NumberClients;
	char* ptr = ipc_memory_client->ClientArray;
	char* ptr2;
	for (int i=0; i<(size-1); i++)
	{
		ptr2 = strchr( ptr, 0 );
		if (ptr2)
			ptr = ptr2+1;
	}
	strcpy( ptr, mClientText );
	ipc_memory_client->NumberClients++;
}*/

void cli_ipc_add_new_client( list<struct in_addr> mbeacon_ip_list )
{
	// Number of clients:
	ipc_memory_client->NumberClients    = mbeacon_ip_list.size();
	list<struct in_addr>::iterator iter = mbeacon_ip_list.begin();
	int byte_array_size = 0;
	char* ptr 			= ipc_memory_client->ClientArray;

	// Now add each client as a string followed by a null terminator.  Hence a string array.
	while(( iter != mbeacon_ip_list.end()) && (byte_array_size<MAX_CLIENT_ARRAY_SIZE))
	{
		char* ip_str = inet_ntoa(*iter);
		int len = strlen(ip_str);
		
		// Copy 1 client:
		memcpy( ptr,  ip_str, len );
		ptr[len] = 0;
		
		// Adjust for next client:
		ptr      += (len+1);		// skip null terminator!
		byte_array_size += (len+1);	// null terminator
		iter++;
	}
}

void print_clients()
{
	int size = ipc_memory_client->NumberClients;
	printf("There are %d available clients.\n", size );

	char* ptr = ipc_memory_client->ClientArray;	
	char* ptr2 = strchr( ptr, 0 );
	
	for (int i=0; i<size; i++)
	{
		printf(" %s \n", ptr );
		ptr2 = strchr( ptr, 0 );
		if (ptr2)
			ptr = ptr2;
	}
	return ;
}



