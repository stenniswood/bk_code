
#include <vector>
#include "vector.hpp"
#include "Sim3D_defines.h"


#define IPC_KEY_SIM           0x04D2        // 1234 in decimal!
#define MAX_CLIENT_ARRAY_SIZE 2048

extern char* 	simulator_shared_memory;
extern int 		simulator_segment_id;
extern struct   simulator_ipc_memory_map*  ipc_memory_sim;

extern class IPC_Exception IPC_Error;

class IPC_Exception : public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "Simulator Memory is not available.";
    }
};

struct Coordinate
{
    float x;
    float y;
    float z;
};
    
#define COMMAND_MOVE_OBJECT          0x101       // Move an item in the 3D space
#define COMMAND_WHERE_IS             0x102       // name room/location where an object is in 3D space
#define COMMAND_WHAT_IS_IN           0x106       // list objects contained in a rectangle
#define COMMAND_NEW_OBJECT           0x103       // Create a new item in 3D space
#define COMMAND_ROBOT_STOP           0x104       // Pick up specified object
#define COMMAND_ROBOT_MOVE_OBJECT    0x105       // Pick up specified object
#define COMMAND_ROBOT_MOVE           0x107       // Relocate (walk) to an XYZ
#define COMMAND_ROBOT_ANGLE          0x109       // Move the robot to specified angles
#define COMMAND_ROBOT_ANGLES         0x10B       // Move the robot to specified angles
#define COMMAND_ROBOT_PREDEFINED_SEQ 0x10D       // Move robot thru a predefined sequence.
#define COMMAND_ROBOT_SEQ            0x10F       //
#define COMMAND_DELETE_OBJECT        0x110       // Create a new item in 3D space
#define COMMAND_SET_VELOCITY         0x112       // Create a new item in 3D space
#define COMMAND_CHANGE_GLOBAL_VAR    0x114       // Change vars such as show Camera, audio, RobotView/WorldView, etc.

#define COMMAND_CHANGE_VISIBILITY    0x116       // ie make walls invisible. (half transparency, etc)


/*const int QUERY_GET_OBJECT_ID       = 0x201;
const int QUERY_GET_OBJECT_COORD    = 0x202;
const int QUERY_COORDINATE          = 0x203; */

#define QUERY_GET_OBJECT_ID                 0x201       // string object name, returns MathVector position.
#define QUERY_GET_OBJECT_POSITION           0x202       // string object name, returns MathVector position.
#define QUERY_OBJECT_POSITION_RELATIVE      0x203       // object name, returns verbal description compared to another object.
#define QUERY_OBJECT_NAME                   0x204       // object id, return string object name.

#define ACTION_OBJECT_OPEN                  0x301       // string object name, returns MathVector position.
#define ACTION_OBJECT_LIFT	                0x302       // string object name, returns MathVector position.
#define ACTION_OBJECT_PUSH	                0x303       // string object name, returns MathVector position.
#define ACTION_OBJECT_GRAB	                0x304       // string object name, returns MathVector position.
#define ACTION_OBJECT_RELEASE               0x305       // string object name, returns MathVector position.


/* SEE robot.hpp for an enum of the ServoIndices.  ie which index goes to which motor.  The person that needs this is
 the one sending the commands, ie. the NLP of abkInstant which posts the commands to this memory.  */
enum ServoIndices {
    
    LEFT_HIP_ROTATE,
    LEFT_HIP_SWING,
    LEFT_HIP_FB_SWING,
    LEFT_KNEE,
    LEFT_ANKLE_SWING,
    
    RIGHT_HIP_ROTATE,
    RIGHT_HIP_SWING,
    RIGHT_HIP_FB_SWING,
    RIGHT_KNEE,
    RIGHT_ANKLE_SWING,
    
    LEFT_SHOULDER_ROTATE,
    LEFT_SHOULDER_SWING,
    LEFT_SHOULDER_FORE_ROTATE,
    LEFT_ELBOW,
    LEFT_WRIST_ROTATE,
    LEFT_WRIST_SWING,
    
