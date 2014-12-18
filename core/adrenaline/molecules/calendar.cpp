// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "adrenaline_windows.h"
#include "calendar.hpp"


Calendar::Calendar() 
{ 
Initialize(	) 
}
Calendar::Calendar( Rectangle* mRect ) 
{ 
Initialize(	) 
}
Calendar::Calendar( int Width, int Height 					 ) 
{ 
Initialize(	) 
}
Calendar::~Calendar() 
{ 
}

void Calendar::Initialize(	) 
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);	
	printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}
int	Calendar::show_date	( int mMonth, int mDay ) 
{ 
}	

int Calendar::onCreate	(	) 
{ 
}
int Calendar::draw 		(	) 
{ 
}
int Calendar::draw_simple_view(	) 
{ 
}
int Calendar::draw_1month_view(	) 
{ 
}
int Calendar::draw_1week_view (	) 
{ 
}
int	Calendar::set_on_click_listener( void (*callback)(void*), void* mOn_click_context ) 
{ 
}
int Calendar::onClick(int x, int y, bool mouse_is_down=true) 
{ 
}



