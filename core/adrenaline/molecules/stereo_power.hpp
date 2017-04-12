#ifndef _STEREO_POWER_LEVELS_H_
#define _STEREO_POWER_LEVELS_H_

#include "control.hpp"
#include "power_level.hpp"
#include "bk_system_defs.h"

#define DefaultPadding 15
class Window;

/* This class shows an audio power indicator (1 channel).
   green, yellow, red color gradients.  with rectangular boxes. */

class StereoPowerLevels : public Control
{
public:
	StereoPowerLevels ( int Left, int Right, int Top, int Bottom );
	StereoPowerLevels ( int Width, int Height 					 );
	StereoPowerLevels ( );
	~StereoPowerLevels( );

	void	Initialize	   (			  );
	void	set_level_left ( float mLevel )	;
	void	set_level_right( float mLevel )	;
	void	set_number_boxes( int mBoxes )	;
	
	virtual void set_width_height ( int Width,  int Height 		);
	virtual void move_to		  ( float Left, float  Bottom	);

	void		set_max( float mMax );
	void 		set_min( float mMin );
	int 		draw   ( 			);

protected:
	PowerLevel left_ch;
	PowerLevel right_ch;
	
};

 
#endif
