#ifndef _MARQUEEVIEW_H_
#define _MARQUEEVIEW_H_

#include "control.hpp"

#define CENTER_HORIZONTAL 0x01
#define CENTER_VERTICAL   0x02

#define CONTINUOUS_SCROLL  0x04
#define SCROLL_STOP_SCROLL 0x08


class MarqueeView : public Control 
{
public:
	MarqueeView();
	MarqueeView( int Left, int Right, int Top, int Bottom );

	void calc_margins	( float fraction );
	void set_font		( Fontinfo* f	 );
	void set_text		( char* NewText  );
	
	int  get_num_chars_fit		( char* mString, int width  );
	int  count_num_lines_present( char* mString, int mWidth );
	virtual void  set_position	( int Left, int Right, int Top, int Bottom );
	
	void center_vertical  ( BOOL on )	{ if (on) style |= CENTER_VERTICAL;	else style &= ~CENTER_VERTICAL;	};
	void continuous_scroll( BOOL on )	{ if (on) style |= CONTINUOUS_SCROLL;	else style &= ~CONTINUOUS_SCROLL;	};
	void scroll_stop_scroll( BOOL on )	{ if (on) style |= SCROLL_STOP_SCROLL;	else style &= ~SCROLL_STOP_SCROLL;	};
	//void center_horizontal( BOOL on )	{ if (on) style |= CENTER_HORIZONTAL; else style &= ~CENTER_HORIZONTAL;	};
		
	char* 			draw_one_line( char* mtext, int mVerticalPix );
	virtual int   	draw   (	);	
	virtual int		onClick(	);
	
private:
	word		style;
	Fontinfo*	font;
	int			left_margin;
	int			right_margin;
	int			font_height;
	
	long int BorderColor;
};

 
#endif
