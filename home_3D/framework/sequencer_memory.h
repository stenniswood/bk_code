//
//  sequencer_memory.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/17/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
/********************   ROBOT INTERFACE      *****************
 
 This holds memory structure of everything needed to supply
 a physical robot with to perform the simulated moves.
 
 ******************** 3D SIMULATOR MEMORY MAP *****************/

#ifndef __home3D__sequencer_memory__
#define __home3D__sequencer_memory__

#include <stdio.h>
#include "Sim3D_defines.h"
#include "robot.hpp"

#ifdef  __cplusplus
extern "C" {
#endif

#define IPC_KEY_SEQ 0x04D3          // 1235 in decimal!
    
extern char* 	sequencer_shared_memory;
extern int 		sequencer_segment_id;
extern struct   sequencer_ipc_memory_map* ipc_memory_seq;
    
#define MAX_CLIENT_ARRAY_SIZE 2048


#define STATUS_NO_ACTION 0
#define STATUS_START     1
#define STATUS_PAUSE     2
#define STATUS_STOP      3

struct one_vector
{
    int     number_of_actuators;
    float   angle_degrees[MAX_SERVOS];
};

/******************** SEQUENCER MEMORY MAP *****************/
struct sequencer_ipc_memory_map
{
    long int StatusCounter;
    char	 ConnectionStatus[64];

    long int AcknowledgeCounter;    // move object
    long int UpdateCounter;         // move object
    
    int      start_pause_stop_status;   //
    float    time_slice_period;         //

    int                number_of_vectors;
    int                last_read_vector;
    struct stBodyPosition  sequence[MAX_SEQUENCE_LENGTH];
    unsigned char      instance_mapping[MAX_SERVOS];

    long int ResponseCounter;       //
    char	 Response        [128]; //
};
/******************** AVISUAL MEMORY MAP *****************/
/*********************************************************/

bool seq_new_command_available( );
void seq_acknowledge_command  ( );
    
void seq_dump_ipc			();
void seq_save_segment_id	(char* mFilename);
int  seq_read_segment_id	(char* mFilename);

bool  is_seq_ipc_memory_available();
int   connect_shared_sequencer_memory( char mAllocate );
int   seq_allocate_memory	();
void  seq_deallocate_memory	(int msegment_id);
long  seq_attach_memory		();
void  seq_reattach_memory	();
void  seq_detach_memory		();

unsigned long seq_get_segment_size();
void seq_fill_memory              ();


bool ipc_add_sequence           ( struct stBodyPosition*  mVector);
bool ipc_write_sequence         ( int mIndex, struct stBodyPosition* mBP );
//bool ipc_write_sequence         ( int mIndex, struct one_vector  mVector );
bool ipc_write_instance_index   ( int mIndex, unsigned char mInstance    );

    
void seq_ipc_write_connection_status( char* mStatus   );
void seq_ipc_write_response         ( char* mSentence );
    
    
#ifdef  __cplusplus
}
#endif



#endif /* defined(__home3D__sequencer_memory__) */
