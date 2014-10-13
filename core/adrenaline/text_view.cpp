/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "text_view.hpp"


#define margin_percent 0.07

TextView::TextView(int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{
	Initialize(	);
	calc_margins( 0.1 );
}

TextView::TextView()
{
	set_position( 0, 0, 0, 0 );
	Initialize(	);
	calc_margins( 0.1 );		
}

void TextView::Initialize(	)
{
	style 			= 0;
	text_size 		= 14.0;
	font_height 	= 2*text_size;
	font 			= &SerifTypeface;
	text_color		= 0xFF9f9f0f;
	border_color 	= 0xFFffffff;
	background_color= 0xFf202020;
}

void TextView::set_position ( int Left, int Right, int Top, int Bottom )
{
	Control::set_position(Left, Right, Top, Bottom);
	calc_margins(0.025);
}
	
void TextView::calc_margins( float fraction )
{
	float TheMargin = (width)*fraction;	
	left_margin  = left       +TheMargin;
	right_margin = left+width -TheMargin;
}

void TextView::set_font( Fontinfo* f )
{
	font = f;
}

/* Return the number of characters that will fit in the pixel width */
int TextView::get_num_chars_fit( char* mString, int mWidth )
{
	int length = strlen(mString);
	VGfloat font_width=0.;
	char tmp;	
	//printf("get_num_chars_fit: length=%d; width=%d font=%d %s\n", length, mWidth, font, mString );

	// Count until width is exceeded:
	for (int i=2; i<length+1; i++)
	{
		// Check the length of 1 character at a time:
		tmp = mString[i];		// Save
		mString[i] = 0;		
		font_width = TextWidth( mString, *font, text_size );		
		//printf("get_num_chars_fit: font_width=%d; width=%d  %s\n", font_width, mWidth, mString );
		mString[i] = tmp;		// Restore

		if (font_width > mWidth)
		{ printf("\n");	return i;	}	// would like to force a return
	}

	printf("\n");
	return length;	// entire string will fit
}
int TextView::count_num_lines_present( char* mString, int mWidth )
{
	
}

// for eol (ie. last space before the last non-space letter.
char* TextView::get_word_break( char* mString, int mMaxLength )
{
	// First remove whitespace from the end of the line!
	// No.  If it ends in space already, just return that char*
	
	// Just incase the string exceeds the desired length.  
	// (ie size of the letters and string is the entire text)	
	char restore = mString[mMaxLength];
	mString[mMaxLength] = 0;

	char* delim1 = strrchr(mString, ' ');
	char* delim2 = strrchr(mString, '\t');
	char* delim = max(delim1, delim2);

	mString[mMaxLength] = restore;
	return delim;
}

void  TextView::set_text( char* NewText )
{	
	all_text = NewText;
}

/* Out of all the text, get one complete line */
char* TextView::get_end_of_line	( char* mText )	// for eol 
{
	int strLength   = strlen(mText);
	char* pos 		= strchr(mText, (int)'\n' );
	int pixel_width = (right_margin-left_margin);
	//printf("TextView::get_end_of_line :  MIddle of \n");
	int length 		= get_num_chars_fit( mText, pixel_width );		

	// If the line does not reach the margin (ie last line) don't find the word break.
	char tmp = mText[length];  mText[length] = 0;
	VGfloat t_width = TextWidth( mText, *font, text_size );
	mText[length] = tmp;	
	if (t_width < (pixel_width-2))	// does not reach right margin
		return &(mText[length]);	// 
	
	char* eol 		= get_word_break( mText, length );
	length 			= (eol-mText);

	// if \n comes first, then readjust for it.
	if ( (pos>0) && ((pos-mText)<length) )
		length = (pos-mText);
	return &(mText[length]);
}

/*
mVerticalPix - is height relative to bottom of the control.
*/
char* TextView::draw_one_line( char* mtext, int mVerticalPix )
{
	char* eol = get_end_of_line( mtext );
	char tmp = *eol;
	*eol = 0;

	if (style & CENTER_HORIZONTAL)
	{
		VGfloat font_width = TextWidth( mtext, *font, text_size );
		int space = (width-(font_width))/2;		
		Text(left+space, bottom+mVerticalPix, mtext, *font, text_size );
	} else {
		Text(left_margin, bottom+mVerticalPix, mtext, *font, text_size );
	}
	*eol = tmp;
	return eol;
}

/********************************************************************
 Comment on RoundRect & TextMid!!!!
	RoundRect Uses width / height as 2nd and 3rd parameters.
	Text characters come out the color of the Fill, not stroke!!
*********************************************************************/
int TextView::draw()
{
	Control::draw();

	if (height==0) 		return 1;
	if (all_text==NULL) return 1;


	Fill_l(text_color);				// 
	int line_height = 1.25*text_size;
	int vertical    = height-line_height;
	char* ptr       = all_text-1;
	print_positions();
	
	if (style & CENTER_VERTICAL)
	{
		vertical = vertical / 2.0;
	}
	int total_length = strlen(all_text);
	//printf("TextView::draw(): mid! %s  %d:vertical=%6.2f\n", all_text, total_length, vertical);

	do {
		ptr = draw_one_line( ptr+1, vertical );
		//printf("TextView::draw(): mid! len=%d   %s \n", total_length, ptr+1);	
		vertical -= line_height;
		if (vertical < 0)
			break;
		//printf("loop 1 completed\n");
	} while ( (ptr-all_text) < total_length );
	//printf("TextView::draw(): done! \n");	
}


int	TextView::onClick()
{	
	draw();
	draw();
	return -1;
}


