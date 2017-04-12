#ifndef _CHECKBOX_
#define _CHECKBOX_

#include "control.hpp"


// Define a CAllback function.


// Offset from <left,bottom> to the check box.
#define BOX_PADDING 4

class CheckBox : public Control
{
public:
	CheckBox();
	CheckBox( int Left, int Right, int Top, int Bottom );
	CheckBox( int Width, int Height );	
	void			Initialize();
	
	virtual void  	set_text		( const char* NewText, bool mWrapContent=false );	// Allocates and copies!		
	virtual int   	draw 			();	
	int 			draw_check_mark ();
	virtual void 	wrap_content    ();
		
	// override set_position to update box_rectangle.
	int				set_height_to_text();
	int				set_check(bool CheckOn=true);

	bool			get_check()		{ return checked;  };
	bool			is_checked()	{ return checked;  };
	void			set_on_click_listener( void (*callback)(void*), void* mOn_click_context );
		
	virtual Control* HitTest ( int x, int y 	);
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	
protected:
	void 	 (*on_click_callback)(void*);
	void* 	 on_click_context;				// often this pointer to a Control*
	
	float	 box_rectangle[4];
	long int box_border_color;
	long int box_background_color;
	
private:
	bool 	checked;
	float 	check_box_side_length;		// assume square.
	
};

 
#endif
