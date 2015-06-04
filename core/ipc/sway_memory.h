#ifdef  __cplusplus
extern "C" {
#endif

#define IPC_KEY_SWAY 1236

extern char* 	segway_shared_memory;
extern int 		segway_segment_id;
extern struct   asegway_ipc_memory_map* ipc_memory_sway;

#define MAX_ARRAY_SIZE 2048

// other people laughing.  this kid is a nut case!
// no that's inverted.  Everyone else repugnant, the kid is laughing.

// Defines for MovementCommand:
// Kind of like: PRNDL
#define MOVE_STAND		 0x00
#define MOVE_FORWARD	 0x01
#define MOVE_BACKWARD	 0x02
#define MOVE_SPIN		 0x03
#define MOVE_CURVE_LEFT	 0x04
#define MOVE_CURVE_RIGHT 0x05

// This arm will not be a lot of Deg of Freedom.  Limited to a few.
#define ARM_STILL		 0x00
#define ARM_MOVE_UP	 	 0x01
#define ARM_MOVE_DOWN	 0x02
#define ARM_SPIN		 0x03
#define ARM_ELBOW		 0x04

/******************** AVISUAL MEMORY MAP *****************/
struct asegway_ipc_memory_map
{
	long int StatusCounter;
	char	 ConnectionStatus[64];
	int		 BaseMovementCommand;	// stand,forward,backward,spin, curve_left, cure_right, etc.
	float	 speed;
	float	 turn_angle;			// for curve left/right only.

	int		 ArmMovementCommand;	// When so equipped
	float	 ArmSpeed;
	float	 ArmAngle;			// position

	char	 SpareArray[MAX_ARRAY_SIZE];		// String array (dimension of NumberClients)
};
/******************** AVISUAL MEMORY MAP *****************/
/*********************************************************/

void dump_ipc				();
void seg_save_segment_id	(char* mFilename);
int  read_segment_id		(char* mFilename);

int  sway_allocate_memory	(); 
void sway_deallocate_memory	();

int  sway_attach_memory		();
void sway_reattach_memory	();
void sway_detach_memory		();

int  sway_get_segment_size	();
void sway_fill_memory		();

void ipc_write_connection_status( char* mStatus   );
void ipc_write_command_text 	( char* mSentence );

int	 connect_shared_abkInstant_sway_memory( );


#ifdef  __cplusplus
}
#endif