    RIGHT_SHOULDER_ROTATE,
    RIGHT_SHOULDER_SWING,
    RIGHT_SHOULDER_FORE_ROTATE,
    RIGHT_ELBOW,
    RIGHT_WRIST_ROTATE,
    RIGHT_WRIST_SWING,
    
    HEAD_TURN,
    HEAD_TILT
};

enum PredefinedPoses {
    
    STAND,
    SIT,
    FLOOR_SIT,
    ONE_KNEE_KNEEL,
    KNEEL,
    SQUAT,
    
    HANDS_ON_HIP,
    FOLDED_ARMS,
    ZOMBIE_ARMS,
    ARMS_DOWN_BY_SIDE,
    
    ARM_STRAIGHT_UP,
    ARM_STRETCHED_SIDE_LEFT,
    ARM_STRETCHED_SIDE_RIGHT,
    ATTENTION_1,
    ATTENTION_2
};



/* Used for creating a new object. */
enum eObjectTypes {
    VERBAL_OBJECT,  // Verbal object?!@
    ATOM,           // ATOM?! SPHERE?
    BOX,
    DOOR,
    IBEAM,
    CYLINDER,
    EXTRUSION,
    PAPER,
    STAIRWAY,
    TEXTURE_CONTAINER,
    ROUTE,                  // 10
    FULL_WALL,
    CHAIR,
    TABLE_4LEGS,
    TABLE_CENTER_POLE,
    DRAWER,
    PALLETTE,
    BARRICADE,
    BOOKCASE,
    BRICK,
    SPHERE,                 // 20
    PICNIC_TABLE,
    CABINET,
    COUNTER,
    TRUSS2D,
    TRUSS3D,
    TRUSSHIGHWAY,
    RAFTER,
    LIGHTSWITCH,
    HINGE,
    ROAD,                   // 30
    TERRAIN,
    PAINTING,
    BASKETBALL,             // 33
    SOCCERBALL_5,           // 34
    SOCCERBALL_4,           // 35
    BASEBALL,               // 36
    ROBOT_LEG,
    ROBOT_ARM,
    ROBOT,
    STREET_SIGN,
    BASKETBALL_ARENA,
    SKETCHUP_IMPORT,
    WINDOW
};

/* this doesn't belong here, but for now it works quite well. */
union uGlobalVars
{
    struct GlobalVars {
        unsigned char CameraOn;
        unsigned char ShowRobotViewPoint;
        
    } gv;
    unsigned char raw_array[sizeof(struct GlobalVars)];
};


#define MAX_IDS 50
/******************** CLIENT CONNECTION       *****************
  This holds memory structure for the client connections (user 
  interface and control) to and from this service.
 
 This allows NLP commands to create, place, query and view
 new scenarios. 
 
 ******************** 3D SIMULATOR CLIENT MEMORY MAP *****************/
struct simulator_ipc_memory_map
{
	long int StatusCounter;
	char	 ConnectionStatus[64];

    long int AcknowledgeCounter;    //
    long int CommandCounter;        //
    
    /*  */
    char	 Sentence[255];								// verbal commands (NLP) activate connection requests.
    long int UpdateCounter;								// Incremented on change to any of below:
    long int AcknowledgedCounter;
    
    char	 Response[255];								// verbal commands (NLP) activate connection requests.
    long int ResponseCounter;							// Incremented on change to any of below:
    long int ResponseAcknowledgedCounter;				// Incremented on change to any of below:
        
    long     Command;               // move object
    long     object_id;             //
    long     robot_id;              //
    long     object_type;           //
    long     object_datum1;         // depends on the command
    long     object_datum2;         //
    long     object_datum3;         //
    char	 object_name[255];		// for finding an object by name.
    
