#ifndef _BUTTON_
#define _BUTTON_

#include "control.hpp"


class Button : public Control 
{
public:
	Button();
	Button( int Left, int Right, int Top, int Bottom );


	int				setState( bool Sink ) {sunken = Sink; };
	
	virtual int   	draw (	);	
	virtual int		onClick();
	
private:
	bool sunken;	

	long int TextColor;
	long int BorderColor;
	long int BackgroundColor;	
};

 
#endif
