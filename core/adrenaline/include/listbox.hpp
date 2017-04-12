#ifndef _ListBox_
#define _ListBox_

#include <list> 
#include <string>
#include <string.h>

#include "control.hpp"
#include "scroll_bar.hpp"
#include "scroll_control.hpp"
#include "touch_gesture.hpp"

using namespace std;


#define DEFAULT_SCROLL_WIDTH 10

struct stColor_set {
	unsigned long even;
	unsigned long odd;
	unsigned long selected;
	unsigned long text;
};


/*const struct stColor_set color_set[] = {
			 { 0xFFFFFFFF, 0xFF589AE8, 0xFF2C5DCE }, 
			 { 0xFF000000, 0xFF9F9F9F, 0xFF272727 }, 
			 { 0xFF000000, 0xFFFFFFFF, 0xFF272727 } 
	}; */

/*
	List box does not have a header.  Use HeaderListBox for that!
	
*/
class ListBox : public ScrollControl 
{
public:
	ListBox( );
	ListBox( int Left, int Bottom, int mWidth, int mNumber_items_shown, int mItem_height );
	ListBox( int Left, int Right,  int Top,    int Bottom );
	ListBox( int Width, int Height );
	~ListBox();
	
	virtual void 	Initialize			(	);
	virtual void  	move_to	  		    ( float mLeft, float  mBottom );
	virtual void 	set_width_height    ( int Width, int Height );
	void			calc_metrics		(   );

	void 			Fill_mainline	  	( float left, float bottom, float width, float height);
	void 			Fill_selected_line	( float left, float bottom, float width, float height);
	void 			Fill_alternateline	( float left, float bottom, float width, float height);
	void			set_selected_color  ( long int mColor ) { selected_color= mColor; };
	void			set_odd_color   	( long int mColor ) { OddColor = mColor; };
	void			set_even_color  	( long int mColor ) { EvenColor= mColor; };
	void			set_color_scheme  	( struct stColor_set& mColorSet );
	void			setAlpha			( float  mAlpha );

	void 			adjust_height_for_num_visible_items( int mNumber_items_shown );

	void			print_info			(	);
	virtual int   	draw 				(	);
	virtual void 	draw_one_row		( int mRow, float mY );
	int 			draw_text			(	);
	int 			draw_line_backgrounds(  );

	void			set_top_down		( bool mIsTopDown ) { isTopDown = mIsTopDown; };
	void 			clear_items			( );

	void 			add_item			( const char* mString );
	void			set_item   			( const char* mString );
	string*			get_item   			( int index =-1     );
	virtual float	get_line_bottom		( int mVisibleIndex );
	virtual size_t	get_total_lines		( );
	int				get_selection		( );
	
	void			set_first_visible	( int mValue 	 );
	virtual void  	set_text_size		( float TextSize );
	virtual void	select				( int mIndex 	 );
	float			get_longest_line	( 				 );	// result in pixels		

	int					get_hit_index	( int x, int y );
	virtual Control*	HitTest			( int x, int y );
	virtual int			onClick			( int x, int y, bool mouse_is_down=true);
	virtual int			onHover		  	( int x, int y	) { return 0; };
	virtual int			onCreate	  	(  );
	virtual int			onGesture	  	( DraggerGesture* mGesture );

	//void			enable_scroll_bar	( bool mEnable = true );
	bool			has_header;
	float			header_height;			// pixels
	float			header_text_size;
	long int		header_text_color;
	long int		header_border_color;
	long int		header_background_color;

protected:
	int				first_visible_line;
	size_t			number_lines_visible;	// this is used anyway despite the scroll bar.
	float 	 		LineHeight;
	bool			isTopDown;
	int				body_height;
	uint16_t		selected_item;
	
private:
	long int 		EvenColor;
	long int 		OddColor;	
	long int 		selected_color;

	list<string> 	LineTexts;
};

/*class HeaderListBox : public ListBox
{
public:
	HeaderListBox();

	bool			has_header;
	float			header_height;			// pixels
	float			header_text_size;
	long int		header_text_color;
	long int		header_border_color;
	long int		header_background_color;
	
};
*/

//	void	removeItem( char* mString );
//	void	FindItemPtr( char* mString );
//	void	FindTextContaining( char* mString );
 
#endif
