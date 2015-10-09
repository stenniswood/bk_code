#ifndef _CALENDAR_HPP_
#define _CALENDAR_HPP_

#include <time.h>
#include "control.hpp"

		

class Calendar : public Control 
{
public:
	Calendar();
	Calendar( Rectangle* mRect );
	Calendar( int Width, int Height 					 );
	~Calendar();
	
	virtual void 	Initialize(	);
	
	int				show_date	( int mMonth, int mDay );	// other than the current

	virtual int   	onCreate	(	);	
	virtual int   	draw 		(	);	
	int   			draw_simple_view(	);	
	int   			draw_1month_view(	);	
	int   			draw_1week_view (	);			
	int				draw_1month_view_small(	);
		
	int				set_on_click_listener( void (*callback)(void*), void* mOn_click_context );
	virtual int		onClick(int x, int y, bool mouse_is_down=true);

	struct tm	m_local_time;	

};

 
#endif
