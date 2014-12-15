#ifndef _TASK_BAR_
#define _TASK_BAR_

#include "control.hpp"
#include "window.hpp"


#include <list>
#include <string>
using namespace std;


/***************************************************************
This control puts some buttons/info on the sides of the screen.

This will hide or open
Later on maybe accept drag and drop items.
Number of controls should be limited by practice < 8
****************************************************************/
class TaskBar : public Window 
{
public:
	TaskBar ( );
	TaskBar ( int Left, int Right, int Top, int Bottom );
	~TaskBar( );

	virtual void	Initialize		 (   );	
	virtual int   	draw 			 (	 );
	void			calc_metrics	 (   );

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
