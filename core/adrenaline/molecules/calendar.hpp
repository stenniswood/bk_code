#ifndef _CALENDAR_HPP_
#define _CALENDAR_HPP_

#include <time.h>
#include "control.hpp"
#include "button.hpp"

		

class Calendar : public Control 
{
public:
	Calendar();
	Calendar( Rectangle* mRect );
	Calendar( int Width, int Height 					 );
	~Calendar();
	
	virtual void 	Initialize(	);
	
	int				show_date	( int mMonth, int mDay );	// other than the current
	void			previous_month();
	void			next_month();

	virtual int   	onCreate	(	);	
	virtual int   	draw 		(	);	
	int   			draw_simple_view(	);	
	int   			draw_1month_view(	);	
	int   			draw_1week_view (	);			
	int				draw_1month_view_small( );
		
	int				set_on_click_listener( void (*callback)(void*), void* mOn_click_context );
	virtual int		onClick		(int x, int y, bool mouse_is_down=true);
	void			place_views	( );
	
	int				m_shown_month;
	int				m_shown_year;
	Button			m_prev;
	Button			m_next;
	struct tm		m_local_time;	
	int				m_display_mode;
};

 
#endif
