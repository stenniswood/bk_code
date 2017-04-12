#ifndef _FINGER_CURSOR_HPP_
#define _FINGER_CURSOR_HPP_

#include "control.hpp"


class FingerCursor : public Control
{
public:
	FingerCursor();
	~FingerCursor();
	
	virtual int   	draw		(	);
	void 			compose_path();
	

};


#endif
