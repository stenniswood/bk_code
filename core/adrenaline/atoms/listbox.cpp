// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string> 
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
	//set_position( Left, Right, Top, Bottom );
	Initialize();
}

ListBox::ListBox(int Left, int Bottom, int mWidth, int mNumber_items_shown, int mItem_height )
:ScrollControl()
{
	set_position( Left, Left+mWidth, Bottom+(mNumber_items_shown*mItem_height)+2, Bottom );
	Initialize();
}
ListBox::ListBox( int Width, int Height )
:ScrollControl( Width, Height )
{
	Initialize();	
}
	
ListBox::ListBox()
:ScrollControl()
{
	Initialize();
	set_position( 0, 0, 0, 0 );
}

ListBox::~ListBox()
{
}

void ListBox::Initialize()
{
	ScrollControl::Initialize();
	first_visible_line = 0;	
	EvenColor 		= 0xFFFFFFFF;
	OddColor  		= 0xFF589AE8;
	body_height		= height;
	isTopDown  		= true;
	background_color = 0x00000000;
	border_color 	= 0x20FFFFFF;
	has_header		= false;
	
	text_color 		= 0xFF000000;
	selected_item   = 1;
	selected_color  = 0xFF2C5DCE;		// exact copy of the Mac color selected item active.
	//selected_color_passive = 0xFFCECECE;	// Mac selected item color window inactive.
//	calc_metrics();
}
void ListBox::move_to ( float mLeft,   float  mBottom )
{
	ScrollControl::move_to( mLeft, mBottom );	
}
void ListBox::set_width_height   ( int Width, int Height )
{
	//int d = (height - body_height);
	//printf("ListBox::set_width_height() d=%d\n", d );
	
	ScrollControl::set_width_height( Width, Height );		
	if (has_header)
		body_height = Height - LineHeight;
	else
		body_height = Height;
	if (vsb) vsb->set_width_height( vsb->get_width(), body_height );
	calc_metrics();
}

void ListBox::calc_metrics( )
{
	LineHeight           = text_size * 1.5;
	number_lines_visible = floor( body_height/LineHeight );

	if (vsb) {
		 vsb->set_amount_visible(number_lines_visible);
		 //width -= vsb->width;
	}
	// The change in height, could change number_lines_visible, which would require scroll bar:
	if (LineTexts.size() > number_lines_visible) {
		enable_v_scroll_bar(true);
		if (vsb) vsb->set_width_height( vsb->width, body_height );
		set_v_scroll_values( LineTexts.size(), 0, first_visible_line, number_lines_visible );
	}
	//printf("number_lines_visible=%d; LineHeight=%5.3f\n", number_lines_visible, LineHeight );
}

void ListBox::Fill_mainline(float mleft, float mbottom, float mwidth, float mheight)
{
	Stroke_l(EvenColor);
	Fill_l  (EvenColor);
	Rect    ( mleft, mbottom, mwidth, mheight );
}

void ListBox::Fill_selected_line(float mleft, float mbottom, float mwidth, float mheight)
{
	Stroke_l ( selected_color );
	Fill_l   ( selected_color );
	float fheight = 0.8*mheight;
	float spacey  = (mheight-fheight)/2.;
	Roundrect( mleft+5, mbottom+spacey, mwidth-20, mheight-spacey, fheight, fheight );
}

