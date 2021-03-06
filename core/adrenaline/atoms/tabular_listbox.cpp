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

#include "control.hpp"
#include "listbox.hpp"
#include "display.h"
#include "tabular_listbox.hpp"

#define margin_percent 0.07
#define Debug 0
 
TabularListBox::TabularListBox( int Left, int Right, int Top, int Bottom )
:ListBox(Left, Right, Top, Bottom )
{
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
	Initialize();
	width  =-1;
	height =-1;
}
TabularListBox::~TabularListBox()
{
}

void TabularListBox::Initialize()
{
	if (Debug) printf("TabularListBox::Initialize()\n");
	ListBox::Initialize();
	strcpy (class_name, "TabularListBox");						
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
	int retval = ListBox::onCreate();
	calc_widths_from_text( );
	calc_column_positions_from_widths( );	
	//printf("TabularListBox::onCreate() column pos \n");		
	return retval;
}

void TabularListBox::print_client_rect(   )		// body_height, scroll_bar width
{
	printf("TabularListBox-ClientRect Info:\t body_height=%d; height=%6.1f; width=%6.1f \n", 
						body_height, height, width );
}

void TabularListBox::print_tab_info(   )
{	
	printf("TabularListBox-Tab Info:\tNumHeadings=%d; \t", Headings.size() );
	for (size_t i=0; i<Headings.size(); i++)
	{
		int sx = Headings[i].start_x;
		printf("%d:\tStart_x=%d; width=%6.1f; %s \n",i, sx, Headings[i].width, 
												Headings[i].text.c_str() );
	}
}

void TabularListBox::clear_items( )  
{
	ListBox::clear_items();
	for(size_t row=0; row<LineData.size(); row++)
	{
		LineData[row].clear();
	}
	LineData.clear();	
	Invalidate();
}

void TabularListBox::calc_metrics( )
{
	ListBox::calc_metrics();
	
	//if (vsb) vsb->set_amount_visible(number_lines_visible);
	if (Debug) printf("TabularLB: calc_metrics: height=%6.2f; body_height=%d\n", 
	height, body_height );	
	
	// The change in height, could change number_lines_visible, which would require scroll bar:
	/*if (LineData.size() > number_lines_visible) {
		enable_v_scroll_bar(true);
		set_v_scroll_values( LineData.size(), 0, first_visible_line, number_lines_visible );
	}*/
}
size_t TabularListBox::get_total_lines( 	)
{
	/*printf("tabularListbox::gettotal_lines\n");*/
	return LineData.size();  
}
	
void TabularListBox::set_width_height( int Width, int Height )
{
	int Ht = Height-(header_text_size*4);
	if (Debug) printf("\tTabularListBox::set_width_height( %d, %d ) lb:Ht %d\n", Width, Height, Ht );
	ListBox::set_width_height( Width, Height );		// Was Ht!!
	calc_metrics();
	if (vsb)   vsb->set_width_height( vsb->width, Height );
	if (Debug) print_positions();
	if (Debug) ListBox::print_positions(); 
}

void TabularListBox::set_row_col_text( string mNewText, int row, int col )
{
	LineData[row][col] = mNewText;
	
	// want to add only if visible...
	Invalidate();
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
	float Hdr_Bottom = Hdr_Top - header_height;   //bottom + body_height;
	int vspace       = ((Hdr_Top-Hdr_Bottom) - header_text_size)/2.0;

	if (Debug) printf("Hdr_Top=%6.1f; Hdr_bottom=%6.1f; \n", Hdr_Top, Hdr_Bottom );
	Stroke_l	( header_border_color 	 );
	Fill_l  	( header_background_color);
	StrokeWidth	( 2.0					 );
	Rect		( left, Hdr_Bottom, width, Hdr_Top-Hdr_Bottom );

	// LOOP THRU ALL HEADINGS:
	float x = left;		// this will keep updating with the width of each heading.
	for (size_t i=0; i < Headings.size(); i++)
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
		
		if (Debug) printf("Header # %d  %6.1f %s\n", i, x, (char*)Headings[i].text.c_str());
		
		//if (Debug) printf("TabularListBox:draw_header: left=%6.2f; bottom=%6.2f;  width=%6.2f; height=%6.2f\n", 
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
	return 1;
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
	//printf( "TabularListBox::draw_one_cell()  x position determined.\n" );

	// Determine the X start pixel:
	if 		(Headings[mCol].alignment == HEADER_ALIGN_LEFT)
		x = Headings[mCol].start_x;
	else if (Headings[mCol].alignment == HEADER_ALIGN_CENTER)
		x = Headings [mCol].start_x + indent;
	else if (Headings[mCol].alignment == HEADER_ALIGN_RIGHT)
		x = Headings[mCol].end_x - text_width;
	//printf("TabularListBox::draw_one_cell()  x position determined.\n");

	Text( x, mY+above_line_offset, (char*)LineData[mRow][mCol].c_str(), SerifTypeface, text_size );
}

