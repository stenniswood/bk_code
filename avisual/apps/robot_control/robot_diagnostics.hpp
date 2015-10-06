#ifndef _ROBOT_DIAGNOSTICS_PANEL_
#define _ROBOT_DIAGNOSTICS_PANEL_


//#include "adrenaline_windows.h"
#include "window.hpp"


class RobotDiagnosticsPanel : public Window
{
public:
	RobotDiagnosticsPanel();

	virtual void 	Initialize(	);
	int				calc_metrics();
	int				place_views();

	void			setup_headings();
	
	virtual int		onCreate	  (  );	// chance to load resources, call functions which use fonts
	virtual int   	draw		 		(				);

};




#endif