void ListBox::Fill_alternateline(float mleft, float mbottom, float mwidth, float mheight)
{
	Stroke_l( OddColor );
	Fill_l  ( OddColor  );
	Rect    ( mleft, mbottom, mwidth, mheight );
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

int ListBox::draw_text()
{
	int num_lines = min( number_lines_visible, get_total_lines() );
	//printf("ListBox::draw_text() num_lines=%d  get_total_lines=%d\n", num_lines, get_total_lines() );
	int i=0;
	int screen_i=0;	
	int start = first_visible_line;
	int stop  = first_visible_line+num_lines;

	Fill_l( text_color );
	float y = get_line_bottom( screen_i );
	for (int i=start; i<stop; i++, screen_i++ )
	{
		y = get_line_bottom( screen_i );
		if (y<(body_height+bottom))
			draw_one_row( i, y );
		//printf( "y=%6.3f Num_lines=%d LH=%6.3f, TextSize=%6.3f above=%6.3f\n", y, num_lines, LineHeight, text_size);
	}
}

void ListBox::adjust_height_for_num_visible_items( int mNumber_items_shown )
{
	body_height = (mNumber_items_shown*LineHeight);
	set_width_height(width, body_height);
	calc_metrics();
}

/*  VisibleIndex means the index which is visible on the screen. 
	This is different from the item index (lines may not be visible)
*/
float ListBox::get_line_bottom( int mVisibleIndex )
{
	float y; 
	if (isTopDown)
	{
		y = bottom + body_height - LineHeight * (mVisibleIndex+1);
		return y;
	} else {
		y = bottom + LineHeight * (mVisibleIndex);
		return y;
	}
}

int ListBox::draw_line_backgrounds()
{
	StrokeWidth(1.0);
	// DRAW LINE BACKGROUND COLOR:
	int   i = first_visible_line;
	int   j = 0;
	float y;
	for (; i<(first_visible_line+number_lines_visible); i++, j++)	
	{
		y = get_line_bottom( j );
		if ((i%2)==0)
			Fill_mainline     (left, y, width, LineHeight);
		else 
			Fill_alternateline(left, y, width, LineHeight);

		if (i==selected_item)
			Fill_selected_line(left, y, width, LineHeight);
	}
}

int ListBox::draw()
{
	if (Debug)  printf("ListBox draw(): bottom=%6.1f; height=%6.1f\n", bottom, height);

	// DRAW Scroll Bars:
	ScrollControl::draw();
	draw_line_backgrounds();

	// DRAW TEXT:
	draw_text();
	
	if (vsb) vsb->draw();
}
 
void ListBox::set_text_size( float mTextSize )
{
	Control::set_text_size( mTextSize );
	calc_metrics();
}

void ListBox::clear_items( )  
{
	LineTexts.clear();
	enable_v_scroll_bar(false);
	Invalidate();
}

void ListBox::add_item( const char* mString )
{
	set_item(mString);
}

void ListBox::set_item( const char* mString )
{
	LineTexts.push_back(mString);
	
	if (LineTexts.size() > number_lines_visible)	// We need scroll bars.
	{
		if (vsb) {		// if already enabled, adjust the max value.
			vsb->set_max_value( LineTexts.size() );
			if (Debug) vsb->print_scroll_info();
		} else {
			enable_v_scroll_bar( true );	// if not, enable them.
			set_v_scroll_values( LineTexts.size(), 0, first_visible_line, 
								 number_lines_visible );			
		}
	}
	// or if set_text_size() changes
	// or if set_postion() changes the height
	// need to re-evaluate scrollbar need.
}

void ListBox::print_info(	)
{
	printf("ListBox:: FirstVisibleItem=%d of %d; VisibleLines=%d; body_height=%6.1f; ", 
				first_visible_line, LineTexts.size(),  
				number_lines_visible, body_height  );

	if (isTopDown)		printf("TopDown:true;\t");
	else 				printf("TopDown:false;\t");	
}

// result in pixels:
float ListBox::get_longest_line(  )
{
	float max_w = 0;
	list<string>::iterator iter = LineTexts.begin();	
	while (iter != LineTexts.end())
	{
		float w = TextWidth( (char*)(*iter).c_str(), SerifTypeface, text_size );
		if (w > max_w)
			max_w = w;
		iter++;
	}
	//printf("ListBox::get_longest_line() max_w=%6.1f\n", max_w );
	return max_w;
}

void ListBox::select( int mIndex )
{
	if (Debug) 
		printf("Selected item # %d/%d  visible_line=%d\n", 
			mIndex, LineTexts.size(), mIndex-first_visible_line);
	selected_item = mIndex;
}

// chance to load resources, call functions which use fonts (already loaded before this call) etc.
int ListBox::onCreate( )
{
	ScrollControl::onCreate( );
	calc_metrics( );
}

// Returns the select index of the listbox:
// or -1 if none.
int ListBox::get_hit_index(int mx, int my)
{
	//printf("ListBox::get_hit_index()\n");
	int   start	 = first_visible_line;
	int   end  	 = first_visible_line + number_lines_visible;
	int	  retval = -1;
	float y;
	//printf("\tbottom=%6.1f, body_height=%6.1f LineHeight=%6.1f\n", bottom, body_height, LineHeight);

	for ( int i=start; i<end; i++ )
	{
		y = get_line_bottom( i-start ); 
		//printf( "y=%6.1f\n", y );
		if ( (my>y) && (my<(y+LineHeight)) )
			retval = i;
	}
	string str = *get_item(retval);
	if (Debug) printf("Selected Item # %d : start=%d, end=%d  %s\n", retval, first_visible_line, end, str.c_str() );
	return retval; 
}

Control* ListBox::HitTest(int x, int y)
{
	ScrollControl::HitTest(x,y);
}

int	ListBox::onClick(int Mousex, int Mousey, bool mouse_is_down)
{	
	// Call ScrollControl::onClick() first.  If click is a scrollbar, then handle it.
	int retval = ScrollControl::onClick( Mousex, Mousey, mouse_is_down );
	if (retval>0)
		return retval;
	
	if ((Mousex<left) || (Mousex>(left+width)))
		return 0;

	int   start=first_visible_line;
	int   end  =first_visible_line+number_lines_visible;
	float y;

	if (Debug) {
		printf("ListBox:: on Click()  : start=%d, end=%d\n",  first_visible_line, end);
		printf("ListBox:: on Click()  : mousex,mousey\n");
		print_positions();
	}
	
	int index = get_hit_index( Mousex, Mousey );
	select( index );

	Invalidate();
	return selected_item;
}



