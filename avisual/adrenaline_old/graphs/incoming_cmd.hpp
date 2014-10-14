#ifndef _VOICE_CMD_
#define _VOICE_CMD_


#include "control.hpp"

/* Abstract Base Class for all graphs */
class IncomingCmd : public Control
{
public:
	IncomingCmd( int Left, int Right, int Top, int Bottom );
	IncomingCmd();
	
	virtual int   draw			(	);		// Draw calls all the sub draw routines.

private:
		
	
}

#endif
