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
#include "bk_system_defs.h"
#include "interrupt.h"

#include "CAN_memory.h"


int 	can_segment_id;
char* 	can_shared_memory;							// unstructured
struct  can_ipc_memory_map* ipc_memory_can=NULL;	// structured memory.

#define Debug 1

static uint8_t print_message(struct sCAN* msg)
{	
	printf(" ID=%2X:%4X:%d; ", msg->id.group.block, 
			 msg->id.group.id, msg->id.group.instance );
	//printf("  %s\t", getID_Text( msg->id.group.id ) );
	printf("DLC=%d; ", msg->header.DLC );
	printf("D[]=");
	int i;
	for (i=0; i<8; i++)
	{
		if (i >= msg->header.DLC)
			printf("   ");
		else
			printf("%2x ", msg->data[i]);
	}
	if (msg->header.rtr==1)	
		printf("\tRTR\n");
	else
		printf("\tDF\n");
}

static void copy_can_msg( struct sCAN* mDest, struct sCAN* mSrc )
{
	memcpy( (void*)mDest, (void*)mSrc, sizeof(struct sCAN) );	
}

static void dump_ipc()
{
	printf("StatusCounter=%d\n", 	  	ipc_memory_can->StatusCounter);
	printf("Connection Status: %s\n", 	ipc_memory_can->ConnectionStatus );
	
	printf("RxHead=%d\n", 				ipc_memory_can->RxHead );
	printf("RxTail=%d\n", 				ipc_memory_can->RxTail );
	int NumMsgs = ipc_memory_can->RxTail-ipc_memory_can->RxHead;
	printf("Number of Messages = %d\n", NumMsgs );
	for (int i=ipc_memory_can->RxHead; i<ipc_memory_can->RxTail; i++)	
		print_message( &(ipc_memory_can->Received[i]) );

	printf("TxHead=%d\n", 				ipc_memory_can->TxHead );
	printf("TxTail=%d\n", 				ipc_memory_can->TxTail );
	NumMsgs = ipc_memory_can->RxTail-ipc_memory_can->RxHead;
	printf("Number of Messages = %d\n", NumMsgs );	
	for (int i=ipc_memory_can->TxHead; i<ipc_memory_can->TxTail; i++)			
		print_message( &ipc_memory_can->Transmit[i] );	
}

static void dump_raw_ipc()
{
	int length = sizeof(struct can_ipc_memory_map);
	for (int i=0; i<length; i++)
	{
		if ((i%32)==0)
			printf("\n");
		printf("%2x ", can_shared_memory[i] );
	}	
}
 
int can_allocate_memory( )
{
	const int 	shared_segment_size = sizeof(struct can_ipc_memory_map);

	/* Allocate a shared memory segment. */
	can_segment_id = shmget( IPC_KEY, shared_segment_size, IPC_CREAT | 0666 );

	// IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (Debug) printf ("shared memory segment_id=%d\n", can_segment_id );
	return can_segment_id;	
}

int can_attach_memory()
{
	/* Attach the shared memory segment. */
	can_shared_memory = (char*) shmat (can_segment_id, 0, 0);
	ipc_memory_can	  = (struct can_ipc_memory_map*)can_shared_memory;
	if (Debug) printf ("shared memory attached at address %p\n", can_shared_memory); 	
}

void can_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	can_shared_memory = (char*) shmat (can_segment_id, (void*) 0x5000000, 0); 
	ipc_memory_can	  = (struct can_ipc_memory_map*)can_shared_memory;	
	if (Debug) printf ("shared memory reattached at address %p\n", can_shared_memory); 
}

void can_detach_memory()
{
	/* Detach the shared memory segment. */
	shmdt (can_shared_memory);
}

int can_get_segment_size()
{
	struct 		shmid_ds	shmbuffer;
	/* Determine the segment’s size. */
	shmctl (can_segment_id, IPC_STAT, &shmbuffer);
	int segment_size = shmbuffer.shm_segsz;
	if (Debug) printf ("segment size: %d\n", segment_size);
	return segment_size;
}
void can_fill_memory()
{
	memset(can_shared_memory, 0, can_get_segment_size());
}

void can_deallocate_memory(int msegment_id)
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
void ipc_write_can_connection_status( char* mStatus )
{
	int length = strlen(mStatus);	
	int MaxAllowedLength = sizeof(ipc_memory_can->ConnectionStatus);
	if (length > MaxAllowedLength)
		length = MaxAllowedLength;	// do not write into memory of next variable.
	ipc_memory_can->ConnectionStatus[length] = 0;  // terminator.	
	ipc_memory_can->StatusCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", StatusCounter, length );
	strcpy( ipc_memory_can->ConnectionStatus, mStatus);
	if (Debug) printf( "|%s|\n", ipc_memory_can->ConnectionStatus );
}

void ipc_write_can_message( struct sCAN* mMsg )
{		
	copy_can_msg( &(ipc_memory_can->Transmit[ipc_memory_can->TxTail]), mMsg );

	ipc_memory_can->TxTail++;
	if (ipc_memory_can->TxTail > MAX_CAN_TX_MESSAGES)
		ipc_memory_can->TxTail = 0;
	
	if (ipc_memory_can->TxTail == ipc_memory_can->TxHead)
		ipc_memory_can->TxOverFlow = 1;
}

void ipc_add_can_rx_message( struct sCAN* mMsg )
{		
	copy_can_msg( &(ipc_memory_can->Received[ipc_memory_can->RxTail]), mMsg );

	ipc_memory_can->RxTail++;
	if (ipc_memory_can->RxTail>MAX_CAN_RX_MESSAGES)
		ipc_memory_can->RxTail = 0;
	
	if (ipc_memory_can->RxTail == ipc_memory_can->RxHead)
		ipc_memory_can->RxOverFlow = 1;
}

BOOL   		 isRxMessageAvailable( )
{
	return (ipc_memory_can->RxTail > ipc_memory_can->RxHead);
}

struct sCAN* GetNext			 ( )
{
	static struct sCAN tmp;	
	copy_can_msg( &tmp, &(ipc_memory_can->Received[ipc_memory_can->RxHead]) );

	ipc_memory_can->RxHead++;
	if (ipc_memory_can->RxHead>MAX_CAN_RX_MESSAGES)
		ipc_memory_can->RxHead = 0;
	
	if (ipc_memory_can->RxHead == ipc_memory_can->RxTail)
		ipc_memory_can->RxOverFlow = 0;	// clear it since no more.

	return &tmp;
}


/*void vis_save_segment_id(char* mFilename)
{
	FILE* fd = fopen(mFilename, "w");
	//FILE* fd = fopen("acan_shared_memseg_id.cfg", "w");
	printf("Segment_id=%d\n", can_segment_id );
	fprintf( fd, "%d", can_segment_id );
	fclose( fd );
}
int read_segment_id(char* mFilename)
{
	FILE* fd = fopen( mFilename, "r" );
	fscanf( fd, "%d", &can_segment_id );
	fclose( fd );
	return can_segment_id;
}*/


