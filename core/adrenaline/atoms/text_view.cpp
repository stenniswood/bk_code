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
#include "global_funcs.h"

#define margin_percent 0.07
#define Debug 0


TextView::TextView(int Left, int Right, int Top, int Bottom )
:ScrollControl(Left, Right, Top, Bottom)
{
	Initialize(	);
}

TextView::TextView( int Width, int Height )
:ScrollControl( Width, Height)
{
	Initialize(	);
}

TextView::TextView()
:ScrollControl()
{
	//set_position( 0, 0, 0, 0 );
	Initialize  (		);	
}

void TextView::Initialize(	)
{
	ScrollControl::Initialize();
	strcpy (class_name, "TextView");	
	
	//if (Debug) printf("TextView::Initialize()\n");
	style 			= 0;
	text_size 		= 14.0;
	font 			= &SerifTypeface;
	text_color		= 0xFF001f00;
	border_color 	= 0xFFffffff;
	background_color= 0xFFE2E2CC;
	m_fraction = 0.05;
	calc_margins();
	calc_metrics();
	//if (Debug) printf("TextView::Initialize() done\n");
}

void TextView::set_position ( int Left, int Right, int Top, int Bottom )
{
	Control::set_position(Left, Right, Top, Bottom);
	calc_margins(0.025);
}
	
// need to be recalculated everytime:  set_width_height()
void TextView::calc_margins( float fraction )
{
	if (fraction != -1.0)	m_fraction = fraction;
	//printf("TextView::calc_margins(%6.2f)  %6.2f %6.2f \n", m_fraction, m_left_margin, m_right_margin);
	
	float TheMargin = (width*m_fraction)/2.;
	m_left_margin  = left       +TheMargin;
	m_right_margin = left+width -TheMargin;
}

void TextView::print_info(	)
{
	printf("TextView::  text_size =%6.1f;  \n", text_size );	
	printf("line_height          \t=%6.2f ",	line_height );
	printf("num_visible_lines    \t=%d ", 		max_num_visible_lines );
	printf("first_visible_line   \t=%d", 		first_visible_line );
	printf("max_num_visible_lines\t=%d;\n", 	max_num_visible_lines );
	printf("num_lines_present    \t=%d;\n", 	num_lines_present );
}

//	printf("calc_metrics:  text_size=%6.1f\n", text_size);
//	printf(": num_visible_lines=%d\n", num_visible_lines );
//	printf("calc_metrics:  lines present=%d\n", max );
void TextView::calc_metrics(  )
{
	if (text_size==0) return;
	
	line_height 		   	  = 1.5*text_size;
	int max_num_visible_lines = (height / line_height);
	int num_lines_present 	  = count_num_lines_present();
	 
	//if (Debug) printf("TextView::calc_metrics() mid\n");
	first_visible_line 	  = 0;
	if (vsb)
		first_visible_line = vsb->get_position();
	
	set_v_scroll_values(  num_lines_present, 0, first_visible_line, max_num_visible_lines );	
	//if (Debug) printf("TextView::calc_metrics() done\n");
}

void TextView::set_font( Fontinfo* f )
{
	font = f;
}

/* Return the number of characters that will fit in the pixel width 
	The string should not contain any \n characters.
*/
int TextView::get_num_chars_fit( char* mString, int mWidthPixels )
{
	int length = strlen(mString);
	VGfloat font_width=0.;
	char tmp;
	//printf("get_num_chars_fit: length=%d; width=%d font=%d %s\n", length, mWidth, font, mString );

	// Count until width is exceeded:
	for (int i=0; i<length+1; i++)
	{
		// Check the length of 1 character at a time:
		tmp = mString[i];		// Save
		if (tmp=='\n')
			return i;

		mString[i] = 0;		
		font_width = TextWidth( mString, *font, text_size );		
		mString[i] = tmp;		// Restore
		//printf("get_num_chars_fit: font_width=%d; width=%d  %s\n", font_width, mWidth, mString );

		if (font_width > mWidthPixels)
		{ return i;	}	// would like to force a return
	}
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
	//int   strLength   	   = strlen(mText);
	char* pos 			   = strchr(mText, (int)'\n' );
	if (pos==NULL)	   pos = strchr(mText, (int)'\r' );
	
	int   available_pixels = (m_right_margin-m_left_margin);
	if (pos==mText) 
		return pos;
	
	int length 			   = get_num_chars_fit( mText, available_pixels );	
	//if (Debug)   printf("TextView::get_end_of_line: newline at %d;  %d/%d chars fit  availablePixels=%d\n", 
	//					pos, length, strLength, available_pixels );

	// If the line does not reach the margin (ie last line) don't find the word break.
	char tmp = mText[length];
	mText[length]   = 0;
	VGfloat t_width = TextWidth( mText, *font, text_size );
	mText[length]   = tmp;

	// Text width in pixels < space available:
	if (t_width < (available_pixels-2))		// does not reach right margin
		return &( mText[length] );

	//printf("TextView::Calling Word Break() %s %d\n", mText, length );
	char* eol 		= get_word_break( mText, length );
	length 			= (eol-mText);

	// if \n comes first, then readjust for it.
	if ((pos>0) && ((pos-mText)<length))
		length = (pos-mText);
	//printf("TextView:: %d \n", length);
	
	return &(mText[length]);
}

