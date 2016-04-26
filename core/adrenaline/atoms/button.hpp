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

	void			set_state	( bool Sink ) { sunken = Sink; };
	virtual int   	draw 		(	);	
	int   			draw_raised (	);	
	int   			draw_sunken (	);	
	int				set_on_click_listener( void (*callback)(void*), void* mOn_click_context );

	virtual int		onClick(int x, int y, bool mouse_is_down=true);

	bool hot_touch;		// these buttons do not redraw when clicked.	
private:
	bool sunken;
	
	void (*on_click_callback)(void*);
	void* on_click_context;				// often this pointer to a Control*
	
};

 
#endif
