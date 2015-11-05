#ifndef _ROBOT_CONTROL_PANEL_
#define _ROBOT_CONTROL_PANEL_


//#include "adrenaline_windows.h"
#include "window.hpp"


class RobotPanel : public Window
{
public:
	RobotPanel();

	virtual void 	Initialize	( );
	int				calc_metrics( );
	int				place_views	( );
	virtual int		onCreate	( );	// chance to load resources, call functions which use fonts
	
	int				pack_enables(char* mData);		// return is length
		
//	void			file_new();	 
//	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	virtual int   	draw		 		(				);

};

//void init_robot_panel();


#endif

