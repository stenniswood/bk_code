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
#include "tabular_listbox.hpp"

#define margin_percent 0.07
#define Debug 1
 
TabularListBox::TabularListBox( int Left, int Right, int Top, int Bottom )
:ListBox(Left, Right, Top, Bottom )
{
	set_position( Left, Right, Top, Bottom );
	Initialize();
}

TabularListBox::TabularListBox( int Width, int Height )
:ListBox( Width, Height )
{
	Initialize();
}

TabularListBox::TabularListBox(int Left, int Bottom, int mWidth, int mNumber_items_shown, int mItem_height )
:ListBox(Left, Bottom, mWidth, mNumber_items_shown, mItem_height)
{
	if (mItem_height==-1)
		mItem_height = text_size * 1.5;	

	set_position( Left, Left+mWidth, Bottom+(mNumber_items_shown*mItem_height)+2, Bottom );
	Initialize();
}

TabularListBox::TabularListBox()
:ListBox()
{
	//printf("TabularListBox ctor \n" );
	//set_position( 0, 0, 0, 0 );
	Initialize();
	width  =-1;
	height =-1;
}
TabularListBox::~TabularListBox()
{
}

void TabularListBox::Initialize()
{
	printf("TabularListBox::Initialize()\n");
	ListBox::Initialize();
	header_text_color       = 0xFF000000;
	header_background_color = 0xFF9f9f9f;
	header_border_color     = 0xFFBFBFBF;
	header_text_size 		= 14.0;

	show();
	show_border(true);
	calc_metrics();
}

int	TabularListBox::onCreate(  	)	// after loading resources, call functions which use fonts (already loaded before this call) etc.	
{
	printf("\t\tTabularListBox::onCreate()\n");
	Control::onCreate();

	calc_widths_from_text( );
	calc_column_positions_from_widths( );	
	printf("TabularListBox::onCreate() column pos \n");		
}

void TabularListBox::print_client_rect(   )		// body_height, scroll_bar width
{
	printf("TabularListBox-ClientRect Info:\t body_height=%6.1f; height=%6.1f; width=%6.1f \n", body_height, height, width );
}

void TabularListBox::print_tab_info(   )
{	
	printf("TabularListBox-Tab Info:\tNumHeadings=%d; \t", Headings.size() );
	for (int i=0; i<Headings.size(); i++)
	{
		int sx = Headings[i].start_x;
		printf("%d:\tStart_x=%d; width=%d; %s \n", sx, Headings[i].width, Headings[i].text.c_str() );
	}
}

void TabularListBox::calc_metrics( )
{
	header_height 		 = header_text_size * 1.5;
	body_height 		 = (height - header_height);
	if (text_size==0) 
		text_size = 12;
	LineHeight           = text_size * 1.5;
	number_lines_visible = floor( body_height/LineHeight );

	printf("TabularLB: calc_metrics: height=%6.2f; body_height=%6.2f\n", height, body_height );	
	
	if (vsb) vsb->set_amount_visible(number_lines_visible);

	// The change in height, could change number_lines_visible, which would require scroll bar:
	if (LineData.size() > number_lines_visible) {
		enable_v_scroll_bar(true);
		set_v_scroll_values( LineData.size(), 0, first_visible_line, number_lines_visible );
	}
}

void TabularListBox::set_width_height( int Width, int Height )
{
	ListBox::set_width_height( Width, Height );
	calc_metrics();
	if (vsb)  vsb->set_width_height( vsb->width, body_height );
}

void TabularListBox::adjust_height_for_num_visible_items ( int mNumber_items_shown )
{
	float NewHeight = (mNumber_items_shown*LineHeight)+2;
	set_width_height( width, NewHeight );
}

vector<string>*	TabularListBox::get_line_data( int mIndex )
{
	return &(LineData[mIndex]);
}

void TabularListBox::set_column_width( int mColumn, int mWidth )
{	
	Headings[mColumn].width = mWidth;
	calc_metrics();
}

