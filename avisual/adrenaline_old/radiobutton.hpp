#ifndef _RADIO_BUTTON_
#define _RADIO_BUTTON_


#include "control.hpp"


class RadioButton : public Control
{
public:
	RadioButton();
	RadioButton( int Left, int Right, int Top, int Bottom );
	
	virtual int   draw 	 ( );	
	virtual	int	  onClick( );
	
private:
	bool	checked;
	
};

 
#endif
