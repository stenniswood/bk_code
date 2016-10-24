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

WRITES TO SHARED VISION MEMORY:
	The "abkInstant" establishes the memory segment.
	and the avisual may connect to it when it's run.
READS:
	
DATE 	:  8/8/2016
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdio.h> 
#include <sys/stat.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <vector>
#include <string>
#include <assert.h>
#include "global.h"
//#include "bk_system_defs.h"
#include "vision_memory.h"


#define Debug 0

char* 	eyes_shared_memory;
int 	eyes_segment_id;
struct  eyes_ipc_memory_map* ipc_memory_eyes=NULL;

#if (PLATFORM==Mac)
char eyes_segment_id_filename[] = "/Users/stephentenniswood/code/Mac/bk_code/shm_ids/eyes_segment_id.cfg";
#elif (PLATFORM==RPI)
char eyes_segment_id_filename[] = "/home/pi/bk_code/shm_ids/eyes_shared_memseg_id.cfg";
#elif (PLATFORM==linux_desktop)
char eyes_segment_id_filename[] = "/home/steve/bk_code/shm_ids/eyes_shared_memseg_id.cfg";
#endif

bool is_eyes_ipc_memory_available()
{
    struct shmid_ds buf;			// shm data descriptor.
    Dprintf("Checking for vision IPC memory... segment id: %s\n", eyes_segment_id_filename );

    // First see if the memory is already allocated:
    eyes_segment_id = eyes_read_segment_id( eyes_segment_id_filename );

    int retval = shmctl(eyes_segment_id, IPC_STAT, &buf);
    if (retval==-1) {
		perror("Vision Memory : shmctl() Error");
        //Dprintf("Error: %s\n", strerror(errno) );
        return false;
    }
    printf( "Vision_memory:  Found segment %d, size=%ld and %d attachments.\n", eyes_segment_id, buf.shm_segsz, buf.shm_nattch );
    
    if ((buf.shm_segsz > 0)			// segment size > 0
        && (buf.shm_nattch >= 0))	// number of attachments.
        return true;
    
    return false;
}

void eyes_ipc_write_connection_status( char* mStatus )
{
	long length = strlen(mStatus);
	int MaxAllowedLength = sizeof(ipc_memory_eyes->ConnectionStatus);
	if (length > MaxAllowedLength)
		length = MaxAllowedLength;	// do not write into memory of next variable.

	ipc_memory_eyes->StatusCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", StatusCounter, length );
	strcpy( ipc_memory_eyes->ConnectionStatus, mStatus);
	Dprintf( "|%s|\n", ipc_memory_eyes->ConnectionStatus );
}

/* 
Return :
    1 => Attached to memory successfully.
    0 => No connection.
*/
int eyes_connect_shared_memory( char mAllocate )
{
    bool available = is_eyes_ipc_memory_available();
    
    if ((!available) && (mAllocate))
    {
        int result = eyes_allocate_memory( );
        if (result == -1)	{
            printf("Cannot allocate shared memory!\n");
        }
        eyes_attach_memory( );
        eyes_fill_memory  ( );

        Dprintf("Saving segment id: ");
        eyes_save_segment_id( eyes_segment_id_filename );
        if ((ipc_memory_eyes!=(struct eyes_ipc_memory_map*)-1) && (ipc_memory_eyes != NULL))
            return 1;
    }
    else
    {
        eyes_attach_memory();
        if ((ipc_memory_eyes!=(struct eyes_ipc_memory_map*)-1) && (ipc_memory_eyes != NULL))
            return 1;			
    }
    return 0;	
}

bool eyes_new_command_available( )
{
//	printf("eyes_new_command_available()  %d %d \n", ipc_memory_eyes->CommandCounter, 
//													ipc_memory_eyes->AcknowledgeCounter);
    bool retval = (ipc_memory_eyes->CommandCounter > ipc_memory_eyes->AcknowledgeCounter);
    return retval;
}

/*	 INSERT _WRITE_CLIENT_ HERE 	*/

void eyes_acknowledge_command( )
{
    ipc_memory_eyes->AcknowledgeCounter = ipc_memory_eyes->CommandCounter;
	//printf("eyes_acknowledge_command()  %d %d \n", ipc_memory_eyes->CommandCounter, 
	//												ipc_memory_eyes->AcknowledgeCounter);

}

void eyes_wait_for_acknowledgement( )
{
    bool still_available=eyes_new_command_available();
    while( still_available )
    {
        still_available = eyes_new_command_available();
    }
}

/************* REPONSE WRAPPERS ****************/
bool eyes_new_server_event_available( )
{	
	//printf("server avail:  %d > %d ?\n", ipc_memory_eyes->ServerCounter,ipc_memory_eyes->ServerAcknowledgedCounter );

    bool   retval = (ipc_memory_eyes->ServerCounter > ipc_memory_eyes->ServerAcknowledgedCounter);
    return retval;
}

void  eyes_write_client_command   	( std::string mCommand )
{
	//long length = strlen(mSentence);
	long length = mCommand.length();	
	int MaxAllowedLength = sizeof(ipc_memory_eyes->client_command);
	if (length>MaxAllowedLength)
		length = MaxAllowedLength;
		
	ipc_memory_eyes->CommandCounter++;
	strcpy(ipc_memory_eyes->client_command, mCommand.c_str() );
}

