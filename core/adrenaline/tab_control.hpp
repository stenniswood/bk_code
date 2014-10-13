#ifndef _TAB_CONTROL_
#define _TAB_CONTROL_

#include "control.hpp"

#include <list>
#include <string>
using namespace std;


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
	~TabControl();

	virtual void	Initialize		 (   );
	virtual int   	draw 			 (	 );
	void			calc_metrics	 (   );

	int				get_selected	 (	 );				// return currently selected tab.
	int				select			 ( int mTabIndex );
	int				add_text_tab	 (   );
	int				add_icon_text_tab(   );

protected:
	void			enable_scroll_buttons( bool mEnable = true );
	int				num_tabs;
	list<Control*>	texts;

private:

};

 
#endif
