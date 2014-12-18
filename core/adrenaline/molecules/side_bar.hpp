#ifndef _SIDE_BAR_CONTROL_
#define _SIDE_BAR_CONTROL_

#include "control.hpp"
#include "window.hpp"


#include <list>
#include <string>
using namespace std;

#define DEFAULT_VSCROLL_WIDTH  10
#define DEFAULT_HSCROLL_HEIGHT 10 

#define MAX_CONTROLS 6

#define SIDEBAR_ALIGN_LEFT   0x02 
#define SIDEBAR_ALIGN_CENTER 0x01 
#define SIDEBAR_ALIGN_RIGHT  0x04 

const int ICON_WIDTH     = 100;
const int EXPANDED_WIDTH = 200;

/***************************************************************
This control puts some buttons/info on the sides of the screen.

This will hide or open
Later on maybe accept drag and drop items.
Number of controls should be limited by practice < 8
****************************************************************/
class SideBar : public Control
{
public:
	SideBar ( );
	SideBar ( int Left, int Right, int Top, int Bottom );
	~SideBar( );

	virtual void	Initialize		 (   );	
	virtual int   	draw 			 (	 );
	void			calc_metrics	 (   );
	void			onPlace			 (   );
	
	void			clear			 ( bool mDelete = true );
	void			toggle_state	 (   );
	virtual void 	load_resources	 ( 	 );
	float			get_bottom	 	 ( int mIndex   );
	float			get_short_width	 (   );	// for minimized view (ie. just icons)
	float			get_expanded_width	 ( );	// for minimized view (ie. just icons)
	
	int				add_control		 ( Control* mControl, char* mText=NULL );
	void			hide			 ( bool mHide = true);
	void			auto_hide		 ( bool mAutoHide = true);
	void			set_alignment	 ( byte mAlignment = SIDEBAR_ALIGN_CENTER );

	virtual int		onCreate	  		(  ) {};
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	
	
protected:
	int				m_state;
	int				m_num_tabs;
	list<string>	m_paired_text;
	float			m_bottom_margin;	
	
	byte			Alignment;
	bool			isHidden;
	bool			AutoHideEnabled;
	
};

 
#endif
