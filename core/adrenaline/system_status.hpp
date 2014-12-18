#ifndef _SYSTEM_STATUS_
#define _SYSTEM_STATUS_

#include "control.hpp"
#include "window.hpp"


#include <list>
#include <string>
using namespace std;

#define DEFAULT_VSCROLL_WIDTH  10
#define DEFAULT_HSCROLL_HEIGHT 10 

#define MAX_CONTROLS 6

#define SYSTEM_ALIGN_TOP     0x01
#define SYSTEM_ALIGN_CENTER  0x02
#define SYSTEM_ALIGN_BOTTOM  0x04

/***************************************************************
This control puts some buttons/info on the sides of the screen.

This will hide or open
Later on maybe accept drag and drop items.
Number of controls should be limited by practice < 8
****************************************************************/
class SystemStatusBar : public Control
{
public:
	SystemStatusBar ( );
	SystemStatusBar ( int Left, int Right, int Top, int Bottom );
	~SystemStatusBar( );

	virtual void	Initialize		 (   );	
	virtual int   	draw 			 (	 );
	void			calc_metrics	 (   );
	void			onPlace			 (   );
	virtual int		onCreate	  		(  );	// chance to load resources, 	
	
	void			set_text		 ( char* mText );
	int				add_control		 ( Control* mControl, char* mText=NULL );
	void			hide			 ( bool mHide = true);
	void			auto_hide		 ( bool mAutoHide = true);
	void			set_alignment	 ( byte mAlignment = SYSTEM_ALIGN_CENTER );

	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	
protected:
	Button*			m_power_button;		// center of display
	Button*			m_calendar;			// center of display	
	
	TextView		m_text_view;		// center of display
	string			m_text;	

	int				num_tabs;
	list<Control*>	controls;
	
private:
	byte		Alignment;
	bool		isHidden;
	bool		AutoHideEnabled;	
};

 
#endif
