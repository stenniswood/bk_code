#ifndef _GRAPHBASE_
#define _GRAPHBASE_

#include <vector>
#include "bk_system_defs.h"
#include "dataset.hpp"
#include "control.hpp"

// Definitions for draw_options bitfield variable.
#define SHOW_TITLE		0x01
#define SHOW_X_LABEL	0x02
#define SHOW_Y_LABEL	0x04
#define SHOW_X_SCALE	0x08		// number on x axis
#define SHOW_Y_SCALE	0x10		// number on x axis



/* Abstract Base Class for all graphs */
class Graphbase : public Control
{
public:
	Graphbase( int Left, int Right, int Top, int Bottom );
	Graphbase();

	virtual void  Initialize	(					);
	void  set_text  	 		( char* Title, char* xLabel, char* yLabel );

	void  show_title()			{ draw_options &= SHOW_TITLE; 	};
	void  show_x_labels()		{ draw_options &= SHOW_X_LABEL; };
	void  show_y_labels()		{ draw_options &= SHOW_Y_LABEL; };
			
	void  set_title_size 		( float NewSize 	)	{ title_size  = NewSize; };
	void  set_xLabel_size		( float NewSize 	)	{ xlabel_size = NewSize; };
	void  set_yLabel_size		( float NewSize 	)	{ ylabel_size = NewSize; };

	void  set_title 			( char* Title 		);
	void  set_xLabel			( char* Label 		);
	void  set_yLabel			( char* Label 		);
	void  set_position			( int Left, int Right, int Top, int Bottom );
	void  calc_body_coords		(					  );
	void  move_to	  			( int Left, int Right );

	// This links the Datasets together!
	void  add_data_series	  	( DataSet* NewData );
	void  remove_data_series	( int index /*DataSet* OldData*/ );

	void  set_horizontal_lines	( int NumberLines ) { NumberHorizontalLines=NumberLines; };	
	void  set_vertical_lines 	( int NumberLines ) { NumberVerticalLines=NumberLines; };
	
	virtual int   draw			(	);		// Draw calls all the sub draw routines.
	virtual int   draw_body		( 	)=0;		// derived class must override this!

	int   	draw_title			( 	);
	int   	draw_x_axis_label	( 	);
	int   	draw_y_axis_label	( 	);
	int 	draw_x_scale		(	);
	int 	draw_y_scale		(	);
	
	void  	draw_horizontal_lines (		);
	void  	draw_vertical_lines   (		);
	
	//virtual int	  draw_body	  (	    )=0;	// Must implement in derived class! Contains the graph body.
	
	void 	find_min();		// finds min of all data series.
	void 	find_max();		// finds min of all data series.
	int		count_data_series();	// how many data series are there?

protected:
	std::vector<DataSet>	data_series;
	

	byte		draw_options;		// bit field 
	long int	title_color;
	long int	xlabel_color;
	long int	ylabel_color;
	long int	grid_color;			// for both vertical & horizontal lines.
		
	float	title_size  ;
	float	xlabel_size ;
	float	ylabel_size ;

	float 	max,min;
	
	// the guts of the graph goes here (title,labels,etc are outside this):
	float	body_left,  body_bottom;
	float	body_width, body_height;
	
	char* 	xAxisLabel;
	char* 	yAxisLabel;
	char* 	title;
	bool	ShowVerticalLines;
	int 	NumberHorizontalLines;
	int 	NumberVerticalLines;
};

#endif
