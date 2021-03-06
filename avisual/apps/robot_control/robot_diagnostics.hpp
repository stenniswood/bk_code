#ifndef _ROBOT_DIAGNOSTICS_PANEL_
#define _ROBOT_DIAGNOSTICS_PANEL_


//#include "adrenaline_windows.h"
#include "window.hpp"


class RobotDiagnosticsPanel : public Window
{
public:
	RobotDiagnosticsPanel();
	virtual ~RobotDiagnosticsPanel();

	virtual void 	Initialize(	);
	void			calc_metrics();
	void			place_views();

	void			setup_headings();
	
	virtual int		onCreate	  (  );	// chance to load resources, call functions which use fonts
	virtual int   	draw		 (	);

};




#endif

