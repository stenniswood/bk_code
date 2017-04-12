#ifndef _ANALOG_VIEW_H_
#define _ANALOG_VIEW_H_

#include <vector>
#include "control.hpp"
#include "adren_network_view.hpp"
#include "can_msg_view.hpp"
#include "leveler.hpp"

using namespace std;

#define MENU_STATE_NORMAL 		1
#define MENU_STATE_GRAYED 		2
#define MENU_STATE_SELECTED 	3
#define MENU_STATE_CIRCLED 		4
#define MENU_STATE_EMPHASIZED 	5


class AnalogView : public Window
{
public:
	AnalogView();
	AnalogView( int Left, int Right, int Top, int Bottom );
	AnalogView( int Width, int Height 					 );
	~AnalogView();

	virtual void 	Initialize(	);
	int				count_rows_showing();
	int				calc_metrics();
	void			place_views();
	

	void			configure_screen();	// based on which checkboxes enabled.
	void			create_group ( int mChip 	);
	void			create_groups( );
	
	void			show_group 	 ( int mGroup, bool mVisible=true  	);
	void			place_group	 ( int mGroup, float mBottom );


	int				handle_incoming_msg	( struct sCAN* msg );
	int				setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms );
	int				setup_triggered_response_msg( struct sCAN* mTriggerMsg, struct sCAN* mResponse );	

	void			fill_phony_msgs();

	virtual int		onCreate	  (  );	// chance to load resources, call functions which use fonts
	
	int				get_hit_index		( int Mousex, int Mousey );
	virtual int		onClick				( int x, int y, bool mouse_is_down=true);
	virtual int   	draw		 		(				);

protected:
	vector<Leveler*>			m_indicators;
	int						m_group_start_index[4];
	int						m_group_end_index[4];
	
	int						m_col_start;
	int						m_row_start;
	int						m_col_increment;
	int						m_row_increment;
	
	CheckBox* 				m_chip_enable1;
	CheckBox* 				m_chip_enable2;
	CheckBox* 				m_chip_enable3;
	CheckBox* 				m_chip_enable4;
	
	Button					m_rescan;
	Button					m_view_graph;		// toggle between outline (msg flow) & graphing of data.

	int						m_width;
	bool					has_scroller;
};


#endif
