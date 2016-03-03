// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <time.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "adrenaline_windows.h"
#include "calendar.hpp"


const int CALENDAR_DEFAULT_HEIGHT = 200;
const int CALENDAR_DEFAULT_WIDTH  = 250;
char days_of_week_1_letter[] = { 'S', 'M', 'T', 'W', 'T', 'F', 'S' };

#define Debug 0
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
	m_prev.set_text( "<", true );
	m_next.set_text( ">", true );

	m_prev.set_on_click_listener( prev_month_cb, this );
	m_next.set_on_click_listener( next_month_cb, this );
	
	register_child( &m_prev );
	register_child( &m_next );	
	place_views();	
}

int	Calendar::show_date	( int mMonth, int mDay ) 
{
 	return 0;
}	

int Calendar::onCreate	(	) 
{ 
	int retval = Control::onCreate();
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
	
	// DRAW DAYS of the WEEK : 
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

	// Calc number of days to back off for the first week.
	static struct tm som;	
	som.tm_mon  = m_shown_month;
	som.tm_mday = 1;
	som.tm_year = m_shown_year-1900;
	som.tm_hour = 1; som.tm_min = 0; som.tm_sec = 0;
	time_t first_day_t = mktime( &som );
	dprintf("First of the %s month is on %s %d \n", Months[som.tm_mon].c_str(), 
					days_of_week[som.tm_wday].c_str(), som.tm_wday );
	int wday    = som.tm_wday;
	
	// Compute days until start of month (to show on first week of the month)
	// Get Last day of previous month : 
	som.tm_mday = 0;
	mktime( &som );
	int prev_month_end = som.tm_mday;
	// Get Last day of shown month : 
	som.tm_mon  = m_shown_month+1;  // have to put it to the next month minus 1 day!
	som.tm_mday = 0;				// minus 1
	mktime( &som );
	int this_month_end = som.tm_mday;	
	// Now back off number of weekdays:
	som.tm_mon  = m_shown_month;
	som.tm_year = m_shown_year-1900;
	som.tm_mday = 1-wday;
	mktime( &som );
	dprintf("Cal starts with %d (%d) month_end=%d\n", som.tm_mday, som.tm_mon, prev_month_end );

	// get Current Day:
	time( &first_day_t );
	struct tm* lt = localtime( &first_day_t );
	m_local_time = *lt;
	int today = lt->tm_mday;
	int this_month = lt->tm_mon;
	
	// DRAW NUMBERS : 
	float margin = 10.;
	float y_increment = (text_size*1.5);
	y -= y_increment;
	int day = som.tm_mday;
	float day_width = (width-2*margin) / 7.;
	int month_offset = -1;
	for (int w=0; w<5; w++)		// week in month.
	{
		x = left + margin;
		dprintf("Date:  " );
		for (int d=0; d<7; d++, day++) 
		{
			if ((day>prev_month_end) && (month_offset==-1)) { day = 1; month_offset=0; }
			if ((day>this_month_end) && (month_offset==0))  { day = 1; month_offset=1; }
			
			sprintf(date, "%2d", day);
			dprintf("%s ", date );
			if ((day==today) && (m_shown_month+month_offset==this_month)) {		/* Highlight Today! */
				Fill_l   ( 0xFFFFFFFF );
				Stroke_l ( 0xFFFFFF00 );		
				Rect(x,y, day_width, y_increment );
				Fill_l   ( text_color );
				Stroke_l ( 0xFFFFFF00 );						
			}
			Text(x, y, date, SerifTypeface, text_size );
			x += day_width;
		}
		dprintf("\n");
		y -= y_increment;
	}
	return 1;
}

int Calendar::draw_1week_view (	) 
{ 
	return 1;
}

int	Calendar::set_on_click_listener( void (*callback)(void*), void* mOn_click_context ) 
{ 
	return 1;
}
int Calendar::onClick(int x, int y, bool mouse_is_down) 
{ 
	Control::onClick(x,y,mouse_is_down); 
	return 1;	
}



