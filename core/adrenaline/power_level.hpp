#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "../core/can/bk_system_defs.h"

#define DefaultPadding 15
class Window;

/* This class shows an audio power indicator (1 channel).
   green, yellow, red color gradients.  with rectangular boxes. */

class PowerLevel : public Control
{

public:
	PowerLevel( int Left, int Right, int Top, int Bottom );
	PowerLevel( int Width, int Height 					 );
	PowerLevel();
	~PowerLevel();

	void		set_level( float mLevel );
	void		set_max  ( float mMax );
	void		set_min  ( float mMin );
	
protected:


};

 
#endif
