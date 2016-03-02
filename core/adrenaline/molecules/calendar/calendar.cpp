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


#define DEBUG 1

void prev_month_cb( void* mCalendar )
{
	Calendar* cal = (Calendar*) mCalendar;
	printf("prev_month_cb!!  %d \n", cal->m_shown_month );
	cal->previous_month();
	cal->Invalidate();
}

void next_month_cb( void* mCalendar )
{
	Calendar* cal = (Calendar*) mCalendar;
	printf("next_month_cb!!  %d \n", cal->m_shown_month );
	cal->next_month();
	cal->Invalidate();
}


const int CALENDAR_DEFAULT_HEIGHT = 200;
const int CALENDAR_DEFAULT_WIDTH  = 250;

char days_of_week_1_letter[] = { 'S', 'M', 'T', 'W', 'T', 'F', 'S' };

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
Calendar::Calendar( int Width, int Height 					 ) 
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
	
	printf("Time=%d:%d:%d\n",      m_local_time.tm_hour, m_local_time.tm_min, m_local_time.tm_sec );
	printf("Date: %d %s :%d:%d\n", m_local_time.tm_mon, Months[m_local_time.tm_mon].c_str(),
								   m_local_time.tm_mday, m_local_time.tm_year+1900 );
	printf("%s (%d)\nBUTTONS: \n", days_of_week[m_local_time.tm_wday].c_str(), m_local_time.tm_wday );
	
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
	text_color = 0xFFFFFF00;
	Fill_l   ( text_color );
	Stroke_l ( 0xFFFFFF00 );		
	float x= left + m_prev.get_width() + 20.;
	float y= bottom + height - (text_size*1.25);
	Text(x, y, Months[m_shown_month].c_str(), SerifTypeface, text_size );

	// DRAW YEAR
	sprintf(date_str, "%4d", m_shown_year );	
	int text_width = TextWidth( date_str, SerifTypeface, text_size );
	TextEnd(m_next.get_left()-text_width, y, date_str, SerifTypeface, text_size );
	
	// DRAW DAYS of the WEEK : 
	x= left + 10.;
	y -= (text_size*2);
	date_str[0] = 0;
	for (int i=0; i<7; i++) 
	{
		sprintf(date, " %c ", days_of_week_1_letter[i] );
		strcat (date_str, date  );
	}
	printf("Date Str (1week) %6.2f %6.2f : %s \n", x, y, date_str );
	Text(x, y, date_str, SerifTypeface, text_size );

	// Calc number of days to back off for the first week.
	struct tm som;	
	som.tm_mon  = m_shown_month;
	som.tm_mday = 1;
	som.tm_year = m_shown_year;
	som.tm_hour = 1; som.tm_min = 0; som.tm_sec = 0;
	mktime( &som );
	printf("First of the %d month is on %s %d \n", som.tm_mon, days_of_week[som.tm_wday].c_str(), som.tm_wday );

	int wday    = som.tm_wday;
	som.tm_mday -= wday;	
	mktime( &som );
	printf("Cal starts with %d (%d) \n", som.tm_mday, som.tm_mon );
	// should now have the m_wday set!

	// DRAW NUMBERS : 
	x = left + 10.;
	y -=  (text_size*1.5);
	int day = som.tm_mday;
	for (int i=0; i<5; i++)
	{
		date_str[0] = 0;		// restart
		for (int j=0; j<7; j++, day++) {
			if (day>30) day = 1;
			sprintf(date, "%2d ", day);
			strcat (date_str, date);
		}
		printf("Date Str (1week) %6.2f %6.2f : %s \n", x,y, date_str );
		Text(x, y, date_str, SerifTypeface, text_size );
		y -= text_size*1.25;
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



