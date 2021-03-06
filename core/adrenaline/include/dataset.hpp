#ifndef _DATA_SET_H_
#define _DATA_SET_H_

#include <vector>
#include <string>

/* Need to redo this class and use a vector<float> template! */

class DataSet
{
public:
	DataSet( 					);	
	DataSet( float* data, int n );
	~DataSet( 					);

	float operator [] 			( int index) { return Data[index]; };
	float 		get_datum 		( int index 			 )  { return Data[index]; };
	void  		set_datum 		( int index, float datum )	{ Data[index]=datum; };		
 	void		reset			( );
 	
	int	 		get_size 		( )  { return Data.size(); };	
	void 		set_data 		( float* data, int n );	// sets pointer (does not free existing memory)
	void 		add	 	 		( float new_member, bool mComputeStats=true );
	void 		shift_add		( float new_member   );		// removes first element adds new to end.

	// Statistics :
	int   count_samples_between	( float min, float max );
	void  compute_stats			(  );	// Does the crunching.
	float calc_average			(  );	// then results can be repeatedly retrieve at no cost.
	float calc_stddev 			(  );		
	float calc_max				(  );
	float calc_min				(  );

	float get_average			(  )	{ return average; };	// then results can be repeatedly retrieve at no cost.
	float get_stddev 			(  )	{ return stddev;  };		
	float get_max				(  )	{ return max;     };
	float get_min				(  )	{ return min;     };
	void  shift	 				(  );

	std::string				name;		// series name.
	long int 				color;

private:
	std::vector<float>	Data;

	// Stats:
	float average;
	float max;
	float min;
	float stddev;

};


#endif
