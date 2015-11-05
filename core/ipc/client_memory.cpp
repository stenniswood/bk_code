
/*********************************************************************
Product of Beyond Kinetics, Inc
------------------------------------------
|	abkInstant |<==>|  bkInstant (wifi)	 |
------------------------------------------
			   |SHM|
------------------------------------------
|   avisual     						 |
------------------------------------------

This code handles IPC Shared memory for graphical display:  visual
Intended for the PiCamScan board on RaspberryPi.

WRITES TO SHARED MEMORY:
	The "abkInstant" establishes the memory segment.
	and the avisual may connect to it when it's run.
READS:
	Lowside Outputs	- this app reserves the right to override the outputs based on the application.
	Servo Angles
	
DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdio.h> 
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <list>
#include <arpa/inet.h>
#include <errno.h>
#include "bk_system_defs.h"
#include "interrupt.h"
#include "client_memory.hpp"


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
	if (fd==NULL) {
		printf("Cannot open file %s. %s \n", mFilename, strerror(errno) );		
	}
	
	//FILE* fd = fopen("client_shared_memseg_id.cfg", "w");
	printf("Segment_id=%d\n", client_segment_id );
	char line[80];
	//fprintf( fd, "%d", client_segment_id );
	sprintf( line, "%d", client_segment_id );
	fwrite ( line, strlen(line), 1, fd  );
	fclose( fd );
}
int cli_read_segment_id(char* mFilename)
{
	char tline[40];
	FILE* fd = fopen( mFilename, "r" );
	if (fd==NULL)  {
		printf("CLI_read_segment_id - ERROR: %s \n", strerror(errno) );	
		return -1;
	}
	int result = fread( tline, 1, 20, fd);		//	fscanf( fd, "%d", &can_segment_id );
	tline[result] = 0;
	client_segment_id = atol( tline );
	fclose( fd );
	printf("cli_segment_id= %d \n", client_segment_id );	
	return client_segment_id;
}

int cli_allocate_memory( )
{
	const int 	shared_segment_size = sizeof(struct client_ipc_memory_map);
	printf ("Client shm seg size=%d\n", shared_segment_size );
	
	/* Allocate a shared memory segment. */
	client_segment_id = shmget( IPC_KEY_CLI, shared_segment_size, IPC_CREAT | 0666 );
	int errsv = errno;
	if (client_segment_id==-1)
		printf("cli_allocate_memory - ERROR: %s \n", strerror(errsv) );
	else 
		printf ("Client shm segment_id=%d\n", client_segment_id );
	// IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	return client_segment_id;
}

int cli_attach_memory()
{
	/* Attach the shared memory segment. */
	int error=0;
	client_shared_memory = (char*)shmat(client_segment_id, 0, 0);
	if (client_shared_memory==(char*)-1) {
		printf("cli_attach_memory - ERROR: %s \n", strerror(errno) );
		return 0;
	} else 
		printf ("Client shm attached at address %p\n", client_shared_memory); 	
	
	ipc_memory_client = (struct client_ipc_memory_map*)client_shared_memory;
	return 1;
}

void cli_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	client_shared_memory = (char*) shmat (client_segment_id, (void*) 0x5000000, 0); 
	if (client_shared_memory==(char*)-1) {
		printf("cli_attach_memory - ERROR: %s \n", strerror(errno) );
		return;
	}
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
	int size = cli_get_segment_size();
	memset(client_shared_memory, 0, size);
//	for (int i=0; i<size; i++)
//		client_shared_memory[i] = (i%128); 
}

void cli_deallocate_memory(int msegment_id)
{
	/* Deallocate the shared memory segment. */ 
	shmctl (msegment_id, IPC_RMID, 0);
}

/*********************************************************/
/* Functions which access the data members:				 */
/*********************************************************/

/* WRITE IMPLIES TO SHARED MEMORY.  And since we are the abkInstant task,
	the writes will be transfer to avisual
FORMAT:
	INT 		ID
	char[255] 	Buffer
*/
void cli_ipc_write_sentence( char* mSentence )
{
	int length = strlen(mSentence);	
	int MaxAllowedLength = sizeof(ipc_memory_client->Sentence);	
	if (length>MaxAllowedLength) {
		length = MaxAllowedLength;
		mSentence[MaxAllowedLength] = 0;
	}
	ipc_memory_client->UpdateCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", SentenceCounter, length);
	strcpy(ipc_memory_client->Sentence, mSentence);
	printf("|%s|\n", ipc_memory_client->Sentence );
}

void cli_ipc_write_connection_status( char* mStatus )
{
	int length = strlen(mStatus);	
	int MaxAllowedLength = sizeof(ipc_memory_client->ConnectionStatus);
	if (length > MaxAllowedLength) {
		length = MaxAllowedLength;	// do not write into memory of next variable.
		mStatus[MaxAllowedLength] = 0;
	}
	ipc_memory_client->StatusCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", StatusCounter, length );
	strcpy( ipc_memory_client->ConnectionStatus, mStatus);
	printf( "|%s|\n", ipc_memory_client->ConnectionStatus );
}