void TabularListBox::draw_one_row( int mRow, float mY )
{
	int size=Headings.size();
	//if (Debug) printf("LineData rows=%d/%d; cols=%d\n", mRow, LineData.size(), size );
	for (int col=0; col < size; col++)
	{
		//if (Debug) printf("\t\t%s\n", LineData[mRow][col].c_str() );
		draw_one_cell( mRow, col, mY);
	}
}

/*float TabularListBox::get_line_bottom( int mVisibleIndex )
{
//	if (Debug) 	return ListBox::get_line_bottom( mVisibleIndex );
	float y;
	if (isTopDown)
	{
		y = bottom + body_height - LineHeight * (mVisibleIndex+1);
		return y;
	} else {
		y = bottom + LineHeight * (mVisibleIndex);
		return y;
	}	
}*/
	
int TabularListBox::draw_vertical_lines()
{	
	float x;
	StrokeWidth(2);
	Stroke_l	( 0xFF000000 );
	Fill_l  	( 0xFF0000AF);		
	for (size_t col=1; col< Headings.size(); col++)
	{
		x = Headings[col].start_x - 2;
		if (x>(left+width))
			break;
		//if (Debug) printf("Vertical Lines: x=%6.2f\n", x);
		Line(x, bottom, x, bottom+body_height);	
	}
	return 1;
}

int TabularListBox::draw()
{
	if (Debug) { printf("TabularListBox:draw():\t");  print_positions(); };
	StrokeWidth(1.0);
	Control::draw( );

	draw_line_backgrounds();	
	draw_header();
	if (Debug) printf("TabularListBox:draw_line_backgrounds() done\n");
	
	draw_text();
	if (Debug) printf("TabularListBox:draw_text_() done\n");
	draw_vertical_lines();
	// DRAW Scroll Bars:
	return ScrollControl::draw();	
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

		if (Headings[col].width > 0)	// only do it if requested.
			continue;

		// Each column has the same font.  So the most characters is the most pixels.
		// But bad assumption.  The font may be proportional, so some letters smaller/longer.		
		// Find the longest data row for this column:
		for (size_t row=0; row < LineData.size(); row++)
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
			if (Debug) printf("Data Row column width:  \t\tfinal_width=%6.2f;  longestline=%s\n", final_width, LineData[max_row][col].c_str());
		}

		// Next check the length of the Header:
		float header_width = TextWidth( (char*)Headings[col].text.c_str(), SerifTypeface, header_text_size );
		if (Debug) printf("\t\t Header_width=%6.2f; %s\n", header_width, Headings[col].text.c_str() );

		if (header_width > final_width)
			final_width = header_width;

		if ((mNotToExceed != -1) && (final_width > mNotToExceed))
			final_width = mNotToExceed;

		Headings[col].width = round(final_width);
		if (Debug) printf("Chosen Column Width= %6.1f\n", Headings[col].width );
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
	has_header = true;
	Headings   = *mHeaderTexts;
	calc_widths_from_text();
	calc_column_positions_from_widths();		
}
void TabularListBox::set_header_text( string& mText, int mColumn )
{
	Headings[mColumn].text = mText;
}
	
void TabularListBox::change_header_titles( string mHeaderTexts, int column )
{
	Headings[column].text = mHeaderTexts;
}

void TabularListBox::select( int mIndex )
{
	if (Debug)
		printf("Selected item # %d/%d  visible_line=%d\n", mIndex, LineData.size(), 
				mIndex-first_visible_line);
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
	has_header = true;
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


