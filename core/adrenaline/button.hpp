#ifndef _BUTTON_
#define _BUTTON_

#include "control.hpp"


class Button : public Control 
{
public:
	Button();
	Button( int Left, int Right, int Top, int Bottom );
	Button( int Width, int Height 					 );
	~Button();
	
	virtual void 	Initialize(	);

	int				set_state	( bool Sink ) { sunken = Sink; };
	virtual int   	draw 		(	);	
	int   			draw_raised (	);	
	int   			draw_sunken (	);	
	virtual int		onClick();
	
private:
	bool sunken;

};

 
#endif
