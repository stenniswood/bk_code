#ifndef _VECTOR_FILE_H_
#define _VECTOR_FILE_H_

#include <vector>
using namespace std;
class Appendage;

#ifdef  __cplusplus
extern "C" {
#endif

// Datatype definitions
#define UNDEFINED_TYPE  0x00
#define FLOAT 			0x80
#define INT   			0x00

//#define POSITION_VECTORS		0x10 
#define POSITION_VECTORS_ANGLE	0x01 
#define POSITION_VECTORS_COUNT	0x02 
#define SPEED_VECTORS 	 		0x03 
#define OTHER_VECTORS	 		0x08

class sOneVector	// CORRESPONDS TO 1 LIMB!
{
public:
	sOneVector();

	void	print_counts( );
	void	print_vector( );
	float	get_angle	( byte index );
	int 	get_count	( byte index );
	
	vector<float> Data;	
	byte 		data_type;		// int,float, Angle, PotValue	

	Appendage*	limb;			// use to convert angles, etc.		
};

class sVectorSet				// A sequence of vectors
{
public:
	sVectorSet();
	vector<class sOneVector> vectors;
	int 		playback_period_ms;
	void		set_data_type			( byte mDataType 	);		// updates all vectors too.
	void		set_limb     			( Appendage* mlimb	);
	float		calc_average_speed_cps	( int mStartIndex, int mActuator );	// speed between 2 consequetive vectors

	byte 		data_type;
	
	void print_vectors( );
};

class Robot;

class sRobotVector		// All vectors for entire robot.
{
public:
	sRobotVector();
	vector<class sVectorSet> limbs;
	byte 		data_type;

	int 		Current_Vindex;
	int 		iterations_requested;	// -1 means infinite.
	int 		iterations_completed;	

	void		print_vector( int mIndex=-1, bool mAngles=true );
	void		init_limbs( );
	
	// FILE ACCESS : 
	void 				read_header			( FILE* f );
	int				    read_line			( FILE* f, byte mDataType);
	void 				read_vector_file	( char* mFilename );

	// FUNCTIONS : 
	float 				get_destination		( int mAppendageIndex, int mActuator 	);
	void 				list_append_vector	( int mListIndex, struct sOneVector& mv );

	void 				list_remove_all			     ( struct sVectorList* mlist 	);
	void 				set_playback_period_all_lists( int mPeriod					);
	void 				set_data_type_all_lists		 ( byte mType );
	void 				set_limbs					 ( Robot& mrobot );
};




#ifdef  __cplusplus
}
#endif

#endif
