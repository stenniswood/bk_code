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
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "text_view.hpp"
#include "marquee.hpp"


#define margin_percent 0.07

MarqueeView::MarqueeView(int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{
	style = 0;
	Visible   = FALSE;
	text_size = 14.0;
	font_height = 2*text_size;
	font = &SerifTypeface;
	printf(" %d \n", font->Count);

	calc_margins( 0.1 );
	text_color		= 0xFF9f9f0f;
	BorderColor 	= 0xFFffffff;
	BackgroundColor = 0xFf202020;
}

MarqueeView::MarqueeView()
{
	style = 0;
	set_position( 0, 0, 0, 0 );
	Visible   = FALSE;
	text_size = 14.0;	
	font_height = 2*text_size;
	font = &SerifTypeface;
	calc_margins( 0.1 );
		
	text_color		= 0xFF9f9f0f;
	BorderColor 	= 0xFFffffff;
	BackgroundColor = 0xFf202020;
}

void MarqueeView::set_position ( int Left, int Right, int Top, int Bottom )
{
	Control::set_position(Left, Right, Top, Bottom);
	calc_margins(0.025);
}
	
void MarqueeView::calc_margins( float fraction )
{
	float TheMargin = (width)*fraction;	
	left_margin  = left       +TheMargin;
	right_margin = left+width -TheMargin;
}

void MarqueeView::set_font( Fontinfo* f )
{
	font = f;
}

/* Return the number of characters that will fit in the pixel width */
int MarqueeView::get_num_chars_fit( char* mString, int mWidth )
{
	int length = strlen(mString);
	VGfloat font_width=0.;
	char tmp;	
	//printf("get_num_chars_fit: length=%d; width=%d  %s\n", length, mWidth, mString );

	// Count until width is exceeded:
	for (int i=2; i<length+1; i++)
	{
		// Check the length of 1 character at a time:
		tmp = mString[i];		// Save
		mString[i] = 0;		
		font_width = TextWidth( mString, *font, text_size );		
		mString[i] = tmp;		// Restore

		if (font_width > mWidth)
		{ printf("\n");	return i;	}	// would like to force a return
	}
	printf("\n");
	return length;	// entire string will fit
}

int MarqueeView::count_num_lines_present( char* mString, int mWidth )
{
	
}

void  MarqueeView::set_text( char* NewText )
{	
	text = NewText;
}

/*
mVerticalPix - is height relative to bottom of the control.
*/
char* MarqueeView::draw_one_line( char* mtext, int mVerticalPix )
{
	int strLength = strlen(mtext);
	int pos = strchr(mtext, (int)'\n');
	int length = get_num_chars_fit( mtext, right_margin-left_margin );
//	printf("MarqueeView::draw_one_line width=%d num_chars=%d nl=%d\n", (right_margin-left_margin),
//					 length, pos);
		
	if ((pos>0) && (pos<length))
		length = pos;

	char tmp = mtext[length];
	mtext[length] = 0;

	if (style & CENTER_HORIZONTAL)
	{
		VGfloat font_width = TextWidth( mtext, *font, text_size );
		int space = (width-(font_width))/2;		
		Text(left+space, bottom+mVerticalPix, mtext, *font, text_size );
	} else {
		Text(left_margin, bottom+mVerticalPix, mtext, *font, text_size );
	}
	mtext[length] = tmp;	
	
	if (length==strLength)
		return NULL;			// Done drawing string.
	return &(mtext[length]);
}

/********************************************************************
 Comment on RoundRect & TextMid!!!!
	RoundRect Uses width / height as 2nd and 3rd parameters.
	Text characters come out the color of the Fill, not stroke!!
*********************************************************************/
int MarqueeView::draw()
{	
	Control::draw();

	Stroke_l( BorderColor );		// box color
	StrokeWidth(4);

	Fill_l(BackgroundColor);		// 
	Rect( left, bottom, +width, +height );
	if (text==NULL) return 1;

	Fill_l(text_color);				// 
	int line_height = 1.25*text_size;
	int vertical    = height-line_height;
	char* ptr = text;

	if (style & CENTER_VERTICAL)
	{
		vertical = vertical / 2.0;	
	}
	
	do {
		ptr = draw_one_line( ptr, vertical );
		vertical -= line_height;
		if (vertical < 0)
			break;
	} while ( ptr ); 

}


int	MarqueeView::onClick()
{	
	draw();
	draw();
	return -1;
}


