#ifndef _VECTOR_FILE_H_
#define _VECTOR_FILE_H_

#include "Container.hpp"
//#include "board_list_oop.hpp" 
//#include "robot.hpp"

class Robot;


// Datatype definitions
#define UNDEFINED_TYPE  0x00
#define FLOAT 			0x01
#define INT   			0x02

#define POSITION_VECTORS 0x10
#define SPEED_VECTORS 	 0x20
#define OTHER_VECTORS	 0x90



class OneVector
{
public:
	OneVector(int Dimension);
	OneVector( OneVector* original );		// copy ctor	
	~OneVector();

	void		set_element	( float mValue, int mIndex	);
	float		get_element	( int   mIndex 				);
	float		operator[]	( int   mIndex 				);
	void		operator-=	( OneVector* mSub			);
	void		operator+=	( OneVector* mAddend		);	
	
	float		x			(  							);
	float		y			(  							);
	float		z			(  							);
	float		get_magnitude(  						);
	
	float*		get_data   ( )  					{ return fData; };
	float* 		atof_array ( char** mStrArray );
	void 		print	   (  ) 					{  printf("OneVector: %6.2f, %6.2f, %6.2f\n", fData[0], fData[1], fData[2] ); };
	int			get_dimension()	{ return dimension; };
	void		to_radians ( );
	void		to_degrees ( );
	
private:
	float* 		fData;
	int	   		dimension;
};

/* The objects:
OneVector			- holds an array of floats
VectorSequence		- holds a sequence of array of floats. 
					  ie enough for 1 limb.
VectorGroupSequence - holds a group of VectorSequence's.  1 for each limb. 
					  ie. enough for entire robot.
*/
#define MODE_ONCE_THRU 0
#define MODE_LOOP 	   1
#define MODE_INVERSE   2					// not implemented

class VectorSequence : public Container		// was  struct sVectorList
{
public:
	VectorSequence(int mDimension);
	~VectorSequence();
	
	OneVector* new_vector	 ( );
	Item* 	   add_vector	 ( OneVector* mVector );
	OneVector* new_add_vector( );

	void	   set_mode	 	 ( int mMode  );
	BOOL	   move_to	 	 ( int mIndex );
	BOOL	   start_over 	 ( 			  )  { return move_to(0);		};
	BOOL	   next_vector 	 ( 			  )  { CurrentIndex++;	return false; };

	OneVector* get_vector	 ( 			  );
	OneVector* get_ivector   ( int mIndex );
	OneVector* get_ivelocity_vector( int mIndex=-1 );
	OneVector* get_iaccel_vector   ( int mIndex=-1 );

	int		   get_dimension ( 			  ) { return Dimension; 	 };
	byte	   data_type;

private:
	// use getNumItems() in Container base class
	byte 	Mode;
	int 	CurrentIndex;
	int		Dimension;
};

class VectorGroupSequence : public Container
{
public:
	// uses Robot to know num_limbs and dimension of each
	VectorGroupSequence 	( );
	~VectorGroupSequence	( );

	// LIST FUNCTIONS:
	void				set_mode				( int mMode 			);
	void 				set_data_type_all_lists	( byte mType, word mSpeed );
	void 				copy_robot_structure	( Robot *mRobot			);
	void 				add_new_vector_sequence	( int dimension 		);
	VectorSequence*		get_limb_sequence		( byte mIndex 			);

	// FILE ACCESS:	
	void 				read_vector_file		( char* mFilename		);
	void				next_vector				( 				 		);
	OneVector*			get_vector_rad			( int mLimbIndex 		);
	OneVector*			get_vector_deg			( int mLimbIndex 		);
	OneVector*			get_velocity_deg		( int mLimbIndex, int mIndex );
	OneVector*			get_acceleration_deg	( int mLimbIndex, int mIndex );
	int					get_number_vectors		( ) { return num_vectors; };

protected:
	void 				read_header				(                 		);
	OneVector*  		read_line				( byte mDataType  		);
	void		  		read_one_vector			( byte mDataType  		);

private:
	FILE*	fd;
	char*	mFileName;
	int		num_vectors;
	int		current_index;
	BOOL	data_is_radians;
	
	int 	playback_period_ms;
};

class ListOfMoves : public Container
{
public:
	ListOfMoves();
	~ListOfMoves();

	VectorGroupSequence*	add_move		( int mIndex );	
	VectorGroupSequence*	get_move		( int mIndex );
	VectorGroupSequence*	get_number_of_moves( );	

private:
	
};



#endif

