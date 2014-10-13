#ifndef _DATA_SET_H_
#define _DATA_SET_H_


class DataSet
{
public:
	DataSet( int NumberElements );	// allocates 
	DataSet( float* data, int n );

	float operator [](int index) {return Data[index]; };
	
	int	 get_count( )  { return last_filled_index; };	
	int	 get_size ( )  { return n; };	
	void set_data ( float* data, int n );	// sets pointer (does not free existing memory)
	void add	  ( float new_member );
	void shift_add( float new_member );		// removes first element adds new to end.

	void  compute_stats	(  );	// Does the crunching.
	float calc_average	(  );	// then results can be repeatedly retrieve at no cost.
	float calc_stddev 	(  );		
	float calc_max		(  );
	float calc_min		(  );
	int   count_samples_between( float min, float max );

	// Accessor Functions:
	float get_average(  )	{ return average; };	// then results can be repeatedly retrieve at no cost.
	float get_stddev (  )	{ return stddev;  };		
	float get_max(  )		{ return max;     };
	float get_min(  )		{ return min;     };
	void  shift	 (  );

private:
	float*  Data;
	int		n;	
	int 	last_filled_index;
	
	// Stats:
	float average;
	float max;
	float min;
	float stddev;
	

public:	
	// Datasets may be linked:
	void		setNext(DataSet* next)	{ Next = next; };
	void		setPrev(DataSet* prev)	{ Prev = prev; };
	DataSet*	getNext()				{ return Next; };
	DataSet*	getPrev()				{ return Prev; };
private:	
	// Multiple DataSeries can be linked together.
	DataSet* Next;
	DataSet* Prev;	
	
};


#endif