/* WRITE IMPLIES TO SHARED MEMORY.  And since we are the abkInstant task,
	the writes will be transfer to avisual
FORMAT:
	INT 		ID
	char[255] 	Buffer
*/
void  eyes_write_server_event( std::string mCommand )
{
	long length = mCommand.length();	
	int MaxAllowedLength = sizeof(ipc_memory_eyes->ServerEvent);
	if (length>MaxAllowedLength)
		length = MaxAllowedLength;
	ipc_memory_eyes->ServerCounter++;

	strcpy(ipc_memory_eyes->ServerEvent, mCommand.c_str() );
}

void  eyes_acknowledge_server   ( )
{
    ipc_memory_eyes->ServerAcknowledgedCounter++;
}
void  eyes_wait_for_server      ( )
{
    bool   responded = eyes_new_server_event_available();
    while( !responded )
        responded = eyes_new_server_event_available();
}
/************* END REPONSE WRAPPERS ****************/

int eyes_allocate_memory( )
{
	const int shared_segment_size = sizeof(struct eyes_ipc_memory_map);
    Dprintf("eyes shared_seg_size=%d\n", shared_segment_size);

	/* Allocate a shared memory segment. */
	eyes_segment_id = shmget( IPC_KEY_EYES, shared_segment_size, IPC_CREAT | 0666 );
    if (errno>0)
        perror("shmget ");
    
	// IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	Dprintf ("eyes3D shm segment_id=%d;  ", eyes_segment_id );
	printf ("Vision memory: allocated %d bytes\n", shared_segment_size );
	return eyes_segment_id;
}

void eyes_deallocate_memory()
{
	/* Deallocate the shared memory segment. */ 
	shmctl (eyes_segment_id, IPC_RMID, 0);
}

long eyes_attach_memory()
{
	/* Attach the shared memory segment. */
	eyes_shared_memory = (char*) shmat (eyes_segment_id, 0, 0);
	ipc_memory_eyes			= (struct eyes_ipc_memory_map*)eyes_shared_memory;
	Dprintf ("eyes3D shm attached at address %p\n\n", eyes_shared_memory);
    return (long)eyes_shared_memory;
}

void eyes_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	eyes_shared_memory = (char*) shmat (eyes_segment_id, (void*) 0x5000000, 0); 
	ipc_memory_eyes			= (struct eyes_ipc_memory_map*)eyes_shared_memory;
	Dprintf ("eyes3D shm reattached at address %p\n", eyes_shared_memory);
}

void eyes_detach_memory()
{
	/* Detach the shared memory segment. */
	shmdt (eyes_shared_memory);
}


void eyes_dump_ipc()
{
	int length = sizeof(struct eyes_ipc_memory_map);
	for (int i=0; i<length; i++)
	{
		if ((i%32)==0)
			printf("\n");
		printf("%x ", eyes_shared_memory[i] );
	}	
}
unsigned long eyes_get_segment_size()
{
	struct 		shmid_ds	shmbuffer;
	/* Determine the segment’s size. */
	shmctl (eyes_segment_id, IPC_STAT, &shmbuffer);
	unsigned long segment_size = shmbuffer.shm_segsz;
	Dprintf ("eyes3D segment size: %lu %lu \n", segment_size, sizeof(struct eyes_ipc_memory_map) );
	return segment_size;
}

void eyes_fill_memory()
{ 
	printf("eyes_fill_memory()\n");
    long size = eyes_get_segment_size();
	printf("\n eyes_fill_memory() size=%d\n\n ", size);
	memset(eyes_shared_memory, 0, size);

//    for (int i=0; i<size; i++)
//        eyes_shared_memory[i] = i%255;    
}

void eyes_save_segment_id(char* mFilename)
{
	FILE* fd = fopen(mFilename, "w");
	//FILE* fd = fopen("eyesulator_shared_memseg_id.cfg", "w");
	Dprintf("Segment_id=%d\n", eyes_segment_id );
	fprintf( fd, "%d", eyes_segment_id );
	fclose( fd );
}
int eyes_read_segment_id(char* mFilename)
{
	FILE* fd = fopen( mFilename, "r" );
    if (fd) {
	fscanf( fd, "%d", &eyes_segment_id );
	fclose( fd );
    } else {
        eyes_segment_id = -1;
    }
	return eyes_segment_id;
}


#define INT_MAX 0xFFFF
void delete_all_shm()
{
    for (int id=0; id < INT_MAX; id++)
        shmctl(id, IPC_RMID, NULL);
}

char* eyes_compose_coordinate     ( MathVector& mNewVelocity )
{

}

bool  eyes_is_valid_pointer( )
{
	if ((ipc_memory_eyes==NULL) || (ipc_memory_eyes==(struct eyes_ipc_memory_map*)-1))
		return false;
	return true;
}

void eyes_compose_coordinate_xy   ( int mx, int my )
{
	if (eyes_is_valid_pointer()==false)
	{ 	printf("Invalid Pointer eyes_memory\n");  return; };
	
	int index = ipc_memory_eyes->stream1_index;
	ipc_memory_eyes->location_stream1[index].x = mx;
	ipc_memory_eyes->location_stream1[index].x = my;
	ipc_memory_eyes->location_stream1[index].z = 0.0;
	ipc_memory_eyes->stream1_index++;
	if (ipc_memory_eyes->stream1_index > MAX_STREAM_SIZE)
		ipc_memory_eyes->stream1_index = 0;
}

