#ifndef _STEREO_POWER_LEVELS_H_
#define _STEREO_POWER_LEVELS_H_

#include "../core/can/bk_system_defs.h"

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

	void 		Initialize();
	void		set_level_left ( float mLevel )	{ left.set_level(mLevel);  };
	void		set_level_right( float mLevel )	{ right.set_level(mLevel); };
	
	void		set_max( float mMax );
	void 		set_min( float mMin );	
	int 		draw( );
	
protected:
	PowerLevel left;
	PowerLevel right;
	
};

 
#endif
