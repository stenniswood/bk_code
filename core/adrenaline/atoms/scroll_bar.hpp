#ifndef _SCROLL_BAR_
#define _SCROLL_BAR_


#include "control.hpp" 
//#include "scroll_control.hpp"
class ScrollControl;
class TabularListBox;

#define NO_HIT  0 
#define HIT_BAR 1
#define HIT_SPACE 2

class ScrollBar : public Control 
{
	friend class ListBox;
	friend class ScrollControl;
	friend class TabularListBox;

public:
	ScrollBar( bool mIsVertical=true );
	ScrollBar( int Left, int Right, int Top, int Bottom, bool mIsVertical=true );
	~ScrollBar();
	void 		  Initialize();	
	
	//virtual void 	set_position	  	 ( int Left, int Right, int Top, int Bottom );
	
	void 			print_scroll_info	( 				 );
	virtual int   	draw 				(				 );	
	void			set_max_value		( long int  mMax   );
	void			set_min_value		( long int  mMin   );
	void			set_amount_visible	( long int  mValue );
	
	long int		get_max				( 				   )	{ return MaxValue; };
	long int		get_min				( 				   )	{ return MinValue; };
	long int		get_position		( 				   );
	void			scroll_to			( long int  mValue );
	void			scroll_by			( long int  mValue );
	
	void			calc_bar_pixels		( );
	void  			set_bar_color    	( long mColor ) { bar_color= mColor; };
		
	void			calc_vertical_bar_pixels( );
	void			calc_horizontal_bar_pixels( );
	
// add speed/friction etc?
	void			set_orientation		( bool mIsVertical=true ) { isVertical=mIsVertical; }

	int				HitTestArea(int x, int y);
	// need to put mouse_is_down into Control:: and all derived classes!
	int		 		onClick	(int x, int y, bool mouse_is_down);

	
protected:
	long int 		interpolate(float bar_top);

protected:
	long int 	MaxValue;
	long int 	MinValue;	
	long int 	AmountVisibleValue;
	long int 	position;				// first visible value (ie position of the top)

	long int 	bar_color;
	int			bar_pixel_length;		// height for vertical; width for horizontal
	int			start_pixel;			// bottom for vertical;  left for horizontal
	bool		is_dragging;			// cursor is dragging.
	int			drag_y_to_top;					// cursor y when clicked

private:
	bool		isVertical;
};

 
#endif
