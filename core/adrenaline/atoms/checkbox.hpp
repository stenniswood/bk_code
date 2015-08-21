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
	
	virtual int   	draw 			();	
	int 			draw_check_mark ();
	virtual void 	wrap_content    ();
		
	// override set_position to update box_rectangle.
	int				set_height_to_text();
	int				set_check(bool CheckOn=true);
	bool			is_checked()	{ return checked;  };
	
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	
protected:
	float	 box_rectangle[4];
	long int box_border_color;
	long int box_background_color;
	
private:
	bool 	checked;
	float 	check_box_side_length;		// assume square.
	
};

 
#endif
