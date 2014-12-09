#ifndef _TAB_CONTROL_
#define _TAB_CONTROL_

#include "control.hpp"


struct stTabInfo {
	char*	text;
	float	width;
	Control*	associated_control;		// this could be the main page drawn below the tab control. 	
};

#define DEFAULT_VSCROLL_WIDTH  10
#define DEFAULT_HSCROLL_HEIGHT 10 

/************************************************************
This tab control implements a standard icon & text.
perhaps should handle a generic Control for each tab.
of which each one could be a window with icon & text 
as children.  But we prefer to simplify for the programmer,
so   
This offers is readily builtin.
************************************************************/


class TabControl : public Control
{
public:
	TabControl();
	TabControl( int Left, int Right, int Top, int Bottom );
	TabControl( int Width, int Height 					 );
	~TabControl();
	
	virtual void 	Initialize(	);
	int				calc_metrics();
	
	int				add_entry			( stTabInfo mEntry, Control* mAssociatedPage );
	int				set_state			( bool mSelected );
	int				enable_add_button	( bool mEnable 	 );
	void			enable_scroll_buttons( bool mEnable = true );

	int			   	draw_curve			(	);
	int			   	draw_one_tab		(	);
	int			   	draw_add_tab 		(	);
	int 		  	draw_scroll_buttons	(	);
	virtual int   	draw 				(	);

	int				get_selected	 (	 );				// return currently selected tab.
	int				select			 ( int mTabIndex );
	
	int				get_hit_index		( int Mousex, int Mousey );
	virtual int		onClick				( int x, int y, bool mouse_is_down=true );
//	int				set_on_click_listener( void (void*) );

protected:
	vector<stTabInfo*>		m_tabs;
	int						m_selection;
	
	int						m_height;
	bool					m_has_scroller;
	bool					m_has_add_button;
};

 
#endif
