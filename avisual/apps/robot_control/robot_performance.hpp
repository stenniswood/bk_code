#ifndef _ROBOT_PERFORMANCE_PANEL_
#define _ROBOT_PERFORMANCE_PANEL_


//#include "adrenaline_windows.h"
#include "window.hpp"
#include "adrenaline_graphs.h"


class RobotPerformancePanel : public Window
{
public:
	RobotPerformancePanel();

	virtual void 	Initialize(	);
	void			calc_metrics();
	int				place_views();

	void			setup_headings();
	
	virtual int		onCreate	  (  );	// chance to load resources, call functions which use fonts
	virtual int   	draw		 		(				);


	Histogram		m_hg_1;
	Histogram		m_hg_2;
	
};




#endif

