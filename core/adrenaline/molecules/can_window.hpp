#ifndef _CAN_APP_H_
#define _CAN_APP_H_

#include "control.hpp"
#include "adren_network_view.hpp"
#include "can_msg_view.hpp"
#include "adrenaline_windows.h"

#define MENU_STATE_NORMAL 		1
#define MENU_STATE_GRAYED 		2
#define MENU_STATE_SELECTED 	3
#define MENU_STATE_CIRCLED 		4
#define MENU_STATE_EMPHASIZED 	5


class CANMessages : public Window
{
public:
	CANMessages();
	CANMessages( int Left, int Right, int Top, int Bottom );
	CANMessages( int Width, int Height 					 );
	~CANMessages();

	virtual void 	Initialize(	);
	int				calc_metrics();
	int				place_views();

	int				handle_incoming_msg	( struct sCAN* msg );
	int				setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms );
	int				setup_triggered_response_msg( struct sCAN* mTriggerMsg, struct sCAN* mResponse );	

	void			fill_phony_msgs();

	virtual int		onCreate	  (  );	// chance to load resources, call functions which use fonts
	
	int				get_hit_index		( int Mousex, int Mousey );
	virtual int		onClick				( int x, int y, bool mouse_is_down=true);
	virtual int   	draw		 		(				);

protected:
	//vector<stVertMenuInfo*>	m_entries;	
	NetworkView				m_board_view;		// List of all boards present
	CANMessageView			m_msg_view;
	//CANFilterView			m_filter_view;
	//CANMessageList		m_msg_list;			// All messages in database.

	Button					m_rescan;
	Button					m_view_graph;		// toggle between outline (msg flow) & graphing of data.

	int						m_width;
	bool					has_scroller;
};

 
#endif