    struct Coordinate Location1;
    struct Coordinate Location2;
    float    servo_angles[MAX_SERVOS];  // Degrees.

    int      num_valid_ids;
    long     new_object_ids[MAX_IDS];
    
};

/******************** CLIENT MEMORY MAP *****************/
/*********************************************************/

void  sim_delete_all_shm();

bool  sim_new_command_available   ( );
void  sim_acknowledge_command     ( );
int   sim_wait_for_acknowledgement( );

bool  sim_new_response_available ( );
void  sim_acknowledge_response   ( );
int   sim_wait_for_response      ( );

void  sim_dump_ipc               ( );
void  sim_save_segment_id        (char* mFilename);
int   sim_read_segment_id        (char* mFilename);

bool  is_sim_ipc_memory_available( );
int   connect_shared_simulator_memory( char mAllocate );
int   sim_allocate_memory        ( );
void  sim_deallocate_memory      ( int msegment_id);
long  sim_attach_memory          ( );
void  sim_reattach_memory        ( );
void  sim_detach_memory          ( );

unsigned long sim_get_segment_size( );
void sim_fill_memory            ( );
    
void sim_ipc_write_connection_status( char* mStatus   );
void sim_ipc_write_response         ( char* mSentence );
void sim_ipc_write_sentence         ( const char* mSentence );

// WRITE COMMANDS TO IPC MEM:
void sim_what_is_in             ( long mRoom_id                      );
void sim_where_is               ( long mObject_id                    );
void sim_where_is               ( MathVector mNewLocation            );

void sim_new_object             ( long mObject_type, int mQuantity, long mDatum2=0, long mDatum3=0 );
void sim_delete_object          ( long mObject_id                                                   );
void sim_move_object            ( long mObject_id, MathVector mNewLocation, MathVector mNewOrientation );
void sim_robot_move_object      ( long mRobot_id,  long       mObject_id,   MathVector mNewLocation, MathVector mNewOrientation );
void sim_stop_robot             ( long mRobot_id   );
void sim_move_robot             ( long mRobot_id,  MathVector mDestination, MathVector mSource, bool mUseSource=false );
void sim_robot_angle            ( long mRobot_id,  int mServo_index, float mNewAngles );
void sim_robot_angles           ( long mRobot_id,  std::vector<int> mservo_indices, MathVector mNewAngles );
void sim_robot_predefined       ( long mRobot_id,  int mMoveSequenceIndex, int repetitions           );
void sim_write_object_ids       ( std::vector<long> mIDs );
void sim_change_visibility      ( long mObject_id,   int& mParameter1, int& mParameter2 );
void sim_set_object_velocity    ( long mObject_id,   MathVector& mNewVelocity );

// READ COMMAND FROM IPC MEM:
void sim_read_command           ( long& mObject_id,   MathVector& mNewLocation, MathVector& mNewOrientation );
void sim_read_move_object       ( long& mObject_id,   MathVector& mNewLocation, MathVector& mNewOrientation );
void sim_read_new_object        ( long& mObject_type, int&         mQuantity                                );
void sim_read_delete_object     ( long& mObject_id );
void sim_read_robot_move_object ( long& mRobot_id,    long&        mObject_id,  MathVector& mNewLocation    );
bool sim_read_move_robot        ( long& mRobot_id,    MathVector&  mSource,     MathVector&  mDestination   );
void sim_read_robot_angle       ( long& mRobot_id,  int& mServo_index, float& mNewAngle );
void sim_read_robot_angles      ( long& mRobot_id,    MathVector& mNewAngles );
void sim_read_robot_predefined  ( long& mRobot_id,    int&         mMoveSequenceIndex,  int& repetitions    );
void sim_read_object_ids        ( std::vector<long>& mIDs );
void sim_read_visibility        ( long& mObject_id,   int& mParameter1, int& mParameter2 );
void sim_read_object_velocity   ( long& mObject_id,   MathVector&   mNewVelocity );

    
