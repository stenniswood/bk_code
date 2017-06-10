#ifndef _CALENDAR_HPP_
#define _CALENDAR_HPP_

#include <time.h>
#include <vector>
#include "control.hpp"
#include "button.hpp"
#include "calendar_summary.hpp"

using namespace std;

class Grid
{
public:
	Grid();
	Grid(int mLeft, int mRight, int mTop, int mBottom );	
	~Grid();

	void			set_boundary ( int mLeft, int mRight, int mTop, int mBottom );
	void			create		 ( int mRows, int mCols );
	void			hit_test	 ( int mx, int mY 		);
	void			set_top_down ( bool mTopDown = true);
	
	int				get_left	 ( int mColumn );
	int				get_bottom	 ( int mRow    );
	
	void			draw		 ( );

	bool			m_top_down_coords;
	int 			m_hit_row;
	int				m_hit_col;
	long int		spare1[5];	
	vector<int>		m_column_start_x;
	long int		spare2[5];	
	vector<int>		m_row_bottom_y;
	int		left;
	int 	bottom;
	int 	width;
	int		height;	
};

class Calendar : public Control 
{
public:
	Calendar();
	Calendar( Rectangle* mRect );
	Calendar( int Width, int Height	 );
	~Calendar();
	
	virtual void 	Initialize(	);
	void 			get_current_time(	);
	
	void			show_date	( int mMonth, int mDay );	// other than the current
	void			previous_month();
	void			next_month();

	virtual int   	onCreate	(	);	
	virtual int   	draw 		(	);	
	int   			draw_simple_view(	);	
	int   			draw_1month_view(	);	
	int   			draw_1week_view (	);			
	int				draw_1month_view_small	( );
	void			compute_date_info		( );

	int				set_on_click_listener( void (*callback)(void*), void* mOn_click_context );
	void			find_date_clicked	 ( );
	virtual int		onClick				 ( int x, int y, bool mouse_is_down=true);
	void			place_views			 ( );
	void			fill_in_days_shown_array(int mStartDay /* prev month */);

	int				m_shown_month;
	int				m_shown_day;	
	int				m_shown_year;
	int				m_shown_weekday;
	
	Button			m_prev;
	Button			m_next;
	Button			m_close;		// or maybe a swipe down.

	CalendarSummary* summary;
//	long int		spare1[5];
		
	int				m_days_shown[6*7];		// includes few days of previous & next month.
	
	struct tm		m_local_time;	
	int				m_display_mode;

	int				m_prev_month_end;		// m_lastday_prev_month
	int				m_this_month_end;
	int				m_next_month_end;		// m_lastday_next_month
	
//	long int		spare3[5];
	
	Grid			m_grid;
};

 
#endif