/*void cli_ipc_write_active_page( short NewActivePage )
{
	ipc_memory_avis->ScreenNumber = NewActivePage;
} */

/* See udp_transponder for update_client_list()		*/

 
#if (PLATFORM==Mac)
char cli_segment_id_filename[] = "/Users/stephentenniswood/code/bk_code/client/cli_shared_memseg_id.cfg";
#elif (PLATFORM==RPI)
char cli_segment_id_filename[] = "/home/pi/bk_code/client/cli_shared_memseg_id.cfg";
#elif (PLATFORM==linux_desktop)
char cli_segment_id_filename[] = "/home/steve/bk_code/client/cli_shared_memseg_id.cfg";
#endif


/* The allocating should be done by abkInstant. 
		Should test for mem segment allocation first.
		Then if requested allocate.
Return : 
	1 => Attached to memory successfully.
	0 => No connection.
*/
int connect_shared_client_memory( char mAllocate )
{
	if (mAllocate)
	{
		int result = cli_allocate_memory( );
		if (result == -1)	{
			printf("Cannot allocate shared memory!\n");
		}
		cli_attach_memory( );
		cli_fill_memory  ( );				
		printf("Saving segment id: ");
		cli_save_segment_id( cli_segment_id_filename );		
		if ((ipc_memory_client!=(struct client_ipc_memory_map*)-1) && (ipc_memory_client != NULL))
			return 1;
	}
	 else  
	{	
		printf( "Reading segment id: %s\n", cli_segment_id_filename );
		cli_read_segment_id( cli_segment_id_filename );
		cli_attach_memory();	
		if ((ipc_memory_client!=(struct client_ipc_memory_map*)-1) && (ipc_memory_client != NULL))
			return 1;			
	}
	return 0;	
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
	return ipc_memory_client->UpdateCounter;
}

BOOL cli_is_new_connection_status()
{
	int latest_count = read_status_counter();
	if (latest_count > ipc_memory_client->StatusAcknowledgedCounter)
	{
		//printf("Latest_count = %d/%d\n", latest_count, ipc_memory_client->StatusAcknowledgedCounter );
		return TRUE;
	}
	return FALSE;
}

void cli_ack_connection_status()
{
	ipc_memory_client->StatusAcknowledgedCounter = ipc_memory_client->StatusCounter;
}

BOOL cli_is_new_update()
{
	int latest_count = read_sentence_counter();
	if (latest_count > ipc_memory_client->AcknowledgedCounter)
	{
		//printf("Latest_count = %d/%d\n", latest_count, ipc_memory_client->AcknowledgedCounter);			
		return TRUE;
	}
	return FALSE;
}
void cli_ack_update_status()
{
	ipc_memory_client->AcknowledgedCounter = ipc_memory_client->UpdateCounter;
}

/*void cli_ipc_add_new_client( struct in_addr mbeacon_ip_list, char* mTextMsg )
{
	ipc_memory_client->NumberClients++;
	std::list<struct in_addr>::iterator iter = mbeacon_ip_list.end();
	int byte_array_size = 0;
	struct stClientData* ptr 			= ipc_memory_client->ClientArray;
	int  i=0;

	// Now add the client as a string followed by a null terminator.  Hence a string array:
	char* ip_str = inet_ntoa(*iter);
	int len = strlen(ip_str);
	
	// Copy 1 client:
	memcpy( ptr[i].address,  ip_str, len );
	memcpy( ptr[i].name,  ip_str, len );
	memcpy( ptr[i].machine,  ip_str, len );	
	
	// Adjust for next client:
	byte_array_size += (len+1);	// null terminator
} */

/*
	Puts a new ip into the shared memory  ipc_memory_client->ClientArray[]
*/
/*void cli_ipc_add_new_clients( std::list<struct in_addr> mbeacon_ip_list )
{
	ipc_memory_client->NumberClients    = mbeacon_ip_list.size();
	std::list<struct in_addr>::iterator iter = mbeacon_ip_list.begin();
	int byte_array_size = 0;
	char* ptr 			= ipc_memory_client->ClientArray;
	int  i=0;

	// Now add each client as a string followed by a null terminator.  Hence a string array:
	while(( iter != mbeacon_ip_list.end()) && (byte_array_size<MAX_CLIENT_ARRAY_SIZE))
	{
		char* ip_str = inet_ntoa(*iter);
		int len = strlen(ip_str);
		
		// Copy 1 client:
		memcpy( ptr[i],  ip_str, len );
		ptr[len] = 0;
		
		// Adjust for next client:
//		ptr      += (len+1);		// skip null terminator!
		byte_array_size += (len+1);	// null terminator
		iter++;
		i++;
	}
	
	ipc_memory_client->UpdateCounter++;	
}*/

void cli_print_clients()
{
	if (ipc_memory_client==NULL) return ;
	
	int size = ipc_memory_client->NumberClients;
	printf("There are %d available clients.\n", size );

	struct stClientData* ptr = ipc_memory_client->ClientArray;		
	for (int i=0; i<size; i++)
	{
		printf(" %s\t%s\t%s \n", ptr[i].name, ptr[i].address, ptr[i].machine );
	}
	return ;
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
