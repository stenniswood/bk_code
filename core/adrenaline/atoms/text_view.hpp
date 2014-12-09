#ifndef _TEXT_VIEW_
#define _TEXT_VIEW_

#include "scroll_control.hpp"

#define CENTER_HORIZONTAL 0x01
#define CENTER_VERTICAL   0x02

class TextView : public ScrollControl 
{
public:
	TextView( );
	TextView( int Left, int Right, int Top, int Bottom );
	TextView( int Width, int Height );
	
	virtual void 	Initialize			  	(				 );
	void 			calc_margins			( float fraction );
	void 			calc_metrics			( 				 );
	void 			set_font				( Fontinfo* f	 );
	virtual void 	set_width_height  	 	( int Width, int Height 		);
	virtual void  	move_to	  		  	 	( float Left,   float  Bottom	);

	void			load_file				( char* mFullFilename 	);
	char*  			get_end_of_line			( char* mText    		);			// for eol 
	char*  			get_word_break			( char* mString, int mMaxLength );	// for eol (ie. last space before the last non-space letter.
	int  			get_num_chars_fit		( char* mString, int width  	);
	int  			count_num_lines_present	( 								);
	virtual void  	set_position			( int Left, int Right, int Top, int Bottom );
	virtual void  	set_text_size		  	( float TextSize		 		);
		
	void center_vertical  		( BOOL on )	{ if (on) style |= CENTER_VERTICAL;	else style &= ~CENTER_VERTICAL;	};
	void center_horizontal		( BOOL on )	{ if (on) style |= CENTER_HORIZONTAL; else style &= ~CENTER_HORIZONTAL;	};

	char* 			draw_one_line( char* mtext, int mVerticalPix );
	virtual int   	draw   		 (	);	
	virtual int		onClick		 (int x, int y, bool mouse_is_down=true);

protected:
	int 		line_height;

private:
	//char*		all_text;
	word		style;
	Fontinfo*	font;
	int			left_margin;
	int			right_margin;
	//int			font_height;
	
};

 
#endif
