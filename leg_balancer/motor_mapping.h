

/* 
	This maps every servo motor to a DriveFive board and channel (letter).
	So that commands can be given such as "bend wrist" and it would know 
	which motor to operate.
	
	The caller specifies a [limb] and [joint] to get a letter.	
	
*/
struct stMap {
	char limb[20];
	char joint[20];
	char board_name[20];		// result of df.get_name()
	char device_name[40];		// /dev/tty.usbmodem-xxxxxxx
	char assigned_motor;			// v, w, x, y, z	
	DriveFive* df;
};


extern struct stMap body_joints[];

#define TOTAL_BODY_JOINTS 22


void  init_map			( );
char* alias_lookup		( char* mAlias );
char* to_alias			( char* mLimb  );

char* get_device_name   ( char* limb, char* joint );
char  get_assigned_motor( char* limb, char* joint );


void fill_in_device_name( const char* mBoardName, const char* mDevName, DriveFive* mDF );
void print_map			( );

