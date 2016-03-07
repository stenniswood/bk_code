#ifndef _CALENDAR_SUMMARY_HPP_
#define _CALENDAR_SUMMARY_HPP_

#include <time.h>
#include "control.hpp"
#include "button.hpp"
#include "calendar_entry.hpp"

/* This window will show the activities for a day on the calendar.
	It sits beside the calendar and changes each time the user clicks a different day.
	
*/
class CalendarSummary : public Control 
{
public:
	CalendarSummary ( );
	CalendarSummary ( Rectangle* mRect		);
	CalendarSummary ( int Width, int Height );
	~CalendarSummary( );
	
	virtual void 	Initialize(	);
	
	int				show_date	( int mMonth, int mDay );	// other than the current


	virtual int   	onCreate			(	);	
	virtual int   	draw 				(	);	
	void   			draw_day_summary	(	);		
	void   			draw_entry_details 	( int mRow_index );			


	int				set_on_click_listener( void (*callback)(void*), void* mOn_click_context );
	virtual int		onClick		(int x, int y, bool mouse_is_down=true);
	void			place_views	( );
	
	int				m_shown_month;
	int				m_shown_day;
	int				m_shown_year;
	
	struct tm		m_local_time;	
	int				m_display_mode;
	
	float			m_left_margin;
	int				m_row_index;
	
	CalendarEntry 	ce;
	
};

 
#endif
