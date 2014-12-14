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
	ScrollBar( int Left,  int Right,  int Top, int Bottom, bool mIsVertical=true );
	ScrollBar( int Width, int Height, bool mIsVertical=true );	
	~ScrollBar();
	void 		  Initialize();	
	
	//virtual void 	set_position	  	 ( int Left, int Right, int Top, int Bottom );
	

	void 			print_scroll_info	( 				 );
	virtual int   	draw 				(				 );	
	void			set_max_value		( long int  mMax   );
	void			set_min_value		( long int  mMin   );
	void			set_amount_visible	( long int  mValue );
	
	long int		get_max				( 				   )	{ return m_MaxValue; };
	long int		get_min				( 				   )	{ return m_MinValue; };
	long int		get_position		( 				   );
	void			scroll_to			( long int  mValue );
	void			scroll_by			( long int  mValue );

	void			page_down			(  );
	void			page_up				(  );
	
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
	// Value Variables:
	long int 	m_MaxValue;
	long int 	m_MinValue;	
	long int 	m_AmountVisibleValue;
	long int 	m_position;				// first visible value (ie position of the top)

	// Drawing Variables (pixels):
	int			m_bar_pixel_length;		// height for vertical; width for horizontal
	int			m_start_pixel;			// bottom for vertical;  left for horizontal
	bool		m_is_dragging;			// cursor is dragging.
	int			m_top_to_y_delta;			// cursor y when clicked
	long int 	bar_color;

private:
	bool		isVertical;
};

 
#endif
