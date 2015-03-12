#ifndef _VECTOR_FILE_H_
#define _VECTOR_FILE_H_

#ifdef  __cplusplus
extern "C" {
#endif

// Datatype definitions
#define UNDEFINED_TYPE  0x00
#define FLOAT 			0x01
#define INT   			0x02

#define POSITION_VECTORS 0x10
#define SPEED_VECTORS 	 0x20
#define OTHER_VECTORS	 0x90

struct stPotVector
{
	vector<word> PotValues;	
};
struct stAngleVector
{
	vector<float> AnglesValues;
};

struct sOneVector
{
	// The size of the data will come from the static Vector object 
	// defined in vectors.h 
	union {
		float* fData; // this could be int* or float* depending on DataType in VectorList
		int*   iData; 
	} values;
	byte DataType;		// int,float, Angle, PotValue
	
	struct sOneVector* Prev;
	struct sOneVector* Next;
};

struct sRawVectorList
{
	// The size of the data will come from the static Vector object 
	// defined in vectors.h 
	vector<stPotVector>  vectors;
	int 	playback_period_ms;
};

struct sVectorList
{
	// The size of the data will come from the static Vector object 
	// defined in vectors.h 
	struct sOneVector* HeadPtr;
	struct sOneVector* TailPtr;
	int		NumVectors;
	int		Dimension;
	int 	playback_period_ms;
	byte	data_type;
};

// LIST FUNCTIONS:
extern struct sVectorList** VectorLists;

void   				init_lists			(										);
void   				init_list			( byte ListIndex, int Dimensionality	);
struct sOneVector* 	list_append_vector	( struct sVectorList* mlist, 
									   	  struct sOneVector*  mv 				);
									   	  
void 				list_remove_all			     ( struct sVectorList* mlist 	);
void 				set_playback_period_all_lists( int mPeriod					);
void 				set_data_type_all_lists		 ( byte mType, word mSpeed		);

// DATA VECTOR FUNCTIONS:
struct sOneVector*  new_vector      ( byte Dimension, byte DataType );
//float* 				atof_array		( byte Dimension,char** mArray	);

// FILE ACCESS:
struct sOneVector*  read_line		( FILE* f, byte mDataType );
void 				read_vector_file( char* mFilename );

#ifdef  __cplusplus
}
#endif

#endif
