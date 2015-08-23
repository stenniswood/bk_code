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
#define _XOPEN_SOURCE
#include <stdio.h> 
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
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


static void dump_ipc()
{
	printf("StatusCounter=%d\n", 	  	ipc_memory_can->StatusCounter);
	printf("Connection Status: %s\n", 	ipc_memory_can->ConnectionStatus );
	
	printf("RxHead=%d\n", 				ipc_memory_can->RxHead );
	printf("RxHead Laps=%d\n", 			ipc_memory_can->RxHeadLap );
	
	printf("TxHead=%d\n", 				ipc_memory_can->TxHead );
	printf("TxTail=%d\n", 				ipc_memory_can->TxTail );
		
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
	can_segment_id = shmget( IPC_KEY_CAN, shared_segment_size, IPC_CREAT | 0666 );
	if (can_segment_id==-1) {
		printf("CAN_read_segment_id - ERROR: %s \n", strerror(errno) );
		return 0;
	} else 
		printf ("CAN Allocated %d bytes shm segment_id=%d\n", shared_segment_size, can_segment_id );
		// IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	return can_segment_id;	
}

int can_attach_memory()
{
	/* Attach the shared memory segment. */
	can_shared_memory = (char*)shmat( can_segment_id, 0, 0 );
	if (can_shared_memory==-1) {
		printf("can_attach_memory - ERROR: %s \n", strerror(errno) );
		return 0;
	} else 
		printf ("CAN shm attached at address %p\n", can_shared_memory); 		
	ipc_memory_can	  = (struct can_ipc_memory_map*)can_shared_memory;
	return 1;
}

void can_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	can_shared_memory = (char*) shmat (can_segment_id, (void*) 0x5000000, 0); 
	if (can_shared_memory==-1) {
		printf("can_attach_memory - ERROR: %s \n", strerror(errno) );
	} else 
		printf ("CAN shm reattached at address %p\n", can_shared_memory); 	
	ipc_memory_can	  = (struct can_ipc_memory_map*)can_shared_memory;	
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
	if (Debug) printf ("CAN segment size: %d\n", segment_size);
	return segment_size;
}
void can_fill_memory()
{
	memset(can_shared_memory, 0, can_get_segment_size());
}

void can_deallocate_memory()
{
	/* Deallocate the shared memory segment. */ 
	shmctl (can_segment_id, IPC_RMID, 0);
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
void copy_can_msg( struct sCAN* mDest, struct sCAN* mSrc )
{
	memcpy( (void*)mDest, (void*)mSrc, sizeof(struct sCAN) );	
}

void ipc_add_can_rx_message( struct sCAN* mMsg )
{		
	copy_can_msg( &(ipc_memory_can->Received[ipc_memory_can->RxHead++]), mMsg );
	
	if (ipc_memory_can->RxHead > MAX_CAN_RX_MESSAGES) {
		ipc_memory_can->RxHead = 0;
		ipc_memory_can->RxHeadLap++;
	}
}

void AddToRxList( struct sCAN* mMsg )
{
	ipc_add_can_rx_message( mMsg );
}

BOOL  shm_isRxMessageAvailable( int* mTail, int* mTailLaps )
{
	BOOL Overrun = FALSE;
	if ((ipc_memory_can==NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
		return FALSE;

	int tail_count = *mTailLaps 			   * MAX_CAN_RX_MESSAGES + *mTail;	
	int head_count = ipc_memory_can->RxHeadLap * MAX_CAN_RX_MESSAGES + ipc_memory_can->RxHead;	
	int msgs_qued = head_count - tail_count;	
	if (msgs_qued >= MAX_CAN_RX_MESSAGES)		// overrun
	{											// scoot tail up keeping as many msgs as possible.
		Overrun    = TRUE;
		*mTailLaps = ipc_memory_can->RxHeadLap - 1;
		tail_count = head_count - (MAX_CAN_RX_MESSAGES-10);	// allow room for 10 more, since the Tail is slower than the head.
		*mTail = (tail_count - *mTailLaps * MAX_CAN_RX_MESSAGES);		
	}
	
//	if (head_count < tail_count)
//		printf("h/t: %d/%d \n", head_count, tail_count );
	/* Not new way.
	if (ipc_memory_can->RxTail == ipc_memory_can->RxHead)
		ipc_memory_can->RxTail = ipc_memory_can->RxHead = 0; 
	(ipc_memory_can->RxHead > ipc_memory_can->RxTail);		*/	
	return (head_count > tail_count);
}

struct sCAN* shm_GetNextRxMsg( int* mTail, int* mTailLaps )
{
	static struct sCAN tmp;	

	if ((ipc_memory_can==NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
		return NULL;
	
	copy_can_msg( &tmp, &(ipc_memory_can->Received[*mTail]) );
	(*mTail)++;
	if (*mTail > MAX_CAN_RX_MESSAGES) {
		*mTail = 0;
		(*mTailLaps)++;
	}
	
	if (ipc_memory_can->RxHead == *mTail)
		ipc_memory_can->RxOverFlow = 0;	// clear it since no more.

	return &tmp;
}


void CAN_save_segment_id(char* mFilename)
{
	char line[40];
	//FILE* fd = fopen(mFilename, "w");
	FILE* fd = fopen("acan_shared_memseg_id.cfg", "w");
	printf("Saved Segment_id=%d\n", can_segment_id );
	sprintf( line, "%d", can_segment_id );
	fwrite( line, strlen(line), 1, fd );
	fclose( fd );
}

int CAN_read_segment_id(char* mFilename)
{
	char tline[40];
	FILE* fd = fopen( mFilename, "r" );
	if (fd==NULL)  {
		printf("CAN_read_segment_id - ERROR: %s %s\n", strerror(errno), mFilename );	
		return -1;
	}

	int result = fread( tline, 1, 20, fd);		//	fscanf( fd, "%d", &can_segment_id );
	tline[result] = 0;
	can_segment_id = atol( tline );
	fclose( fd );
	printf("can_segment_id= %d \n", can_segment_id );	
	return can_segment_id;
}


int can_connect_shared_memory(char mAllocate)
{
	if (mAllocate) {		
		// First see if the memory is already allocated:
		//CAN_read_segment_id("/home/pi/bk_code/amonitor/acan_shared_memseg_id.cfg");
	    //shmctl(can_segment_id, IPC_RMID, NULL);
		//printf("can_connect_shared_memory() attempting to allocate.\n");
		// Now Re-allocate : 
		int result = can_allocate_memory();
		if (result) 
			printf("ERROR: can_connect_shared_memory() cannot allocate.\n");
		
		can_attach_memory();
		can_fill_memory	 ();
		CAN_save_segment_id("/home/pi/bk_code/amonitor/acan_shared_memseg_id.cfg");			
		if ((ipc_memory_can == NULL) || (ipc_memory_can==(can_ipc_memory_map*)-1))
			return 1;

	} else  {
		CAN_read_segment_id("/home/pi/bk_code/amonitor/acan_shared_memseg_id.cfg");	
		can_attach_memory();		
		if ((ipc_memory_can == NULL) || (ipc_memory_can==(can_ipc_memory_map*)-1))
			return 1;
	}
	return 0;
}

