#ifndef _MOTOR_VECTOR_H_
#define _MOTOR_VECTOR_H_


/* A Vector is just a set of Instance numbers with a name. 
	ie "vec1"  [3,5,8,10] 
	ie "vec1"  [1,3,9,2 ]
*/
struct stVector {
	char* Name;
	byte* Instances;
	byte  Dimension;
	struct stVector* Next;
	struct stVector* Prev;	
};

struct stVector* NewVector( char* mName, byte mSize );
void AssignBoards		( struct stVector* mVector, byte* mInstances );
void send_angle_vector	( byte mSize, byte* mInstances, float* mAngles, float* mSpeeds );


#endif
