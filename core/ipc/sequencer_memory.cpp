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
//#include "bk_system_defs.h"
#include "global.h"
#include "interrupt.h"
#include "sequencer_memory.hpp"


#define Debug 1

/*********************************************************/
/* Class Functions 										 */
/*********************************************************/

SequencerIPC::SequencerIPC()	
: SHMBase(IPC_KEY_SEQ, sizeof(struct sequencer_ipc_memory_map), "seq_shared_memseg_id.cfg" )
{
}
SequencerIPC::~SequencerIPC()
{
}

int SequencerIPC::read_sentence_counter()
{
	struct sequencer_ipc_memory_map* ipc_memory = get_memory_seq();
	return ipc_memory->SentenceCounter;
}
char* SequencerIPC::get_sentence()
{
	struct sequencer_ipc_memory_map* ipc_memory = get_memory_seq();
	return (ipc_memory->Sentence);
}
BOOL SequencerIPC::is_new_sentence()
{
	struct sequencer_ipc_memory_map* ipc_memory = get_memory_seq();
	if (ipc_memory->SentenceCounter  > ipc_memory->SentenceAcknowledgedCounter)
	{
		return TRUE;
	}
	return FALSE;
}
void SequencerIPC::ack_sentence_counter	()
{
	struct sequencer_ipc_memory_map* ipc = get_memory_seq();
	ipc->SentenceAcknowledgedCounter = ipc->SentenceCounter;
}

void SequencerIPC::wait_for_ack_sentence_counter( )
{
	struct sequencer_ipc_memory_map* ipc = get_memory_seq();
	Dprintf("sentence counter=%d/%d;\n", ipc->SentenceAcknowledgedCounter, ipc->SentenceCounter );
	while (ipc->SentenceAcknowledgedCounter < ipc->SentenceCounter)
	{};
}

void SequencerIPC::write_sentence( char* mSentence )
{
	struct sequencer_ipc_memory_map* ipc_memory = get_memory_seq();

	int length = strlen(mSentence);	
	int MaxAllowedLength = sizeof(ipc_memory->Sentence);	
	if (length>MaxAllowedLength)
		length = MaxAllowedLength;
		
	strcpy(ipc_memory->Sentence, mSentence);
	ipc_memory->SentenceCounter++;
	printf("|%s|:%d\n", ipc_memory->Sentence, ipc_memory->SentenceCounter );	
}

/* WRITE IMPLIES TO SHARED MEMORY.  And since we are the abkInstant task,
	the writes will be transfer to avisual
FORMAT:
	INT 		ID
	char[255] 	Buffer
*/

void SequencerIPC::write_connection_status( char* mStatus )
{
	struct  sequencer_ipc_memory_map* seq = get_memory_seq();
	
	int length = strlen(mStatus);		
	int MaxAllowedLength = sizeof(seq->ConnectionStatus);
	if (length > MaxAllowedLength) {
		length = MaxAllowedLength;	// do not write into memory of next variable.
		mStatus[MaxAllowedLength] = 0;
	}
	seq->StatusCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", StatusCounter, length );
	strcpy( seq->ConnectionStatus, mStatus);
	printf( "|%s|\n", seq->ConnectionStatus );
}


bool SequencerIPC::ipc_write_sequence( int mIndex, struct stBodyPositionVector* mBP )
{
	struct  sequencer_ipc_memory_map* seq = get_memory_seq();
    if ((mIndex >= MAX_SEQUENCES) || (seq == NULL))
        return false;
    
    seq->SequenceArray[mIndex] = *mBP;
    return true;
}

// Return true if added.
bool SequencerIPC::ipc_add_sequence( struct stBodyPositionVector*  mVector )
{
	struct  sequencer_ipc_memory_map* seq = get_memory_seq();
    if (seq == NULL)  return false;
    
    int index = seq->NumberSequences++;
    if (seq->NumberSequences >= MAX_SEQUENCES)
        seq->NumberSequences = 0;
    
    return ipc_write_sequence( index, mVector);
}


/* See udp_transponder for update_client_list()		*/


