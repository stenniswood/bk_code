#ifndef _CONTROL_W_SCROLL_
#define _CONTROL_W_SCROLL_

//#include <list>
#include "control.hpp"
#include "scroll_bar.hpp"
//#include <list>
//using namespace std;
//#include <string> 


#define DEFAULT_VSCROLL_WIDTH 10
#define DEFAULT_HSCROLL_HEIGHT 10 

/**************************************
This is an abstract class which has a scroll bar
built in.  Used in classes such as TextView,
ListBox, etc.  Which often need scroll capability.
This offers is readily builtin.
***************************************/

class ScrollControl : public Control 
{
	
public:
	ScrollControl();
	ScrollControl( int Left, int Right, int Top, int Bottom );
	ScrollControl( int Width, int Height );
	~ScrollControl();
	
	virtual void	Initialize		 (   );	
	virtual int   	draw 			 (	 );
	void			calc_metrics	 (   );

	void			enable_v_scroll_bar( bool mEnable );
	void 			enable_h_scroll_bar( bool mEnable,int max, int min, int first_visible, 
										 int amount_visible );

	virtual void  	move_to	  		   ( float mLeft,   float  mBottom );
	virtual void 	set_width_height   ( int Width, int Height );

	void 			set_v_scroll_values( int max, int min, int first_visible, 
										 int amount_visible );
		
	void 			set_first_visible_v( int mValue );
	void			set_first_visible_h( int mValue );

	void 			set_amount_visible_v( int mValue );
	void 			set_amount_visible_h( int mValue );

	virtual Control*	HitTest(int x, int y);
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
		
protected:
	int				v_first_visible_value;
	int   			v_visible_value;
	int				h_first_visible_value;
	int   			h_visible_value;

	ScrollBar*		vsb;
	ScrollBar*		hsb;

private:

};

 
#endif
