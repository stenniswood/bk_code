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
#include "calendar_summary.hpp"

#define DEBUG 1

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
int		CalendarSummary::show_date	( int mMonth, int mDay )
{ 
}	
void	CalendarSummary::previous_month()
{ 
}
void	CalendarSummary::next_month()
{ 
}
int   	CalendarSummary::onCreate			(	)
{ 
	printf("CalendarSummary::onCreate\n");
	int retval = Control::onCreate();
	printf("CalendarSummary::onCreate\n");	
	return retval;
}	
int   	CalendarSummary::draw 				(	)
{ 
	return Control::draw();
}	
int   	CalendarSummary::draw_day_summary	(	)
{ 

}		
int   	CalendarSummary::draw_day_details 	(	)
{ 
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