int TabularListBox::draw_header(	)
{
	// CALCULATE PIXEL LOCATIONS:
	float Hdr_Top    = bottom + height;
	float Hdr_Bottom = bottom + body_height;
	int vspace       = ((Hdr_Top-Hdr_Bottom) - header_text_size)/2.0;

	//printf("Hdr_Top=%6.1f; Hdr_bottom=%6.1f; \n", Hdr_Top, Hdr_Bottom );
	Stroke_l	( header_border_color 	 );
	Fill_l  	( header_background_color);
	StrokeWidth	( 2.0					 );
	Rect		( left, Hdr_Bottom, width, Hdr_Top-Hdr_Bottom );

	// LOOP THRU ALL HEADINGS:
	float x = left;		// this will keep updating with the width of each heading.
	for (int i=0; i < Headings.size(); i++)
	{
		x = Headings[i].start_x;
		if ((x>(left+width)) || (Headings[i].end_x>(left+width)))
		{
			Fill_l  	( 0xFF2f0000 );
			float rw = (left+width) - x;
			Rect		( x, Hdr_Bottom, rw, Hdr_Top-Hdr_Bottom );
			break;
		}

		// DRAW THE RECTANGLE:
		Stroke_l	( header_border_color 	 );
		Fill_l  	( header_background_color);
		StrokeWidth	( 2.0					 );
		Rect( x, Hdr_Bottom, Headings[i].width, Hdr_Top-Hdr_Bottom ); 
		
		printf("Header # %d  %6.1f %s\n", i, x, (char*)Headings[i].text.c_str());
		
		//printf("TabularListBox:draw_header: left=%6.2f; bottom=%6.2f;  width=%6.2f; height=%6.2f\n", 
		//	Headings[i].start_x, Hdr_Bottom, Headings[i].width, Hdr_Top-Hdr_Bottom );	
		Fill_l  	( header_text_color );
		
		// DRAW THE TEXT:
		if (Headings[i].alignment & HEADER_ALIGN_LEFT)
		{
			Text( x, Hdr_Bottom+vspace, (char*)Headings[i].text.c_str(), SerifTypeface, header_text_size );
		}
		else if (Headings[i].alignment & HEADER_ALIGN_CENTER)
		{
			float midx = x + Headings[i].width / 2.0;
			TextMid( midx, Hdr_Bottom+vspace, (char*)Headings[i].text.c_str(), SerifTypeface, header_text_size );
		}
		else if (Headings[i].alignment & HEADER_ALIGN_RIGHT)
		{
			TextEnd( Headings[i].end_x, Hdr_Bottom+vspace, (char*)Headings[i].text.c_str(), SerifTypeface, header_text_size );
		}
	}
}

// This class implements strings only for the view.
// this could be overridden - another class which draws "icon - string"
// for instance.
void TabularListBox::draw_one_cell( int mRow, int mCol, float mY )
{
	if (Headings[mCol].end_x > (left+width))
		return;
	
	// Compute measurements for correctly placing the text:
	float text_width = TextWidth( (char*)LineData[mRow][mCol].c_str(), SerifTypeface, text_size );
	int   col_space  = (Headings[mCol].width - text_width);
	int   indent     = col_space / 2.0;
	float above_line_offset = (LineHeight-text_size)/2.0;
	int x;
	printf( "TabularListBox::draw_one_cell()  x position determined.\n" );

	// Determine the X start pixel:
	if 		(Headings[mCol].alignment == HEADER_ALIGN_LEFT)
		x = Headings[mCol].start_x;
	else if (Headings[mCol].alignment == HEADER_ALIGN_CENTER)
		x = Headings [mCol].start_x + indent;
	else if (Headings[mCol].alignment == HEADER_ALIGN_RIGHT)
		x = Headings[mCol].end_x - text_width;
	printf("TabularListBox::draw_one_cell()  x position determined.\n");

	Text( x, mY+above_line_offset, (char*)LineData[mRow][mCol].c_str(), SerifTypeface, text_size );
}

void TabularListBox::draw_one_row( int mRow, float mY )
{
	int size=Headings.size();
	//printf("LineData rows=%d/%d; cols=%d\n", mRow, LineData.size(), size );
	for (int col=0; col < size; col++)
	{
		//printf("\t\t%s\n", LineData[mRow][col].c_str() );
		draw_one_cell( mRow, col, mY);
	}
}

