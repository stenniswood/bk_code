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

#define SIDEBAR_ALIGN_TOP     0x01
#define SIDEBAR_ALIGN_CENTER  0x02
#define SIDEBAR_ALIGN_BOTTOM  0x04

const int ICON_WIDTH     =100;
const int EXPANDED_WIDTH =300;

/***************************************************************
This control puts some buttons/info on the sides of the screen.

This will hide or open
Later on maybe accept drag and drop items.
Number of controls should be limited by practice < 8
****************************************************************/
class SideBar : public Window 
{
public:
	SideBar ( );
	SideBar ( int Left, int Right, int Top, int Bottom );
	~SideBar( );

	virtual void	Initialize		 (   );	
	virtual int   	draw 			 (	 );
	void			calc_metrics	 (   );

	float			get_short_width	 ( );	// for minimized view (ie. just icons)
	float			get_expanded_width	 ( );	// for minimized view (ie. just icons)
	
	int				add_control		 ( Control* mControl, char* mText=NULL );
	void			hide			 ( bool mHide = true);
	void			auto_hide		 ( bool mAutoHide = true);
	void			set_alignment	 ( byte mAlignment = SIDEBAR_ALIGN_CENTER );

	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	
	
protected:

	int				num_tabs;
	list<Control*>	controls;
	list<string>	paired_text;

private:
	byte		Alignment;
	bool		isHidden;
	bool		AutoHideEnabled;	
};

 
#endif
