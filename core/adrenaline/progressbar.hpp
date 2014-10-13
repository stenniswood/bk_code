#ifndef _ProgressBar_
#define _ProgressBar_

#include "control.hpp"


/* Warning the show percentage option will show below the 'bottom'
   rectangular outline!  May correct this later so that all controls
   are gauranteed to be within the client rectangle. 
*/
class ProgressBar : public Control 
{
public:
	ProgressBar		(										   );
	ProgressBar		( int Left, int Right, int Top, int Bottom );
	//ProgressBar( int Width, int Height);  Not good because no markers for length.
	
	void			Initialize			( );	
	void			set_percentage		( float Percentage);
	void			set_max_value		( float Max		  );
	void			set_current_value	( float current   );
	void			show_percentage 	( bool mShow=true )	{ show_percent = mShow; };

	// override these because show_percentage text hangs below bottom!
	//void  			set_position_above  ( Control* Sibling );
	// This is harder b/c we need to know if the Sibling is ProgressBar or not.	
	//	void  			set_position_below	  ( Control* Sibling );	
	
	void			print_info			( );
	virtual int   	draw 				( );

private:
	float 		fraction;			// 1.0 is 100%
	bool  		show_percent;
	long int	fill_color;
	
	// Alternate to percent
	float 		Max;
	float 		Current;
};

 
#endif
