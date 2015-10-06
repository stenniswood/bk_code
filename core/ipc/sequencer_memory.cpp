
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
#include "sequencer_memory.hpp"


char* 	sequencer_shared_memory;
int 	sequencer_segment_id;
struct  sequencer_ipc_memory_map* ipc_memory_sequencer =NULL;

 
void seq_dump_ipc()
{
	int length = sizeof(struct sequencer_ipc_memory_map);
	for (int i=0; i<length; i++)
	{
		if ((i%32)==0)
			printf("\n");
		printf("%2x ", sequencer_shared_memory[i] );
	}	
}

void seq_save_segment_id(char* mFilename)
{
	FILE* fd = fopen(mFilename, "w");
	if (fd==NULL) {
		printf("Cannot open file %s. %s \n", mFilename, strerror(errno) );		
	}
	
	//FILE* fd = fopen("client_shared_memseg_id.cfg", "w");
	printf("Segment_id=%d\n", sequencer_segment_id );
	char line[80];
	//fprintf( fd, "%d", sequencer_segment_id );
	sprintf( line, "%d", sequencer_segment_id );
	fwrite ( line, strlen(line), 1, fd  );
	fclose( fd );
}
int seq_read_segment_id(char* mFilename)
{
	char tline[40];
	FILE* fd = fopen( mFilename, "r" );
	if (fd==NULL)  {
		printf("seq_read_segment_id - ERROR: %s \n", strerror(errno) );	
		return -1;
	}
	int result = fread( tline, 1, 20, fd);		//	fscanf( fd, "%d", &can_segment_id );
	tline[result] = 0;
	sequencer_segment_id = atol( tline );
	fclose( fd );
	printf("seq_segment_id= %d \n", sequencer_segment_id );	
	return sequencer_segment_id;
}

int seq_allocate_memory( )
{
	const int shared_segment_size = sizeof(struct sequencer_ipc_memory_map);
	printf ("Client shm seg size=%d\n", shared_segment_size );
	
	/* Allocate a shared memory segment. */
	sequencer_segment_id = shmget( IPC_KEY_CLI, shared_segment_size, IPC_CREAT | 0666 );
	int errsv = errno;
	if (sequencer_segment_id==-1)
		printf("seq_allocate_memory - ERROR: %s \n", strerror(errsv) );
	else 
		printf ("Client shm segment_id=%d\n", sequencer_segment_id );
	// IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	return sequencer_segment_id;
}

int seq_attach_memory()
{
	/* Attach the shared memory segment. */
	int error=0;
	sequencer_shared_memory = (char*) shmat (sequencer_segment_id, 0, 0);
	if (sequencer_shared_memory==(char*)-1) {
		printf("seq_attach_memory - ERROR: %s \n", strerror(errno) );
		return 0;
	} else 
		printf ("Client shm attached at address %p\n", sequencer_shared_memory); 	
	
	ipc_memory_sequencer		 = (struct sequencer_ipc_memory_map*)sequencer_shared_memory;
	return 1;
}

void seq_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	sequencer_shared_memory = (char*) shmat (sequencer_segment_id, (void*) 0x5000000, 0); 
	if (sequencer_shared_memory==(char*)-1) {
		printf("seq_attach_memory - ERROR: %s \n", strerror(errno) );
		return;
	}
	ipc_memory_sequencer	 = (struct sequencer_ipc_memory_map*)sequencer_shared_memory;	
	printf ("Client shm reattached at address %p\n", sequencer_shared_memory); 
}

void seq_detach_memory()
{
	/* Detach the shared memory segment. */
	shmdt (sequencer_shared_memory);
}

int seq_get_segment_size()
{
	struct 		shmid_ds	shmbuffer;
	/* Determine the segment’s size. */
	shmctl (sequencer_segment_id, IPC_STAT, &shmbuffer);
	int segment_size = shmbuffer.shm_segsz;
	printf ("Client segment size: %d\n", segment_size);
	return segment_size;
}
void seq_fill_memory()
{
	int size = seq_get_segment_size();
	memset(sequencer_shared_memory, 0, size);
//	for (int i=0; i<size; i++)
//		sequencer_shared_memory[i] = (i%128); 
}

void seq_deallocate_memory(int msegment_id)
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

void seq_ipc_write_connection_status( char* mStatus )
{
	int length = strlen(mStatus);	
	int MaxAllowedLength = sizeof(ipc_memory_sequencer->ConnectionStatus);
	if (length > MaxAllowedLength) {
		length = MaxAllowedLength;	// do not write into memory of next variable.
		mStatus[MaxAllowedLength] = 0;
	}
	ipc_memory_sequencer->StatusCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", StatusCounter, length );
	strcpy( ipc_memory_sequencer->ConnectionStatus, mStatus);
	printf( "|%s|\n", ipc_memory_sequencer->ConnectionStatus );
}

/*void seq_ipc_write_active_page( short NewActivePage )
{
	ipc_memory_avis->ScreenNumber = NewActivePage;
} */

/* See udp_transponder for update_client_list()		*/

 
#if (PLATFORM==Mac)
char seq_segment_id_filename[] = "/Users/stephentenniswood/code/bk_code/client/seq_shared_memseg_id.cfg";
#elif (PLATFORM==RPI)
char seq_segment_id_filename[] = "/home/pi/bk_code/client/seq_shared_memseg_id.cfg";
#elif (PLATFORM==linux_desktop)
char seq_segment_id_filename[] = "/home/steve/bk_code/client/seq_shared_memseg_id.cfg";
#endif


/* The allocating should be done by abkInstant. 
		Should test for mem segment allocation first.
		Then if requested allocate.
Return : 
	1 => Attached to memory successfully.
	0 => No connection.
*/

