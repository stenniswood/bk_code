#include <stdio.h> 
#include <sys/stat.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include "visual_memory.hpp"


aVisualMemory::aVisualMemory()
: SHMBase(IPC_KEY_VIS, sizeof(struct avisual_ipc_memory_map), "avisual_shm.cfg" )
{
}

aVisualMemory::~aVisualMemory()
{
	deallocate_memory();
}

void aVisualMemory::ipc_write_connection_status( char* mStatus )
{
	struct avisual_ipc_memory_map* ipc_memory_avis = get_memory_avis();

	int length = strlen(mStatus);	
	int MaxAllowedLength = sizeof(ipc_memory_avis->ConnectionStatus);
	if (length > MaxAllowedLength)
		length = MaxAllowedLength;	// do not write into memory of next variable.

	strcpy( ipc_memory_avis->ConnectionStatus, mStatus);
	printf( "|%s|\n", ipc_memory_avis->ConnectionStatus );

	ipc_memory_avis->StatusCounter++;
}

/* WRITE IMPLIES TO SHARED MEMORY.  And since we are the abkInstant task,
	the writes will be transfer to avisual
FORMAT:
	INT 		ID
	char[255] 	Buffer
*/
void aVisualMemory::ipc_write_command_text( char* mSentence )
{
	struct avisual_ipc_memory_map* ipc_memory_avis = get_memory_avis();

	int length = strlen(mSentence);	
	int MaxAllowedLength = sizeof(ipc_memory_avis->Sentence);	
	if (length>MaxAllowedLength)
		length = MaxAllowedLength;
		
	strcpy(ipc_memory_avis->Sentence, mSentence);
	printf("|%s|\n", ipc_memory_avis->Sentence );

	ipc_memory_avis->SentenceCounter++;
}


char* aVisualMemory::get_connection_status()
{
	struct avisual_ipc_memory_map* ipc_memory_avis = get_memory_avis();
	return (ipc_memory_avis->ConnectionStatus);
}
char* aVisualMemory::get_sentence()
{
	struct avisual_ipc_memory_map* ipc_memory_avis = get_memory_avis();
	return (ipc_memory_avis->Sentence);
}

int aVisualMemory::read_status_counter()
{
	struct avisual_ipc_memory_map* ipc_memory_avis = get_memory_avis();
	return ipc_memory_avis->StatusCounter;
}
int aVisualMemory::read_sentence_counter()
{
	struct avisual_ipc_memory_map* ipc_memory_avis = get_memory_avis();
	return ipc_memory_avis->SentenceCounter;
}

BOOL aVisualMemory::is_new_connection_status()
{
	struct avisual_ipc_memory_map* ipc_memory_avis = get_memory_avis();
	if (ipc_memory_avis->StatusCounter > ipc_memory_avis->StatusAcknowledgedCounter )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL aVisualMemory::is_new_sentence()
{
	struct avisual_ipc_memory_map* ipc_memory_avis = get_memory_avis();
	if (ipc_memory_avis->SentenceCounter  > ipc_memory_avis->SentenceAcknowledgedCounter)
	{
		return TRUE;
	}
	return FALSE;
}

void aVisualMemory::ack_status_counter	()
{ 
	struct avisual_ipc_memory_map* avis = get_memory_avis();
	avis->StatusAcknowledgedCounter = avis->StatusCounter;
}

void aVisualMemory::ack_sentence_counter	()
{
	struct avisual_ipc_memory_map* avis = get_memory_avis();
	avis->SentenceAcknowledgedCounter = avis->SentenceCounter;
}	

char*	aVisualMemory::get_response					()
{
	struct avisual_ipc_memory_map* avis = get_memory_avis();
	return avis->Response;
}
BOOL 	aVisualMemory::is_new_response					()
{
	struct avisual_ipc_memory_map* avis = get_memory_avis();
	if (avis->ResponseCounter > avis->ResponseAcknowledgedCounter)
		return TRUE;
	else
		return FALSE;
}
void 	aVisualMemory::ack_response_counter			()
{
	struct avisual_ipc_memory_map* avis = get_memory_avis();
	avis->ResponseAcknowledgedCounter = avis->ResponseCounter;
}

