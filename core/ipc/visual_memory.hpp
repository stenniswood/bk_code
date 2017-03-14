#ifndef _IPC_AVISUAL_HPP_
#define _IPC_AVISUAL_HPP_

#include "global.h"
#include "machine_defs.h"	// base path for segment_id filename.
#include "shm_base.hpp"


#define MAX_CLIENT_ARRAY_SIZE 2048
#define IPC_KEY_VIS 0x04D0

/******************** AVISUAL MEMORY MAP *****************/
struct avisual_ipc_memory_map
{
	long int StatusCounter;
	Uint32_t StatusAcknowledgedCounter;	
	char	 ConnectionStatus[64];

	long int SentenceCounter;
	Uint32_t SentenceAcknowledgedCounter;
	char	 Sentence        [128];	

	char	 Response[255];								// verbal commands (NLP) activate connection requests.
	Uint32_t ResponseCounter;							// Incremented on change to any of below:
	Uint32_t ResponseAcknowledgedCounter;				// Incremented on change to any of below:
    char     ResponderName[100];                        // "instant", "simulator", etc

	//int		 NumberClients;
	//char	 ClientArray[MAX_CLIENT_ARRAY_SIZE];		// String array (dimension of NumberClients)
	//short	 ScreenNumber;			// Which screen is being displayed.  voice commands can change.  Simplistic for now a single number per page.	
};
/******************** AVISUAL MEMORY MAP *****************/


/* This class will be the base for any shared memory 
*/
class aVisualMemory : public SHMBase
{
public:
	aVisualMemory ( );
	~aVisualMemory( );
	
	struct avisual_ipc_memory_map*  get_memory_avis() { return (struct avisual_ipc_memory_map*)m_shared_memory; };
	void 	ipc_write_connection_status		( char* mStatus   );
	void 	ipc_write_command_text	   		( char* mSentence );
	
	char*	get_connection_status			();
	BOOL 	is_new_connection_status		();
	int 	read_status_counter				();
	void 	ack_status_counter				();
	
	char*	get_sentence					();
	BOOL 	is_new_sentence					();
	int  	read_sentence_counter			();	
	void 	ack_sentence_counter			();

	char*	get_response					();
	BOOL 	is_new_response					();
	void 	ack_response_counter			();

};


#endif
