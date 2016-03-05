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

	virtual void  set_text  	( const char* NewText, bool mWrapContent=false );	// Allocates and copies!	
	virtual void  wrap_content	( );
	int			  join_group	( RadioButton* mNewButton );
	int			  select    	(  );
	float		  get_longest_width();
	void		  expand_group_widths();		// make all radiobuttons in this group the width of the longest.

	//	virtual Control*	HitTest		  ( int x, int y 	);
		
	virtual int   draw 	 ( );	
	virtual	int	  onClick(int x, int y, bool mouse_is_down=true);
	
private:
	float		box_rectangle[4];
	bool		checked;
	long int	selected_color;
	
	
	RadioButton* Next;
	RadioButton* Prev;	

};

 
#endif