int seq_connect_shared_sequencer_memory( char mAllocate )
{
	if (mAllocate)
	{
		int result = seq_allocate_memory( );
		if (result == -1)	{
			printf("Cannot allocate shared memory!\n");
		}
		seq_attach_memory( );
		seq_fill_memory  ( );				
		printf("Saving segment id: ");
		seq_save_segment_id( seq_segment_id_filename );		
		if ((ipc_memory_sequencer!=(struct sequencer_ipc_memory_map*)-1) && (ipc_memory_sequencer != NULL))
			return 1;
	}
	 else  
	{	
		printf( "Reading segment id: %s\n", seq_segment_id_filename );
		seq_read_segment_id( seq_segment_id_filename );
		seq_attach_memory();	
		if ((ipc_memory_sequencer!=(struct sequencer_ipc_memory_map*)-1) && (ipc_memory_sequencer != NULL))
			return 1;			
	}
	return 0;	
}
 
char* get_connection_status()
{
	return (ipc_memory_sequencer->ConnectionStatus);
}

/******************** _is_new_____ functions *************************/
BOOL  seq_is_new_connection_status	()
{ 
	if (ipc_memory_sequencer==NULL) return FALSE;
	
	if (ipc_memory_sequencer->StatusCounter > ipc_memory_sequencer->StatusAcknowledgedCounter)
	{
		return TRUE;
	}
	return FALSE;
	
}
BOOL  seq_is_new_performance_status	()
{
	if (ipc_memory_sequencer==NULL) return FALSE;
	if (ipc_memory_sequencer->PerformanceUpdateCounter > ipc_memory_sequencer->PerformanceAcknowledgedCounter)
	{
		return TRUE;
	}
	return FALSE;	 
}
BOOL  seq_is_new_diagnostic_status	()
{ 
	if (ipc_memory_sequencer==NULL) return FALSE;
	if (ipc_memory_sequencer->DiagnosticUpdateCounter > ipc_memory_sequencer->DiagnosticAcknowledgedCounter)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL  seq_is_new_enable_status	    ()
{ 
	if (ipc_memory_sequencer==NULL) return FALSE;
	if (ipc_memory_sequencer->EnableUpdateCounter > ipc_memory_sequencer->EnableAcknowledgedCounter)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL  seq_is_new_vector_status	    ()
{ 
	if (ipc_memory_sequencer==NULL) return FALSE;
	if (ipc_memory_sequencer->VectorUpdateCounter > ipc_memory_sequencer->VectorAcknowledgedCounter)
	{
		//printf("Latest_count = %d/%d\n", latest_count, ipc_memory_sequencer->PerformanceAcknowledgedCounter );
		return TRUE;
	}
	return FALSE;
}
/********** BEGIN _acknowledgement___ functions *************************/
void seq_ack_connection_status	()
{ 
	ipc_memory_sequencer->StatusAcknowledgedCounter = ipc_memory_sequencer->StatusCounter;
}
void seq_ack_performance_status	()
{ 
	ipc_memory_sequencer->PerformanceAcknowledgedCounter = ipc_memory_sequencer->PerformanceUpdateCounter;
}
void seq_ack_diagnostic_status	()
{ 
	ipc_memory_sequencer->DiagnosticAcknowledgedCounter = ipc_memory_sequencer->DiagnosticUpdateCounter;
}
void seq_ack_enable_status	()
{ 
	ipc_memory_sequencer->EnableAcknowledgedCounter = ipc_memory_sequencer->EnableUpdateCounter;
}
void seq_ack_vector_status	()
{ 
	ipc_memory_sequencer->VectorAcknowledgedCounter = ipc_memory_sequencer->VectorUpdateCounter;
}
/********** END _acknowledgement___ functions *************************/

void seq_print_sequence()
{
	if (ipc_memory_sequencer==NULL) return ;
	
	int size = ipc_memory_sequencer->NumberSequences;
	printf("There are %d available clients.\n", size );

	struct stBodyPositionVector* ptr = ipc_memory_sequencer->SequenceArray;
	for (int i=0; i<size; i++)
	{
		int servos = ptr[i].num_active_servos;
		for(int j=0; j<servos; j++)
		printf(" %6.1f\t\n", ptr[i].servo_values[j] );
	}
	return ;
}



/*void seq_ipc_add_new_client( struct in_addr mbeacon_ip_list, char* mTextMsg )
{
	ipc_memory_sequencer->NumberClients++;
	std::list<struct in_addr>::iterator iter = mbeacon_ip_list.end();
	int byte_array_size = 0;
	struct stClientData* ptr 			= ipc_memory_sequencer->ClientArray;
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
	Puts a new ip into the shared memory  ipc_memory_sequencer->ClientArray[]
*/
/*void seq_ipc_add_new_clients( std::list<struct in_addr> mbeacon_ip_list )
{
	ipc_memory_sequencer->NumberClients    = mbeacon_ip_list.size();
	std::list<struct in_addr>::iterator iter = mbeacon_ip_list.begin();
	int byte_array_size = 0;
	char* ptr 			= ipc_memory_sequencer->ClientArray;
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
	
	ipc_memory_sequencer->UpdateCounter++;	
}*/

/*void seq_ipc_add_new_client 	( char* mClientText )
{
	int size  = ipc_memory_sequencer->NumberClients;
	char* ptr = ipc_memory_sequencer->ClientArray;
	char* ptr2;
	for (int i=0; i<(size-1); i++)
	{
		ptr2 = strchr( ptr, 0 );
		if (ptr2)
			ptr = ptr2+1;
	}
	strcpy( ptr, mClientText );
	ipc_memory_sequencer->NumberClients++;
}*/
