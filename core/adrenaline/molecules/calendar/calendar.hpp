#ifndef _CALENDAR_HPP_
#define _CALENDAR_HPP_

#include <time.h>
#include <vector>
#include "control.hpp"
#include "button.hpp"

using namespace std;

class Grid
{
public:
	Grid();
	Grid(int mLeft, int mRight, int mTop, int mBottom );	
	~Grid();

	void			set_boundary (int mLeft, int mRight, int mTop, int mBottom );		
	void			create(int mRows, int mCols);
	void			hit_test(int mx, int mY);	
	
	int 			m_hit_row;
	int				m_hit_col;
	vector<int>		m_column_start_x;
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
	int				find_date_clicked	 ( int x, int y );
	virtual int		onClick				 ( int x, int y, bool mouse_is_down=true);
	void			place_views			 ( );
	
	int				m_shown_month;
	int				m_shown_day;	
	int				m_shown_year;
	Button			m_prev;
	Button			m_next;
	struct tm		m_local_time;	
	int				m_display_mode;
	
	int				m_prev_month_end;
	int				m_this_month_end;
};

 
#endif
