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



const int CALENDAR_DEFAULT_HEIGHT = 100;
const int CALENDAR_DEFAULT_WIDTH  = 100;


string Months[] = { 
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
	time_t t = time(NULL);
	m_local_time =  *localtime(&t);	
	printf("now: %d-%d-%d %d:%d:%d\n", 
			 m_local_time.tm_year+1900, m_local_time.tm_mon+1,
			 m_local_time.tm_mday, 		m_local_time.tm_hour, 
			 m_local_time.tm_min, 		m_local_time.tm_sec);

	set_width_height( CALENDAR_DEFAULT_HEIGHT, CALENDAR_DEFAULT_WIDTH );

}
int	Calendar::show_date	( int mMonth, int mDay ) 
{ 
}	

int Calendar::onCreate	(	) 
{ 
	Control::onCreate();
}
int Calendar::draw 		(	) 
{
	Control::draw(); 
	draw_1month_view_small();
}
int Calendar::draw_simple_view(	) 
{ 
}
int Calendar::draw_1month_view_small(	) 
{

 /* 
	Text(  Month );
	for (	
*/}
int Calendar::draw_1week_view (	) 
{ 
}
int	Calendar::set_on_click_listener( void (*callback)(void*), void* mOn_click_context ) 
{ 
}
int Calendar::onClick(int x, int y, bool mouse_is_down=true) 
{ 
	Control::onClick(x,y,mouse_is_down); 
}



