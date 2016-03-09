// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <time.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "adrenaline_windows.h"
#include "calendar.hpp"


const 	float x_margin = 10.;

const int CALENDAR_DEFAULT_HEIGHT = 200;
const int CALENDAR_DEFAULT_WIDTH  = 250;
char days_of_week_1_letter[] = { 'S', 'M', 'T', 'W', 'T', 'F', 'S' };

#define Debug 1
#define dprintf if (Debug) printf

void prev_month_cb( void* mCalendar )
{
	Calendar* cal = (Calendar*) mCalendar;
	dprintf("prev_month_cb!!  %d \n", cal->m_shown_month );
	cal->previous_month();
	cal->Invalidate();
}

void next_month_cb( void* mCalendar )
{
	Calendar* cal = (Calendar*) mCalendar;
	dprintf("next_month_cb!!  %d \n", cal->m_shown_month );
	cal->next_month();
	cal->Invalidate();
}

void close_calendar_cb( void* mCalendar )
{
	MainDisplay.hide_calendar();
}


string days_of_week_abrev[] = {
	"Sun", "Mon", "Tue", "Wed", "Thurs", "Fri", "Sat"
};
string days_of_week[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

string Months[12] = { 
		"January",  "February",  "March",  
		"April", 	"May",  	"June",  
		"July",  	"August", 	"September",
		"October",  "November", "December" };


Grid::Grid()
{
	m_column_start_x = new vector<int>();
	m_row_bottom_y   = new vector<int>();
}
Grid::Grid(int mLeft, int mRight, int mTop, int mBottom )
{
	m_column_start_x = new vector<int>();
	m_row_bottom_y   = new vector<int>();

	set_boundary(mLeft, mRight, mTop, mBottom);	
}
Grid::~Grid()
{
}
void Grid::set_boundary (int mLeft, int mRight, int mTop, int mBottom )
{
	left   = mLeft;
	bottom = mBottom;
	width  = mRight - mLeft;
	height = mTop- mBottom;
}
void Grid::create(int mRows, int mCols)
{
	float y = bottom;
	float x = left;
	float y_inc = height / mRows;
	float x_inc = width / mCols;

	m_row_bottom_y->clear();
	m_column_start_x->clear();
	for (int r=0; r<mRows; r++)
	{	m_row_bottom_y->push_back( trunc(y) );	y+= y_inc;		}
	for (int c=0; c<mCols; c++)
	{	m_column_start_x->push_back( trunc(x) );	x+= x_inc;		}

	m_row_bottom_y->push_back( bottom+height );
	m_column_start_x->push_back( left+width );	
}
void Grid::hit_test(int mx, int my)
{
	int rows = m_row_bottom_y->size();
	int cols = m_column_start_x->size();
	m_hit_row = -1;
	m_hit_col = -1;
	for (int r=0; r<rows; r++)
	{
		if (my < (*m_row_bottom_y)[r])
		{	m_hit_row = r-1;  break; }
	}	
	for (int c=0; c<cols; c++)
	{
		if (mx < (*m_column_start_x)[c]) 
		{	m_hit_col = c-1;	break; }
	}

	if ((m_hit_col>=0) && (m_hit_row>=0))
		printf("GRID HIT:  row,col=<%d,%d>\n", m_hit_row, m_hit_col );
}

void Grid::draw ()
{	
	for (int r=0; r<m_row_bottom_y->size(); r++)
		Line( left, (*m_row_bottom_y)[r], left+width, (*m_row_bottom_y)[r] );

	//printf("Grid: # Cols=%d\n", m_column_start_x.size() );
	for (int c=0; c<m_column_start_x->size(); c++)
		Line( (*m_column_start_x)[c], bottom, (*m_column_start_x)[c], bottom+height );	

	// Draw Inverted square if clicked:		
	if ((m_hit_col>=0) && (m_hit_row>=0))
	{
		float t_left   = (*m_column_start_x)[m_hit_col];
		float t_bottom = (*m_row_bottom_y  )[m_hit_row];
		float t_width  = (*m_column_start_x)[m_hit_col+1] - (*m_column_start_x)[m_hit_col];
		float t_height = (*m_row_bottom_y)[m_hit_row+1]   - (*m_row_bottom_y)[m_hit_row];
		//Stroke_l( box_border_color );
		Fill_l  ( 0xFFFFFFFF );	
		Rect( t_left, t_bottom, t_width, t_height );
	}
		
}

/********************************************************************************
							class Calendar
********************************************************************************/
Calendar::Calendar() 
{ 
	Initialize(	);
}
Calendar::Calendar( Rectangle* mRect ) 
{ 
	Initialize(	);
}
Calendar::Calendar( int Width, int Height ) 
{ 
	Initialize(	);
}
Calendar::~Calendar() 
{ 
}

void Calendar::Initialize(	) 
{
	strcpy(class_name, "Calendar");
	
	time_t t 	  =  time(NULL);
	m_local_time  =  *localtime(&t);	
	m_shown_month = m_local_time.tm_mon;
	m_shown_year  = m_local_time.tm_year+1900;
	m_shown_day	  = m_local_time.tm_mday;
	
	background_color = 0xFFA0002C;
	set_width_height( CALENDAR_DEFAULT_WIDTH, CALENDAR_DEFAULT_HEIGHT );

	m_prev.set_on_click_listener( prev_month_cb, this );
	m_next.set_on_click_listener( next_month_cb, this );
	m_close.set_on_click_listener( close_calendar_cb, this );
	
	register_child( &m_prev );
	register_child( &m_next );	
	register_child( &m_close );
}

void	Calendar::show_date	( int mMonth, int mDay ) 
{
	m_shown_month = mMonth;
	m_shown_day   = mDay;
	compute_date_info();
}	

int Calendar::onCreate	(	) 
{ 
	int retval = Control::onCreate();
	m_prev.set_text( "<", true );
	m_next.set_text( ">", true );
	m_close.set_text( " . ", true );	
	
	place_views();
	return retval;
}

int Calendar::draw 		(	)
{	
	//printf("Calendar::draw()\n");
	if (Visible==false) { return 0; }
	
	dprintf("Time=%d:%d:%d\n",      m_local_time.tm_hour, m_local_time.tm_min, m_local_time.tm_sec );
	dprintf("Date: %d %s :%d:%d\n", m_local_time.tm_mon, Months[m_local_time.tm_mon].c_str(),
								   m_local_time.tm_mday, m_local_time.tm_year+1900 );
	dprintf("%s (%d)\nBUTTONS: \n", days_of_week[m_local_time.tm_wday].c_str(), m_local_time.tm_wday );
	
	Control::draw(); 
	switch(m_display_mode)
	{
	case 0 : draw_1month_view_small();
			break;
	case 1 : draw_1month_view();
			break;
	case 2 : draw_simple_view();
			break;
	case 3 : draw_1week_view();
			break;
	default: 	return 0;
			break;			
	}
	return 1;
}

void	Calendar::place_views()
{
	float top = bottom+height;	
 	m_next.set_position( left+width-m_next.get_width(), left+width, top, top-m_next.get_height() );
 	m_prev.set_position( left, left+m_prev.get_width(), top, top-m_prev.get_height() );
 	m_close.move_to( left+width-m_close.get_width(),  bottom );

	// Must Match the draw() 	
	float w = (width-2*x_margin);
	float y= bottom + height - (text_size*1.25) - (text_size*1) - (text_size*1.5)+4;	
 	m_grid.set_boundary ( left+x_margin, left+x_margin+w, y, y-5*(text_size*1.5) );
 	m_grid.create(5,7);
}

int Calendar::draw_simple_view(	) 
{ 
	return 1;
}
int Calendar::draw_1month_view(	)
{
	return 1;
}

void Calendar::previous_month()
{
	m_shown_month--;
	if (m_shown_month<0) {
		m_shown_month = 12;
		m_shown_year--;
	}
}

void Calendar::next_month()
{
	m_shown_month++;
	if (m_shown_month>12) {
		m_shown_month = 0;
		m_shown_year++;
	}
}


int Calendar::draw_1month_view_small(	) 
{
	char date_str[24];
	char date[8];

	//m_grid.draw();
	
	// DRAW MONTH NAME
	text_color = 0xFFFF0000;
	Fill_l   ( text_color );
	Stroke_l ( 0xFFFFFF00 );		
	float x= left + m_prev.get_width() + 20.;
	float y= bottom + height - (text_size*1.25);
	Text(x, y, Months[m_shown_month].c_str(), SerifTypeface, text_size );

	// DRAW YEAR
	sprintf(date_str, "%4d", m_shown_year );	
	int text_width = TextWidth( date_str, SerifTypeface, text_size );
	float year_x = left+width - m_next.get_left()-text_width;
	dprintf("Year is: %s; m_next.left=%d;  x=%6.1f; width=%d\n", date_str, m_next.get_left(), year_x, text_width );
	TextEnd(year_x, y, date_str, SerifTypeface, text_size );	// 
	

	// DRAW DAYS of the WEEK "M,T,W,T,F,S,S") : 
	x= left + 10.;
	y -= (text_size*2);
	date_str[0] = 0;
	for (int i=0; i<7; i++) 
	{
		sprintf(date, " %2c ", days_of_week_1_letter[i] );
		strcat (date_str, date  );
	}
	dprintf("Date Str (1week) %6.2f %6.2f : %s \n", x, y, date_str );
	Text(x, y, date_str, SerifTypeface, text_size );

	// get Current Day:
	time_t   first_day_t;
	time( &first_day_t );
	struct tm* lt = localtime( &first_day_t );
	m_local_time = *lt;
	int today = lt->tm_mday;
	int this_month = lt->tm_mon;
	
	// DRAW NUMBERS : 
	float y_increment = (text_size*1.5);
	y -= y_increment;
	float day_width = (width-2*x_margin) / 7.;
	int month_offset = -1;
	int c=0;
	for (int w=0; w<5; w++)		// week in month.
	{
		x = left + x_margin;
		dprintf("Date:  " );
		for (int d=0; d<7; d++) 
		{			
			sprintf(date, "%2d", m_days_shown[c]);
			dprintf("%s ", date );
			if ((m_days_shown[c]==m_shown_day) && (month_offset==0)) {		/* Highlight Today! */
				Fill_l   ( 0xFFFFFFFF );
				Stroke_l ( 0xFFFFFF00 );		
				Rect(x,y, day_width, y_increment );
				Fill_l   ( text_color );
				Stroke_l ( 0xFFFFFF00 );						
			}
			if ((m_days_shown[c]==today) && (m_shown_month+month_offset==this_month)) {		/* Highlight Today! */
				Fill_l   ( 0xFFFFFFFF );
				Stroke_l ( 0xFFFFFF00 );		
				Rect(x,y, day_width, y_increment );
				Fill_l   ( text_color );
				Stroke_l ( 0xFFFFFF00 );						
			}
			Text(x, y, date, SerifTypeface, text_size );
			x += day_width;
			c++;
		}
		dprintf("\n");
		y -= y_increment;
	}
	return 1;
}

void Calendar::fill_in_days_shown_array( int mStartDay /* prev month */ )
{
	int day = mStartDay;
	int month_offset = -1;
	int c=0;
	for (int w=0; w<5; w++)		// week in month
	{
		for (int d=0; d<7; d++, day++) 
		{			
			if ((day>m_prev_month_end) && (month_offset==-1)) { day = 1; month_offset=0; }
			if ((day>m_this_month_end) && (month_offset==0))  { day = 1; month_offset=1; }
			m_days_shown[c++] = day;
			day++;
		}
	}	
}

/* Outputs are:
	m_shown_weekday,
	m_prev_month_end,
	m_this_month_end,
	m_days_shown[0]		The starting day for the calendar (ie. Sunday of prev month)
*/
void Calendar::compute_date_info()
{
	// Calc number of days to back off for the first week.
	static struct tm som;	
	som.tm_mon  = m_shown_month;
	som.tm_mday = 1;
	som.tm_year = m_shown_year-1900;
	som.tm_hour = 1; som.tm_min = 0; som.tm_sec = 0;
	
	time_t first_day_t = mktime( &som );			// fill in day of week.
	dprintf("First of the %s month is on %s %d \n", Months[som.tm_mon].c_str(), 
					days_of_week[som.tm_wday].c_str(), som.tm_wday );
	m_shown_weekday    = som.tm_wday;
	
	// Get Last day of previous month : 
	som.tm_mday = 0;
	mktime( &som );
	m_prev_month_end = som.tm_mday;
	
	// Get Last day of shown month : 
	som.tm_mon  = m_shown_month+1;  // have to put it to the next month minus 1 day!
	som.tm_mday = 0;				// minus 1
	mktime( &som );
	m_this_month_end = som.tm_mday;	
	
	// Now back off number of weekdays:
	som.tm_mon  = m_shown_month;
	som.tm_year = m_shown_year-1900;
	som.tm_mday = 1-m_shown_weekday;
	mktime( &som );
	dprintf("Cal starts with %d (%d) month_end=%d\n", som.tm_mday, som.tm_mon, m_prev_month_end );
	m_days_shown[0] = som.tm_mday;
	
	fill_in_days_shown_array( m_days_shown[0] );
}

int Calendar::draw_1week_view (	) 
{ 
	return 1;
}

int	Calendar::set_on_click_listener( void (*callback)(void*), void* mOn_click_context ) 
{ 
	return 1;
}

int	Calendar::find_date_clicked( int x, int y )
{
	int month_offset = -1;
	int day = -1;
	float margin = 10.;
	float grid_left;
	float day_width = (width-2*margin) / 7.;	
	// Find which Day was clicked:
	// (make grid of dates), then find what number is in that grid point.
	float grid_bottom = bottom + height - (text_size*1.25);
	float y_increment = (text_size*1.5);
	if (y<grid_bottom) 
	{
		for (int w=0; w<5; w++)		// week in month.
		{
			grid_left = left + margin;
			for (int d=0; d<7; d++, day++) 
			{
				if ((day>m_prev_month_end) && (month_offset==-1)) { day = 1; month_offset=0; }
				if ((day>m_this_month_end) && (month_offset==0))  { day = 1; month_offset=1; }
				grid_left += day_width;
				if ((x<grid_left) && (y>grid_bottom) )
				{
					return day;
				}
			}
			grid_bottom -= y_increment;
		}
	}	
	return -1;	
}

int Calendar::onClick(int x, int y, bool mouse_is_down) 
{
	m_grid.hit_test(x,y);
	int date = find_date_clicked(x,y); 
	Control::onClick(x,y,mouse_is_down); 
	return 1;	
}



