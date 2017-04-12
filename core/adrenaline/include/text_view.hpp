#ifndef _TEXT_VIEW_
#define _TEXT_VIEW_

#include "scroll_control.hpp"

// Mask 0x03
#define TEXTVIEW_ALIGN_MASK_HORIZ 0xFFFC
#define TEXTVIEW_ALIGN_LEFT 	 0x00
#define TEXTVIEW_ALIGN_CENTER 0x01
#define TEXTVIEW_ALIGN_RIGHT  0x02

#define TEXTVIEW_CENTER_HORIZONTAL 0x01

// Mask 0x04
#define TEXTVIEW_CENTER_VERTICAL   0x04

class TextView : public ScrollControl 
{
public:
	TextView( );
	TextView( int Left, int Right, int Top, int Bottom );
	TextView( int Width, int Height );

	virtual void 	Initialize			  	(				 );
	void 			calc_margins			( float fraction = -1.0 );
	void 			calc_metrics			( 				 );
	void			print_info				(	);
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
 
	void 			set_alignment_horizontal( byte mAlign=TEXTVIEW_ALIGN_LEFT );		
	void 			center_vertical  		( BOOL on );
	void 			center_horizontal		( BOOL on );

	char* 			draw_one_line( char* mtext, int mVerticalPix );
	virtual int   	draw   		 (	);	
	virtual int		onClick		 (int x, int y, bool mouse_is_down=true);

protected:
	float 		line_height;
	int			max_num_visible_lines;
	int			first_visible_line;	
	int			num_lines_present;

private:
	//char*		all_text;
	word		style;
	Fontinfo*	font;
	float		m_left_margin;
	float		m_right_margin;
	float		m_fraction;
	
};

 
#endif
