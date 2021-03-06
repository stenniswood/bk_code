#ifndef _VISION_MEMORY_
#define _VISION_MEMORY_

#include <vector>
#include "vector.hpp"

#define IPC_KEY_EYES			0x04D8        // 1236 in decimal!
#define MAX_CLIENT_ARRAY_SIZE	2048
#define MAX_STREAM_SIZE			5
extern char* 	eyes_shared_memory;
extern int		eyes_segment_id;
extern struct   eyes_ipc_memory_map*  ipc_memory_eyes;

#ifndef STCOORDINATE
#define STCOORDINATE 1
struct Coordinate
{
    float x;
    float y;
    float z;
};
#endif

#define MAX_IDS 50
/*******************  VISION SYSTEM CONNECTION  ************************
  This holds memory structure for the machine vision connections.  It
  is a bidirectional communication.  
  
  The client software may request something from the vision system,
  	such as "what me." or "follow my finger", or "wait till you see the
  	door open".  These are passed in as verbal commands.
 
  The vision software may also initiate.  For example, "basketball on 
  collision course with me!", or "the door is opened", or "I cannot see you
  anymore."
  
 This allows NLP commands to create, place, query and view
 new scenarios.
 ******************** 3D SIMULATOR CLIENT MEMORY MAP *****************/
struct eyes_ipc_memory_map
{
	long int StatusCounter;
	char	 ConnectionStatus[64];		// Contains the latest connection status.  
	//	"Connected", "Server terminated", etc.
	
	char 	 MachineVisionInstance[80];	// Multiple sets of eyes may be available.
	// For example - one RPI dedicated to face detection and recognition.
	//  Perhaps another for wall and obstacle avoidance.
	//  Perhaps another for object recognition (scissors, remote control, plate, etc)

	// Client Command : 
    long int CommandCounter;    
    char	 client_command[255];		// for finding an object by name.
    long int AcknowledgeCounter;

	// Machine Vision Initiated/Response to Client Command:        
	long int ServerCounter;							// Incremented on change to any of below:
	char	 ServerEvent[255];
    long int ServerAcknowledgedCounter;				// Incremented on change to any of below:

	struct Coordinate	location_stream1[MAX_STREAM_SIZE];		// Use this for streaming face detect or other locations; (for instance left eye)
	struct Coordinate	location_stream2[MAX_STREAM_SIZE];		// Use this for streaming face detect or other locations; (for instance right eye)
	struct Coordinate	location_stream3[MAX_STREAM_SIZE];		// Use this for streaming face detect or other locations; (for instance face center)	
	int	stream1_index;
	int	stream2_index;
	int	stream3_index;
	
    // Some miscellaneous parameters may also be passed, but these must
    // be converted to ascii and passed in the text.    
};

/******************** CLIENT MEMORY MAP *****************/
/*********************************************************/

bool  is_eyes_ipc_memory_available	  ( );
void  eyes_ipc_write_connection_status( char* mStatus  );
int   eyes_connect_shared_memory	  ( char mAllocate );

bool  eyes_new_command_available    ( );
void  eyes_write_client_command   	( std::string mCommand );
void  eyes_acknowledge_command      ( );
void  eyes_wait_for_acknowledgement ( );

bool  eyes_new_server_event_available( );		// called by client to query the server status.
void  eyes_write_server_event   	( std::string mCommand );
void  eyes_acknowledge_server    	( );		// called by client
void  eyes_wait_for_server       	( );		// called by server

// ALLOCATION & ATTACHMENT : 
int   eyes_allocate_memory         ( );
void  eyes_deallocate_memory       ( );
long  eyes_attach_memory           ( );
void  eyes_reattach_memory         ( );
void  eyes_detach_memory           ( );

void  eyes_dump_ipc                ( );
unsigned long eyes_get_segment_size( );
void  eyes_fill_memory             ( );
void  eyes_save_segment_id         ( char* mFilename );
int   eyes_read_segment_id         ( char* mFilename );

void  delete_all_shm				 ( );

// UTILITY FUNCTIONS (to assist composing the server event string)
bool  eyes_is_valid_pointer		  ( );
char* eyes_compose_coordinate     ( MathVector& mNewVelocity );
void  eyes_compose_coordinate_xy  ( int mx, int my );

    
#endif
