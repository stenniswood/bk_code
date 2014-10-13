#ifndef _TEXT_VIEW_
#define _TEXT_VIEW_

#include "control.hpp"

#define CENTER_HORIZONTAL 0x01
#define CENTER_VERTICAL   0x02

class TextView : public Control 
{
public:
	TextView();
	TextView( int Left, int Right, int Top, int Bottom );
	
	virtual void 	Initialize			  	(				 );
	void 			calc_margins			( float fraction );
	void 			set_font				( Fontinfo* f	 );
	void 			set_text				( char* NewText  );

	char*  			get_end_of_line			( char* mText    );	// for eol 
	char*  			get_word_break			( char* mString, int mMaxLength );	// for eol (ie. last space before the last non-space letter.
	int  			get_num_chars_fit		( char* mString, int width  );
	int  			count_num_lines_present	( char* mString, int mWidth );
	virtual void  	set_position			( int Left, int Right, int Top, int Bottom );
	
	void center_vertical  		( BOOL on )	{ if (on) style |= CENTER_VERTICAL;	else style &= ~CENTER_VERTICAL;	};
	void center_horizontal		( BOOL on )	{ if (on) style |= CENTER_HORIZONTAL; else style &= ~CENTER_HORIZONTAL;	};

	char* 			draw_one_line( char* mtext, int mVerticalPix );
	virtual int   	draw   		 (	);	
	virtual int		onClick		 (	);

private:
	char*		all_text;
	word		style;
	Fontinfo*	font;
	int			left_margin;
	int			right_margin;
	int			font_height;
	
};

 
#endif
