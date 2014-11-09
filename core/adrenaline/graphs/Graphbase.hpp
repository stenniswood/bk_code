#ifndef _GRAPHBASE_
#define _GRAPHBASE_

#include "dataset.hpp"
#include "control.hpp"

/* Abstract Base Class for all graphs */
class Graphbase : public Control
{
public:
	Graphbase( int Left, int Right, int Top, int Bottom );
	Graphbase();

	virtual void  Initialize	(					);
	void  set_text  	 		( char* Title, char* xLabel, char* yLabel );

	void  set_title_size 		( float NewSize 	)	{ title_size  = NewSize; };
	void  set_xLabel_size		( float NewSize 	)	{ xlabel_size = NewSize; };
	void  set_yLabel_size		( float NewSize 	)	{ ylabel_size = NewSize; };

	void  set_title 			( char* Title 		);
	void  set_xLabel			( char* Label 		);
	void  set_yLabel			( char* Label 		);
	void  set_position			( int Left, int Right, int Top, int Bottom );
	void  move_to	  			( int Left, int Right );

	// This links the Datasets together!
	void  addDataSeries	  		( DataSet* NewData );
	void  removeDataSeries		( DataSet* OldData );
		
	void  set_horizontal_lines	( int NumberLines ) { NumberHorizontalLines=NumberLines; };	
	void  show_vertical_lines 	( bool Show ) {ShowVerticalLines=Show; };
	
	virtual int   draw			(	);		// Draw calls all the sub draw routines.
	virtual int   draw_body		( 	)=0;		// derived class must override this!
	int   draw_title			( 	);
	int   draw_x_axis_label		( 	);
	int   draw_y_axis_label		( 	);
	void  draw_horizontal_lines (   );
	void  draw_vertical_lines   (   );
	//virtual int	  draw_body		(	)=0;	// Must implement in derived class! Contains the graph body.
 
protected:
//	Container* DataSeries;
	DataSet* DataHead;
	DataSet* DataTail;	

	float	title_size  ;
	float	xlabel_size ;
	float	ylabel_size ;

	char* 	xAxisLabel;
	char* 	yAxisLabel;
	char* 	title;
	bool	ShowVerticalLines;
	int 	NumberHorizontalLines;	
};

 
#endif
