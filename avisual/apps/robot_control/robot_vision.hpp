#ifndef _ROBOT_VISION_PANEL_
#define _ROBOT_VISION_PANEL_
/* Show live analysis of vision system. */


#include "window.hpp"


class RobotVisionPanel : public Window
{
public:
	RobotVisionPanel();

	virtual void 	Initialize(	);
	void			calc_metrics();
	int				place_views();

	void			setup_headings();
	
	virtual int		onCreate	  (  );	// chance to load resources, call functions which use fonts
	virtual int   	draw		 		(				);

	
};




#endif

