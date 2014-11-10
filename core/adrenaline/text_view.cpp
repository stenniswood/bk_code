/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
 
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
:ScrollControl(Left, Right, Top, Bottom)
{
	Initialize(	);
	calc_margins( 0.1 );
}
TextView::TextView( int Width, int Height )
:ScrollControl( Width, Height)
{
	Initialize(	);
		
}
TextView::TextView()
:ScrollControl()
{
	set_position( 0, 0, 0, 0 );
	Initialize  (		);
	calc_margins( 0.1 	);
}

void TextView::Initialize(	)
{
	style 			= 0;
	text_size 		= 14.0;
	line_height		= 1.25*text_size;
	font 			= &SerifTypeface;
	text_color		= 0xFF001f00;
	border_color 	= 0xFFffffff;
	background_color= 0xFFE2E2CC;
	calc_margins(0.1);
}

void TextView::set_position ( int Left, int Right, int Top, int Bottom )
{
	Control::set_position(Left, Right, Top, Bottom);
	calc_margins(0.025);
}
	
// need to be recalculated everytime:  set_width_height()

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


/* Out of all the text, get one complete line */
char* TextView::get_end_of_line	( char* mText )	// for eol 
{
	int strLength   	 = strlen(mText);
	char* pos 			 = strchr(mText, (int)'\n' );
	int available_pixels = (right_margin-left_margin);
	int length 			 = get_num_chars_fit( mText, available_pixels );	
	//printf("TextView::get_end_of_line: newline:%d;  fit:%d / %d  availablePixels=%d\n", pos, length, strLength, available_pixels);
	
	// If the line does not reach the margin (ie last line) don't find the word break.
	char tmp = mText[length];  
	mText[length] = 0;
	VGfloat t_width = TextWidth( mText, *font, text_size );
	mText[length] = tmp;
	
	// Text width in pixels < space available:	
	if (t_width < (available_pixels-2))		// does not reach right margin
		return &( mText[length] );			

	char* eol 		= get_word_break( mText, length );
	length 			= (eol-mText);

	// if \n comes first, then readjust for it.
	if ( (pos>0) && ((pos-mText)<length) )
		length = (pos-mText);
	return &(mText[length]);
}


/* There's an error here somewhere!! */
int TextView::count_num_lines_present(  )
{
	int Lines = 0;
	int total_length = strlen(text);
	char* ptr = text;
	char* eol = get_end_of_line( text );
	printf("total length=%d; chars to end_of_line=%d\n", total_length, (eol-text) );
	do {
		eol = get_end_of_line( ptr );
		Lines++;
		ptr = eol+1;
		printf("chars to end_of_line=%d\n", (eol-text) );
	} while ( (ptr-text) < total_length );
	return Lines;
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
		Text(left+space,  bottom+mVerticalPix, mtext, *font, text_size );
	} else {
		Text(left_margin, bottom+mVerticalPix, mtext, *font, text_size );
	}
	*eol = tmp;
	return eol;
}

void  TextView::set_text_size( float TextSize )
{
	ScrollControl::set_text_size( TextSize );
	calc_metrics();
}

/********************************************************************
 Comment on RoundRect & TextMid!!!!
	RoundRect Uses width / height as 2nd and 3rd parameters.
	Text characters come out the color of the Fill, not stroke!!
*********************************************************************/
int TextView::draw()
{
	ScrollControl::draw();

	if (height==0) 	return 1;
	if (text==NULL) return 1;

	// 
	Fill_l(text_color);					// 
	line_height 	= 1.25*text_size;
	printf(" text_size % \n");
	
	int vertical    = height-line_height;
	char* ptr       = text-1;
	
	if (style & CENTER_VERTICAL)
	{
		int num_lines   = count_num_lines_present();
		int text_height = (num_lines*line_height);
		// we print from the top down, and vertical is base of the first line.
		vertical = (height- text_height) / 2.0 + (text_height-line_height);	
	}
	int total_length = strlen(text);
	//printf("TextView::draw(): mid! %s  %d:vertical=%d\n", text, total_length, vertical);

	do {
		ptr = draw_one_line( ptr+1, vertical );
		//printf("TextView::draw(): mid! len=%d   %s \n", (ptr-text), ptr+1);	
		vertical -= line_height;
		if (vertical < 0)
			break;
		//printf("loop 1 completed\n");
	} while ( (ptr-text) < total_length ); 
	//printf("TextView::draw(): done! \n");	
}

void TextView::calc_metrics(  )
{
	printf("calc_metrics:  text_size=%6.1f\n", text_size);
	line_height 		  = 1.25*text_size;
	int num_visible_lines = (height / line_height);
	printf(": num_visible_lines=%d\n", num_visible_lines );
	int max 			  = count_num_lines_present();
	printf("calc_metrics:  lines present=%d\n", max);
	int first_visible 	  = 0;
	if (vsb)
		first_visible     = vsb->get_position();

	printf("TextView::calc_metrics():  max=%d; min=0; num_visible_lines=%d\n",
			max, num_visible_lines );
	set_v_scroll_values(  max, 0, first_visible, num_visible_lines );
}

void TextView::load_file( char* mFullFilename )
{
	struct stat buf;
	FILE* fd = fopen( mFullFilename, "r" );
	printf("load_file:: filename=%s;  \t\tfd=%x\n", mFullFilename, fd);
	if (fd==NULL)
	{
		text = "File not found";
	}
	else
	{
		// Get the filesize
		errno=0;
		int result = stat(mFullFilename, &buf);
		int errsv = errno;
		//perror(" -fstat error- ");
		int FileSize = buf.st_size;
		printf("load_file: result=%d/%d;  fizesize=%d\n", result, errsv, FileSize);
				
		text = new char[FileSize];
		int bytes_read=0;
		char* ptr = text;
		// need to allocate entire file at once.
		bytes_read = fread( text, 1, FileSize, fd );
		if (bytes_read != FileSize)
		{
			printf("ERROR Loading Text File!  Bytes read=%d!\n", bytes_read );
		}				
		printf("Loaded Text File:  Bytes read=%d\n", bytes_read );
		calc_metrics();
	}
}

void TextView::set_width_height( int Width, int Height )
{
	Control::set_width_height(Width,Height);
	calc_margins(0.1);
}
void TextView::move_to ( float Left,   float  Bottom )
{
	Control::move_to(Left,Bottom);
	calc_margins(0.1);
}

int	TextView::onClick(int x, int y, bool mouse_is_down)
{	
	return -1;
}


