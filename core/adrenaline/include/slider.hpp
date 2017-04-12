//#ifndef _SLIDER_H_
//#define _SLIDER_H_

#include "control.hpp"

/*
	This dialog box will allow someone to specify which CAN msg, and bytes
	should be used to extract the 8 or 16 byte value for graphing.
	ie it indicates first of all the 
			CAN message ID, 
			CAN instance, 
			Size of the value (8 or 16, 32)
			byte index to start at (within the CAN msg)
*/
class SliderControl : public Control
{
public:
	SliderControl();	
	~SliderControl();
	//SliderControl(float Max, float Min);

	virtual void 	Initialize(	);
	int				calc_metrics();

	virtual int		onKey	( char mKey );
	// set level percent actually moves the marker.
	// want to expand this to set max, min, and level.
	// so we can show any unit, not just percents!
	
	int   set_level			(float level		 );
	void  set_level_percent	(float percent		 );
	void  set_max			(float max 			 ) { yMax 	= max; };
	void  set_min			(float min 			 ) { yMin 	= min; };
	
	float	get_level_pixel		(	);
	void 	draw_horizontal_tics(  );
	void 	draw_vertical_line	(	);
	void 	draw_y_label		(	);
	void 	draw_x_label		(	);
	void 	draw_l_title		(	);
	void	draw_marker			(	);

	virtual int 	draw		(	);

private:
	float 	yMax, yMin;
	float	Level;		// Must be somewhere between yMin & yMax
	
	char* 	xAxisLabel;
	char* 	yAxisLabel;
	char* 	title;
};



//#endif
