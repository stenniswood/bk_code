#ifndef _SEQUENCER_MEMORY_HPP_
#define _SEQUENCER_MEMORY_HPP_

/************************************************************
	Data Flow - abkInstant data routed to this RPI device
	will delegate to this shared memory for a chance to handle.
	Provided that no other protocol upstream already responded.
	So the top_level_protocol.cpp  - pass_to_aux_apps() will 
	put the telegram into this shared memory.	aseq polls 
	
	This text will then pass thru aseq's own protocol (with response 
	if necessary; or simply move the motors - no verbal feedback).
	
	This can be greatly expanded - from simple commands such as:

	to low level commands such as:
		MOTOR SPEED: M1=125; M2=0; M3=23;  M4=-126; M5=15; M6=23;
		MOTOR POSITION: M1=125; M2=0; M3=23;  M4=-126; M5=15; M6=23;
	TO mid-level:
		Play sequence file "walk.csv", "dance.csv", "open_door.csv"
		etc.
	TO high-level:
		"sit", "stand", "half-step forward", "turn around", etc.	
		
	NOTICE:  There is also similarity between the simulator IPC memory
	and this - such as body pose.  Provision is made to load sequences
	from simulator to sequencer directly.
 ************************************************************/
#include <list>
#include "global.h"
#include "body_pose.h"

#define IPC_KEY_SEQ    0x04D5
#define MAX_SEQUENCES  300


struct stPerformanceData
{
	float	servo_request[MAX_SERVOS];	// Requested vector (copy of data)
	float	servo_result[MAX_SERVOS];	// Final destinations for the vector.
	float	servo_energy[MAX_SERVOS];
};

struct stDiagnosticData
{
	float	servo_request[MAX_SERVOS];	// Requested vector (copy of data)
	float	servo_result [MAX_SERVOS];	// Final destinations for the vector.
	float	servo_energy [MAX_SERVOS];
};



/******************** sequencer MEMORY MAP *****************/
struct sequencer_ipc_memory_map
{
	long int StatusCounter;
	char	 ConnectionStatus[64];
	long int StatusAcknowledgedCounter;

	long int SentenceCounter;
	Uint32_t SentenceAcknowledgedCounter;
	char	 Sentence        [128];	

	char	 Response[255];								// verbal commands (NLP) activate connection requests.
	Uint32_t ResponseCounter;							// Incremented on change to any of below:
	Uint32_t ResponseAcknowledgedCounter;				// Incremented on change to any of below:
    char     ResponderName[100];                        // "instant", "simulator", etc

	// PERFORMANCE DATA (for evaluation the robot's motion):
	long int PerformanceUpdateCounter;			// Incremented on change to any of below:
	long int PerformanceAcknowledgedCounter; 	
	struct stPerformanceData PerformanceArray[MAX_SERVOS];		// String array (dimension of NumberClients)

	// DIAGNOSTIC DATA :
	long int DiagnosticUpdateCounter;			// Incremented on change to any of below:
	long int DiagnosticAcknowledgedCounter; 	
	struct stDiagnosticData DiagnosticArray[MAX_SERVOS];		// String array (dimension of NumberClients)

	// ENABLES :
	long int EnableUpdateCounter;				// Incremented on change to any of below:
	long int EnableAcknowledgedCounter;
	BOOL	 EnableArray[MAX_SERVOS];		// String array (dimension of NumberClients)

	// VECTOR INPUTS:
	long int VectorUpdateCounter;					// Incremented on change to any of below:
	long int VectorAcknowledgedCounter; 	
	int		 NumberSequences;
	struct stBodyPositionVector SequenceArray[MAX_SEQUENCES];		// String array (dimension of NumberClients)
};
/******************** sequencer MEMORY MAP *****************/

//bool seq_new_command_available( );
//void seq_acknowledge_command( );
#include "shm_base.hpp"


class SequencerIPC : public SHMBase
{
	public:
	SequencerIPC();
	~SequencerIPC();
	
	struct  sequencer_ipc_memory_map* get_memory_seq()	{ return (struct  sequencer_ipc_memory_map*)m_shared_memory;  };
	
	/*********** MEMORY DATA ACCESS FUNCTIONS **************/
	// for instant to fill in the shared mem buffer:
	void write_connection_status( char* mStatus   );
	void write_performance_data ( struct stPerformanceData* mData  );
	void write_diagnostic_data  ( struct stDiagnosticData*  mData  );
	void write_enables	        ( BOOL* mEnableArray, int mSize    );
	void write_vector           ( struct stBodyPositionVector* mStatus );
	void write_sentence		   	( char* mSentence );
	
	void  print_sequence		    ( );

	char*	get_sentence					();
	BOOL 	is_new_sentence					();
	int  	read_sentence_counter			();	
	void 	ack_sentence_counter			();
	void	wait_for_ack_sentence_counter	();
	

	char* get_connection_status		();

	BOOL  is_new_connection_status	();
	BOOL  is_new_performance_status	();
	BOOL  is_new_diagnostic_status	();
	BOOL  is_new_enable_status	    ();
	BOOL  is_new_vector_status	    ();

	void  ack_connection_status		();
	void  ack_performance_status	();
	void  ack_diagnostic_status		();
	void  ack_enable_status			();
	void  ack_vector_status			();

	bool  ipc_add_sequence          ( struct stBodyPositionVector*  mVector );
	bool  ipc_write_sequence		( int mIndex, struct stBodyPositionVector* mBP );

};

/************************************************************/

#endif

/*
/*const int l_hip_rotate	 = 0;
const int l_hip_swing	 = 1;
const int l_hip_fb_swing = 2;
const int l_knee		 = 3;
const int l_ankle		 = 4;
const int r_hip_rotate	 = 5;
const int r_hip_swing	 = 6;
const int r_hip_fb_swing = 7;
const int r_knee		 = 8;
const int r_ankle		 = 9;
const int l_shoulder_rotate	 	= 10;
const int l_shoulder_swing	 	= 11;
const int l_shoulder_fore_rotate= 12;
const int l_wrist_rotate	 	= 13;
const int l_wrist_swing		 	= 14;
const int r_shoulder_rotate	 	= 15;
const int r_shoulder_swing	 	= 16;
const int r_shoulder_fore_rotate= 17;
const int r_wrist_rotate	 	= 18;
const int r_wrist_swing		 	= 19;
*/

struct stBodyPosition {     // all angles given in degrees!
    // Legs:
    float   l_hip_rotate;
    float   l_hip_swing;
    float   l_hip_fb_swing;
    float   l_knee;
    float   l_ankle;

    float   r_hip_rotate;
    float   r_hip_swing;
    float   r_hip_fb_swing;
    float   r_knee;
    float   r_ankle;

    // ARMS:
    float   l_shoulder_rotate;
    float   l_shoulder_swing;
    float   l_shoulder_fore_rotate;
    float   l_elbow;
    float   l_wrist_rotate;
    float   l_wrist_swing;

    float   r_shoulder_rotate;
    float   r_shoulder_swing;
    float   r_shoulder_fore_rotate;
    float   r_elbow;
    float   r_wrist_rotate;
    float   r_wrist_swing;
    
    float   head_turn;
    float   head_tilt;
};

/*struct stBodyPositionVector 
{
	int 	num_active_servos;
	float	servo_values[MAX_SERVOS];	// See "struct stBodyPosition" at bottom of this file for order definition:
};*/