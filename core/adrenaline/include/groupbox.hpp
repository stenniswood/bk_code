#ifndef _GROUPBOX_
#define _GROUPBOX_


#include "control.hpp"


class GroupBox : public Control
{
public:
	GroupBox();
	GroupBox( int Left, int Right, int Top, int Bottom );
	virtual int draw();
	
private:


};

 
#endif
