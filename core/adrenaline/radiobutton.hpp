#ifndef _RADIO_BUTTON_
#define _RADIO_BUTTON_


#include "control.hpp"

#define RADIO_BUTTON_CIRCLE_RADIUS 9

class RadioButton : public Control
{
public:
	RadioButton();
	RadioButton( int Left, int Right, int Top, int Bottom );
	RadioButton( int Width, int Height );
	
	void 		  Initialize();	
	
	int			  join_group( RadioButton* mNewButton );
	int			  select    (  );
	
	virtual int   draw 	 ( );	
	virtual	int	  onClick( );
	
private:
	float		box_rectangle[4];
	bool		checked;
	long int	selected_color;
	
	
	RadioButton* Next;
	RadioButton* Prev;	

};

 
#endif