float TabularListBox::get_line_bottom( int mVisibleIndex )
{
//	return ListBox::get_line_bottom( mVisibleIndex );
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
	
int TabularListBox::draw_vertical_lines()
{	
	float x,y1,y2;
	StrokeWidth(2);
	Stroke_l	( 0xFF000000 );
	Fill_l  	( 0xFF0000AF);		
	for (int col=1; col< Headings.size(); col++)
	{
		x = Headings[col].start_x - 2;
		if (x>(left+width))
			break;
		//printf("Vertical Lines: x=%6.2f\n", x);
		Line(x, bottom, x, bottom+body_height);	
	}
}

int TabularListBox::draw()
{
	if (Debug) { printf("TabularListBox:draw():\t");  print_positions(); };
	StrokeWidth(1.0);
	Control::draw( );

	draw_line_backgrounds();	
	draw_header();
	printf("TabularListBox:draw_line_backgrounds() done\n");
	
	draw_text();
	printf("TabularListBox:draw_text_() done\n");
	draw_vertical_lines();
	// DRAW Scroll Bars:
	ScrollControl::draw();
}

/* This scans all rows and the header to find max number of pixels.
   Stores the result in Header width.
*/
void TabularListBox::calc_widths_from_text( int mNotToExceed )
{	
	int num_columns   = Headings.size();
	int max_chars     = 0;
	int max_row       = -1;
	float final_width = 0;
	int ColumnSpacing = 5.0;

	for (int col=0; col<num_columns; col++)
	{
		max_chars = 0;

		// Each column has the same font.  So the most characters is the most pixels.
		// But bad assumption.  The font may be proportional, so some letters smaller/longer.		
		// Find the longest data row for this column:
		for (int row=0; row < LineData.size(); row++)
		{
			int len = LineData[row][col].length();
			if (len > max_chars)
			{
				max_chars = len;
				max_row   = row;
			}
		}				
		if (max_row>=0)
		{
			final_width = TextWidth( (char*)LineData[max_row][col].c_str(), SerifTypeface, text_size );
			final_width += ColumnSpacing;
			printf("Data Row column width:  \t\tfinal_width=%6.2f;  longestline=%s\n", final_width, LineData[max_row][col].c_str());
		}

		// Next check the length of the Header:
		float header_width = TextWidth( (char*)Headings[col].text.c_str(), SerifTypeface, header_text_size );
		printf("\t\t Header_width=%6.2f; %s\n", header_width, Headings[col].text.c_str() );

		if (header_width > final_width)
			final_width = header_width;

		if ((mNotToExceed != -1) && (final_width > mNotToExceed))
			final_width = mNotToExceed;

		Headings[col].width = round(final_width);
		printf("Chosen Column Width= %d\n", Headings[col].width );
	}
}

void TabularListBox::calc_column_positions_from_widths( )
{
	int num_cols = Headings.size();
	if (num_cols>0)
	{	
		Headings[0].start_x = left;
		Headings[0].end_x   = Headings[0].start_x + Headings[0].width;

		for (int col=1; col<num_cols; col++)
		{
			// Calculate the start_x:
			Headings[col].start_x = Headings[col-1].end_x + 1;	

			// Calculate the end_x:
			Headings[col].end_x = Headings[col].start_x + Headings[col].width;
		}	
//		if (width==-1)
//			width = Headings[num_cols-1].end_x;
	}
}

void TabularListBox::move_to( float Left, float Bottom )
{
	ListBox::move_to( Left, Bottom );
	calc_column_positions_from_widths();
}

void TabularListBox::set_headings( vector<struct HeaderItemInfo> *mHeaderTexts )
{
	//Headings = *mHeaderTexts;
	calc_widths_from_text();
	calc_column_positions_from_widths();		
}

void TabularListBox::change_header_titles( string mHeaderTexts, int column )
{
	Headings[column].text = mHeaderTexts;
}

void TabularListBox::select( int mIndex )
{
//	printf("Selected item # %d/%d  visible_line=%d\n", mIndex, LineData.size(), 
//				mIndex-first_visible_line);
	selected_item = mIndex;
}

// 
void TabularListBox::add_row( vector<string> *mData )
{
	LineData.push_back(*mData);
	if (LineData.size() > number_lines_visible)
	{
		if (vsb) {
			vsb->set_max_value( LineData.size() );
			//vsb->print_scroll_info();
		} else {
			enable_v_scroll_bar( true );
			set_v_scroll_values( LineData.size(), 0, first_visible_line, 
								 number_lines_visible );			
		}
	}
	// or if set_text_size() changes
	// or if set_postion() changes the height
	// need to re-evaluate scrollbar need.
}

// row data will be increased and blanked!
// Will be added to the last column!
void TabularListBox::add_column( struct HeaderItemInfo* mNewHeading )
{
	Headings.push_back( *mNewHeading );
	
	int rows  = LineData.size();
	for (int i=0; i<rows; i++)
	{
		LineData[i].push_back( " " );
	}
}

void TabularListBox::update_col_data( vector<string> *mNewColText, int mColumn  )
{
	int incoming_rows = mNewColText->size();
	int current_rows  = LineData.size();
	int rows = min( incoming_rows, current_rows );
	
	for (int i=0; i<rows; i++)
	{
		LineData[i][mColumn] = (*mNewColText)[rows];
		
	}
}