/* There's an error here somewhere!! */
int TextView::count_num_lines_present(  )
{
	/* To do this we have to work with the TextWidth and font.  Because proportional 
	   fonts change how much is on a line and thus how many lines are present.    */
	if (text==NULL) return 0;

	int Lines 		 = 0;
	int total_length = strlen(text);
	char* ptr 		 = text;
	char* eol = NULL;
	
	do {
		eol = get_end_of_line( ptr );
		char remember = 0;
		if (eol) {
			remember = *eol;	// eol
			*eol = 0;
			Dprintf("length_from_start=%d; eol_length=%d;|%s|\n", (eol-text), (eol-ptr), ptr );
			*eol = remember;		// restore
		}
		Lines++;				// Count it!
		ptr = eol+1;
	} while ( (eol-text) < total_length );
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
	VGfloat font_width = TextWidth( mtext, *font, text_size );
	const int pad = 10;
	
	if (style & TEXTVIEW_ALIGN_CENTER)
	{
		int space = (width-(font_width))/2;		
		Text(left+space,  bottom+mVerticalPix, mtext, *font, text_size );
	} else if (style & TEXTVIEW_ALIGN_RIGHT) {		
		TextEnd(left+width-pad,  bottom+mVerticalPix, mtext, *font, text_size );	
	} else 	{
		Text(m_left_margin, bottom+mVerticalPix, mtext, *font, text_size );
	}
	*eol = tmp;
	return eol;
}

void  TextView::set_text_size( float TextSize )
{
	ScrollControl::set_text_size( TextSize );
	calc_metrics();
}

void TextView::set_alignment_horizontal( byte mAlign )	
{ 
	style &= TEXTVIEW_ALIGN_MASK_HORIZ; 
	style |= mAlign;
	printf(" TextView::set_alignment_horizontal() style=%d\n", style );
}
void TextView::center_vertical  	( BOOL on )	
{ 
	if (on) style |= TEXTVIEW_CENTER_VERTICAL;	
	else style &= ~TEXTVIEW_CENTER_VERTICAL;	
}
void TextView::center_horizontal	( BOOL on )	
{ 
	if (on) style |= TEXTVIEW_CENTER_HORIZONTAL; 
	else style &= ~TEXTVIEW_CENTER_HORIZONTAL;	
}



/********************************************************************
 Comment on RoundRect & TextMid!!!!
	RoundRect Uses width / height as 2nd and 3rd parameters.
	Text characters come out the color of the Fill, not stroke!!
*********************************************************************/
int TextView::draw()
{
	ScrollControl::draw();
 
	if (height==0) 	return 0;
	if (text==NULL) return 0;

	// 
	Fill_l(text_color);					// 
	line_height 	= 1.25*text_size;
	//if (Debug)  printf("TextView::draw text_size %X \n", text_size);
	
	int vertical    = height-line_height;
	char* ptr       = text-1;
	
	if (style & TEXTVIEW_CENTER_VERTICAL)
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
	return 1;
}

char NotFoundMsg[] = "File not found";

void TextView::load_file( char* mFullFilename )
{
	struct stat buf;
	FILE* fd = fopen( mFullFilename, "r" );
	if (Debug) 	printf("load_file:: filename=%s;\t\tfd=%p\n", mFullFilename, fd);
	if (fd==NULL)
	{
		text = NotFoundMsg;
	}
	else
	{
		// Get the filesize
		errno=0;
		int result = stat( mFullFilename, &buf );
		int errsv = errno;
		//perror(" -fstat error- ");
		int FileSize = buf.st_size;
		if (Debug) 	printf("load_file: result=%d/%d;  fizesize=%d\n", result, errsv, FileSize);
				
		text = new char[FileSize];
		int bytes_read=0;
		//char* ptr = text;
		// need to allocate entire file at once.
		bytes_read = fread( text, 1, FileSize, fd );
		if (bytes_read != FileSize)
		{
		if (Debug) printf("ERROR Loading Text File!  Bytes read=%d!\n", bytes_read );
		}				
		if (Debug) printf("Loaded Text File:  Bytes read=%d\n", bytes_read );
		calc_metrics();
	}
}

void TextView::set_width_height( int Width, int Height )
{
	Control::set_width_height(Width,Height);
	calc_margins();
}
void TextView::move_to( float Left,   float  Bottom )
{
	Control::move_to(Left,Bottom);
	calc_margins();
}

int	TextView::onClick(int x, int y, bool mouse_is_down)
{
	return -1;
}
