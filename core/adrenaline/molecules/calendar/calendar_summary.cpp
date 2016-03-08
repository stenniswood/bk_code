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
#include "calendar_entry.hpp"
#include "calendar_summary.hpp"
#include "bk_system_defs.h"




#define Debug 1

const float ROW_SPACING_MULTIPLIER = 1.25;

/* This should retrieve all entries from the SQL database for the selected day.

*/
CalendarSummary::CalendarSummary ( )
{ 
	Initialize();
}
CalendarSummary::CalendarSummary ( Rectangle* mRect		)
{
	Initialize(); 
}
CalendarSummary::CalendarSummary ( int Width, int Height )
{
	Initialize();
}
CalendarSummary::~CalendarSummary( )
{ 
}

void 	CalendarSummary::Initialize(	)
{ 
	Control::Initialize();
	
	//background_color = 0xFFFFFF00;
	background_color = 0xFFA0002C;	
}

/* The user wants to see the specified month and day */
int		CalendarSummary::show_date	( int mMonth, int mDay )
{ 
	ce.m_scheduled_time_bd.tm_mon = mMonth;
	ce.m_scheduled_time_bd.tm_mday = mDay;	
	ce.find_date(2);
	int number_entries = ce.get_number_results();
	dprintf("CalendarSummary::show_date() %d entries found for this date\n", number_entries );
}

int   	CalendarSummary::onCreate			(	)
{ 
	dprintf("CalendarSummary::onCreate()\n");
	int retval = Control::onCreate();
	m_left_margin = left+10;	
	return retval;
}

int   	CalendarSummary::draw 				(	)
{ 
	int retval = Control::draw();
	if (Visible)
		draw_day_summary();	
	return retval;	
}	

void 	CalendarSummary::draw_day_summary	(	)
{ 
	int row_count = 0;
	MYSQL_ROW result = ce.goto_first_row();		//mysql_fetch_row( ce.m_result );	
	while (result)
	{	
		ce.extract_result();
		draw_entry_details( row_count++ );
		result = ce.goto_next_row();
	}
}

/* For the last row fetched inside CalendarEntry ce class */
void  	CalendarSummary::draw_entry_details 	( int mRow_index )
{ 
	string text = ce.form_summary_string();

	Stroke_l   ( text_color );		// box color
	Fill_l     ( text_color );		// box color	
	float x = m_left_margin;
	float y = bottom+height - mRow_index * text_size * ROW_SPACING_MULTIPLIER;
	dprintf("<%4.1f, %4.1f> %s\n", x,y,text.c_str() ); 
	if (y>0)
		Text(x,y, text.c_str(), SerifTypeface,  text_size);	
}

int		CalendarSummary::set_on_click_listener( void (*callback)(void*), void* mOn_click_context )
{ 
}

int		CalendarSummary::onClick		(int x, int y, bool mouse_is_down)
{ 
	return Control::onClick(x,y,mouse_is_down);
}

void	CalendarSummary::place_views	( )
{ 
}
