#ifndef _SEQUENCER_MEMORY_HPP_
#define _SEQUENCER_MEMORY_HPP_

/************************************************************
	Data Flow - sequencer List from UDP received to avisual 
	
	abkInstant receives the UDP beacon and stores it ___[where]___
	Then puts it into this shared memory.
	avisual polls 
 ************************************************************/

#include <list>
#include "global.h"
#include "body_pose.h"

#define IPC_KEY_SEQ    1275
#define MAX_SEQUENCES  300

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

/*struct stBodyPositionVector 
{
	int 	num_active_servos;
	float	servo_values[MAX_SERVOS];	// See "struct stBodyPosition" at bottom of this file for order definition:
};*/

/******************** sequencer MEMORY MAP *****************/
struct sequencer_ipc_memory_map
{
	long int StatusCounter;
	char	 ConnectionStatus[64];
	long int StatusAcknowledgedCounter;

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
extern char* 	sequencer_shared_memory;
extern int 		sequencer_segment_id;
extern struct   sequencer_ipc_memory_map* ipc_memory_sequencer;
/*********************************************************/

/*********** SHARED MEMORY SEGMENT FUNCTIONS **************/
void seq_dump_ipc			( );
void seq_save_segment_id	(char* mFilename);
int  seq_read_segment_id	(char* mFilename);
int  seq_allocate_memory	( );
void seq_deallocate_memory	( int msegment_id );
int  seq_attach_memory		( );
void seq_detach_memory		( );
void seq_reattach_memory	( );
int  seq_get_segment_size   ( );
void seq_fill_memory		( );
/************************************************************/

//bool seq_new_command_available( );
//void seq_acknowledge_command( );

/*********** MEMORY DATA ACCESS FUNCTIONS **************/
// for instant to fill in the shared mem buffer:
void seq_ipc_write_connection_status( char* mStatus   );
void seq_ipc_write_performance_data ( struct stPerformanceData* mData  );
void seq_ipc_write_diagnostic_data  ( struct stDiagnosticData*  mData  );
void seq_ipc_write_enables	        ( BOOL* mEnableArray, int mSize    );
void seq_ipc_write_vector           ( struct stBodyPositionVector* mStatus );

void seq_print_sequence		    	( );

int   seq_connect_shared_sequencer_memory( char mAllocate=FALSE );
char* seq_get_connection_status		();

BOOL  seq_is_new_connection_status	();
BOOL  seq_is_new_performance_status	();
BOOL  seq_is_new_diagnostic_status	();
BOOL  seq_is_new_enable_status	    ();
BOOL  seq_is_new_vector_status	    ();

void seq_ack_connection_status	();
void seq_ack_performance_status	();
void seq_ack_diagnostic_status	();
void seq_ack_enable_status		();
void seq_ack_vector_status		();

bool ipc_add_sequence           ( struct stBodyPositionVector*  mVector );

/************************************************************/

#endif

/*
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
*/