/* The allocating should be done by abkInstant. 
		Should test for mem segment allocation first.
		Then if requested allocate.
Return : 
	1 => Attached to memory successfully.
	0 => No connection.
*/

 
char* SequencerIPC::get_connection_status()
{
	struct  sequencer_ipc_memory_map* seq = get_memory_seq();
	return (seq->ConnectionStatus);
}

/******************** _is_new_____ functions *************************/
BOOL  SequencerIPC::is_new_connection_status	()
{ 
	struct  sequencer_ipc_memory_map* seq = get_memory_seq();
	if (seq==NULL) return FALSE;
	
	if (seq->StatusCounter > seq->StatusAcknowledgedCounter)
		return TRUE;
	else 
		return FALSE;
}
BOOL  SequencerIPC::is_new_performance_status	()
{
	struct  sequencer_ipc_memory_map* seq = get_memory_seq();
	if (seq==NULL) return FALSE;
	if (seq->PerformanceUpdateCounter > seq->PerformanceAcknowledgedCounter)
		return TRUE;
	else 
		return FALSE;	 
}
BOOL  SequencerIPC::is_new_diagnostic_status	()
{ 
	struct  sequencer_ipc_memory_map* ipc_memory_sequencer = get_memory_seq();
	if (ipc_memory_sequencer==NULL) return FALSE;
	if (ipc_memory_sequencer->DiagnosticUpdateCounter > ipc_memory_sequencer->DiagnosticAcknowledgedCounter)
		return TRUE;
	else 
		return FALSE;
}
BOOL  SequencerIPC::is_new_enable_status	    ()
{ 
	struct  sequencer_ipc_memory_map* ipc_memory_sequencer = get_memory_seq();
	if (ipc_memory_sequencer==NULL) return FALSE;
	if (ipc_memory_sequencer->EnableUpdateCounter > ipc_memory_sequencer->EnableAcknowledgedCounter)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL  SequencerIPC::is_new_vector_status	    ()
{ 
	struct  sequencer_ipc_memory_map* ipc_memory_sequencer = get_memory_seq();
	if (ipc_memory_sequencer==NULL) return FALSE;
	if (ipc_memory_sequencer->VectorUpdateCounter > ipc_memory_sequencer->VectorAcknowledgedCounter)
	{
		//printf("Latest_count = %d/%d\n", latest_count, ipc_memory_sequencer->PerformanceAcknowledgedCounter );
		return TRUE;
	}
	return FALSE;
}
/********** BEGIN _acknowledgement___ functions *************************/
void SequencerIPC::ack_connection_status	()
{ 
	struct  sequencer_ipc_memory_map* ipc_memory_sequencer = get_memory_seq();
	ipc_memory_sequencer->StatusAcknowledgedCounter = ipc_memory_sequencer->StatusCounter;
}
void SequencerIPC::ack_performance_status	()
{ 
	struct  sequencer_ipc_memory_map* ipc_memory_sequencer = get_memory_seq();
	ipc_memory_sequencer->PerformanceAcknowledgedCounter = ipc_memory_sequencer->PerformanceUpdateCounter;
}
void SequencerIPC::ack_diagnostic_status	()
{ 
	struct  sequencer_ipc_memory_map* ipc_memory_sequencer = get_memory_seq();
	ipc_memory_sequencer->DiagnosticAcknowledgedCounter = ipc_memory_sequencer->DiagnosticUpdateCounter;
}
void SequencerIPC::ack_enable_status	()
{ 
	struct  sequencer_ipc_memory_map* ipc_memory_sequencer = get_memory_seq();
	ipc_memory_sequencer->EnableAcknowledgedCounter = ipc_memory_sequencer->EnableUpdateCounter;
}
void SequencerIPC::ack_vector_status	()
{ 
	struct  sequencer_ipc_memory_map* ipc_memory_sequencer = get_memory_seq();
	ipc_memory_sequencer->VectorAcknowledgedCounter = ipc_memory_sequencer->VectorUpdateCounter;
}
/********** END _acknowledgement___ functions *************************/

void SequencerIPC::print_sequence()
{
	struct  sequencer_ipc_memory_map* ipc_memory_sequencer = get_memory_seq();
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

/*void seq_ipc_write_active_page( short NewActivePage )
{
	ipc_memory->ScreenNumber = NewActivePage;
} */
