// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "listbox.hpp"
#include "display.h"


#define margin_percent 0.07
#define Debug 0


ListBox::ListBox(int Left, int Right, int Top, int Bottom )
: ScrollControl(Left, Right, Top, Bottom )
{
	set_position( Left, Right, Top, Bottom );
	Initialize();
}

ListBox::ListBox(int Left, int Bottom, int mWidth, int mNumber_items_shown, int mItem_height )
:ScrollControl()
{
	set_position( Left, Left+mWidth, Bottom+(mNumber_items_shown*mItem_height)+2, Bottom );
	Initialize();
}

ListBox::ListBox()
:ScrollControl()
{
	set_position( 0, 0, 0, 0 );
	Initialize();
}
ListBox::~ListBox()
{
}

void ListBox::Initialize()
{
	ScrollControl::Initialize();
	first_visible_line = 0;	
	EvenColor = 0xFFFFFFFF;
	OddColor  = 0xFF589AE8;
	body_height= height;
	isTopDown  = true;
	text_color = 0xFF000000;
//	calc_metrics();
}

void ListBox::Fill_mainline(float left, float bottom, float width, float height)
{
	Stroke_l(EvenColor);
	Fill_l(EvenColor);
	Rect( left, bottom, width, height );
}

void ListBox::Fill_alternateline(float left, float bottom, float width, float height)
{
	Stroke_l( EvenColor );
	Fill_l  ( OddColor  );
	Rect    ( left, bottom, width, height );
}

void ListBox::setAlpha( float  mAlpha )
{
	unsigned long int a = mAlpha*255;
	EvenColor &= 0x00FFFFFF;
	OddColor  &= 0x00FFFFFF;
	EvenColor |= a;
	OddColor  |= a;
}

string*	ListBox::get_item(int index)
{
	list<string>::iterator iter = LineTexts.begin();
	for (int i=0; i<index; iter++)
		i++;
	return &(*iter);
}

void ListBox::set_first_visible( int mValue )
{
	first_visible_line = mValue;	
	if (vsb)
		vsb->scroll_to(first_visible_line);
}

void ListBox::draw_one_row( int mRow, float mY )
{
	float above_line_offset = (LineHeight-text_size)/2.0;
	Text( left, mY+above_line_offset, (char*)get_item(mRow)->c_str(), SerifTypeface, text_size );
}

int ListBox::draw_text_bottom_up()
{
	int num_lines = min(number_lines_visible, (int)LineTexts.size());
	int i=0;
	int start = first_visible_line;
	int stop  = first_visible_line+num_lines;

	Fill_l(text_color);
	float y;
	for (int i=start; i<stop; i++, y+=LineHeight)
	{
		y = bottom + (i-first_visible_line)*LineHeight;
		draw_one_row( i, y );
	}
}

int ListBox::draw_text_top_down()
{
	int num_lines = min(number_lines_visible, (int)LineTexts.size());
	int i=0;
	int start = first_visible_line;
	int stop  = first_visible_line+num_lines;

	Fill_l(text_color);
	float y;
	for (int i=start; i<stop; i++, y-= LineHeight)
	{
		y = bottom + height - (i-first_visible_line +1)*LineHeight;
		draw_one_row( i, y );
		printf( "y=%6.3f Num_lines=%d LH=%6.3f, TextSize=%6.3f above=%6.3f\n", y, num_lines, LineHeight, text_size);
		//Text( left, y+above_line_offset, (char*)get_item(i)->c_str(), SerifTypeface, text_size );
	}
}

void ListBox::calc_metrics( )
{
	LineHeight           = text_size * 1.5;
	number_lines_visible = ceil( height/LineHeight );

	if (vsb) vsb->set_amount_visible(number_lines_visible);

	// The change in height, could change number_lines_visible, which would require scroll bar:
	if (LineTexts.size() > number_lines_visible) {
		enable_v_scroll_bar(true);
		set_v_scroll_values( LineTexts.size(), 0, first_visible_line, number_lines_visible );
	}
	if (show_header)
		draw_header();
		
	//printf("number_lines_visible=%d; LineHeight=%5.3f\n", number_lines_visible, LineHeight );	
}

void ListBox::adjust_height_for_num_visible_items( int mNumber_items_shown )
{
	height = (mNumber_items_shown*LineHeight)+2;
	set_position( left, left+width, bottom+height, bottom );
	calc_metrics();
}

int ListBox::draw()
{	
	//printf("number_lines_visible=%d\n", number_lines_visible);

	if (Debug) printf("ListBox draw(): bottom=%6.1f; height=%6.1f\n", bottom, height);
	StrokeWidth(1.0);

	int   i=0;
	float y=bottom;
	for (y=bottom; y<(bottom+height); y+=LineHeight, i++)
	{
		if ((i%2)==0)
			Fill_mainline     (left, y, width, LineHeight);
		else 
			Fill_alternateline(left, y, width, LineHeight);
	}
	StrokeWidth(3.0);

	if (isTopDown)
		draw_text_top_down();
	else
		draw_text_bottom_up();

	// DRAW Scroll Bars:
	ScrollControl::draw();
	
	if (show_header)
		draw_header();
}
 
void ListBox::set_text_size( float mTextSize )
{
	Control::set_text_size( mTextSize );
	calc_metrics();
}

void ListBox::ClearItems( )  
{
	LineTexts.clear();
	enable_v_scroll_bar(false);
}

void ListBox::set_item( char* mString )
{
	LineTexts.push_back(mString);
	if (LineTexts.size() > number_lines_visible)
	{
		if (vsb) {
			vsb->set_max_value( LineTexts.size() );
			vsb->print_scroll_info();
		} else {
			enable_v_scroll_bar( true );
			set_v_scroll_values( LineTexts.size(), 0, first_visible_line, 
								 number_lines_visible );			
		}
	}
	// or if set_text_size() changes
	// or if set_postion() changes the height
	// need to re-evaluate scrollbar need.
}
void ListBox::move_to	  		   ( float mLeft,   float  mBottom )
{
	ScrollControl::move_to( mLeft, mBottom );	
}

void ListBox::print_info(	)
{
	printf("ListBox:: FirstVisibleItem=%d of %d; VisibleLines=%d; body_height=%6.1f; ", 
				first_visible_line, LineTexts.size(),  
				number_lines_visible, body_height  );

	if (isTopDown)		printf("TopDown:true;\t");
	else 				printf("TopDown:false;\t");	
	if (show_header)	printf("show_header:true;\t");
	else				printf("show_header:false;\t");

}

void ListBox::set_width_height   ( int Width, int Height )
{
	ScrollControl::set_width_height( Width, Height );
	calc_metrics();
}

void ListBox::draw_header( )
{
	// [intensity, Red, Green, Blue, Alpha]
	VGfloat stops[] = {
	0.0, 0.0, 0.0, 1.0, 1.0,
	0.5, 0.0, 0.0, 0.5, 1.0,
	1.0, 0.0, 0.0, 0.0, 1.0
	};
	float x1 = left;	float y1 = body_height;
	float x2 = width;	float y2 = height;
	FillLinearGradient(x1, y2, x1, y1, stops, 3);
	Rect(x1, y1, x2-x1, y2-y1);
}



