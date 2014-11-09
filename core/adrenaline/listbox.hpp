#ifndef _ListBox_
#define _ListBox_

#include "control.hpp"
#include "scroll_bar.hpp"
#include "scroll_control.hpp"

#include <list>
using namespace std;
#include <string>

#define DEFAULT_SCROLL_WIDTH 10

class ListBox : public ScrollControl 
{
public:
	ListBox();
	ListBox( int Left, int Bottom, int mWidth, int mNumber_items_shown, int mItem_height );
	ListBox( int Left, int Right, int Top, int Bottom  );
	~ListBox();
	
	virtual void 	Initialize();

	void 			Fill_mainline	  (float left, float bottom, float width, float height);
	void 			Fill_selected_line(float left, float bottom, float width, float height);
	void 			Fill_alternateline(float left, float bottom, float width, float height);
	void 			adjust_height_for_num_visible_items( int mNumber_items_shown );

	void			print_info			(	);
	virtual int   	draw 				(	);
	virtual void 	draw_one_row		( int mRow, float mY );
	int 			draw_text_top_down	(	);
	int 			draw_text_bottom_up	(	);
	int 			draw_line_backgrounds(  );
	void 			draw_header			(	);
	void			calc_metrics		(   );
	// move_to virtual is done in scroll_control.cpp

	virtual void  	move_to	  		    ( float mLeft,   float  mBottom );
	virtual void 	set_width_height    ( int Width, int Height );
		
	void 			clear_items			( );
	void			set_item   			( char* mString );
	string*			get_item   			( int index     );
	void			set_first_visible	( int mValue );	
	virtual void  	set_text_size		( float TextSize );
	
	void			set_odd_color   ( long int mColor ) { OddColor =mColor; };
	void			set_even_color  ( long int mColor ) { EvenColor=mColor; };
	void			setAlpha		( float  mAlpha );

	virtual void	select( int mIndex );
	
	virtual Control*	HitTest(int x, int y);
	virtual int			onClick(int x, int y, bool mouse_is_down=true);
	
	//void			enable_scroll_bar( bool mEnable = true );
	
protected:
	void 			initialize();
	int				first_visible_line;
	int   			number_lines_visible;	// this is used anyway despite the scroll bar.
	float 	 		LineHeight;
	bool			isTopDown;
	bool			show_header;		// a header across the top.
	int				body_height;
	int				selected_item;
		
private:
	long int 		EvenColor;
	long int 		OddColor;	
	long int 		selected_color;

	
	list<string> 	LineTexts;
};

//	void	removeItem( char* mString );
//	void	FindItemPtr( char* mString );
//	void	FindTextContaining( char* mString );
//virtual int		onClick();
 
#endif
