#ifndef _GYRO_VIEW_H_
#define _GYRO_VIEW_H_

#include "control.hpp"
#include "button.hpp"
#include "window.hpp"

#include "../core/adrenaline/graphs/line_graph.hpp"
#include "../core/adrenaline/graphs/dataset.hpp"
#include "../core/adrenaline/graphs/histogram.hpp"
//#include "adrenaline_windows.h"
//#include "adrenaline_graphs.h"


/*#define MENU_STATE_NORMAL 		1
#define MENU_STATE_GRAYED 		2
#define MENU_STATE_SELECTED 	3
#define MENU_STATE_CIRCLED 		4
#define MENU_STATE_EMPHASIZED 	5
*/

struct stValueInfo {
	int	 instance;
	int  id;
	int  start_byte;
	int	 num_bytes;
};

class GyroOverview : public Window
{
public:
	GyroOverview ( );
	GyroOverview ( int Left, int Right, int Top, int Bottom );
	GyroOverview ( int Width, int Height 					);
	~GyroOverview( );

	virtual void 	Initialize  ( );
	int				calc_metrics( );
	virtual void	place_views ( );

	int				handle_generic_msg ( struct sCAN* msg );
	int				handle_incoming_msg	( struct sCAN* msg );
	int				setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms );
	int				setup_triggered_response_msg( struct sCAN* mTriggerMsg, struct sCAN* mResponse );	
	void			register_with_display_manager();

	void			fill_phony_msgs();

	virtual int		onCreate	  (  );	// chance to load resources, call functions which use fonts
	
	//int				get_hit_index		( int Mousex, int Mousey );
	virtual int		onClick				( int x, int y, bool mouse_is_down=true);
	virtual int   	draw		 		(				);

	struct stValueInfo		m_match_data[3];
	
protected:
	
	DataSet* 	accel [3];
	DataSet* 	gyro  [3];
	DataSet* 	magnet[3];

	Histogram* 	hg ;		
	LineGraph* 	x_axis;
	LineGraph* 	y_axis;
	LineGraph* 	z_axis;
	
	Button					m_restart;
	Button					m_value1;
	Button					m_value2;
	Button					m_value3;
	
	int						m_width;
	bool					has_scroller;
};


#endif
