#ifndef _CHECKBOX_
#define _CHECKBOX_

#include "control.hpp"


class CheckBox : public Control
{
public:
	CheckBox();
	CheckBox( int Left, int Right, int Top, int Bottom );

	virtual int   	draw (	);	
	virtual int		onClick();
	
private:
	bool checked;
	
};

 
#endif
