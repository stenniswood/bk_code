#ifndef _CAN_APP_H_
#define _CAN_APP_H_

#include "control.hpp"


#define MENU_STATE_NORMAL 		1
#define MENU_STATE_GRAYED 		2
#define MENU_STATE_SELECTED 	3
#define MENU_STATE_CIRCLED 		4
#define MENU_STATE_EMPHASIZED 	5


class CANApp : public Window
{
public:
	CANApp();
	CANApp( int Left, int Right, int Top, int Bottom );
	CANApp( int Width, int Height 					 );
	~CANApp();

	virtual void 	Initialize(	);
	int				calc_metrics();
	int				place_views();
	 
	int				handle_incoming_msg	( struct sCAN* msg );
	int				setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms );
	int				setup_triggered_response_msg( struct sCAN* mTriggerMsg, struct sCAN* mResponse );	
	
	virtual int   	draw		 		(				);

	int				get_hit_index		( int Mousex, int Mousey );
//	int				set_on_click_listener( void (void*) );	
	virtual int		onClick(int x, int y, bool mouse_is_down=true);

protected:
	//vector<stVertMenuInfo*>	m_entries;	
	CANMessageView			m_msg_view;
	CANFilterView			m_filter_view;
	CANAdrenalineView		m_board_view;		// List of all boards present
	CANMessageList			m_msg_list;			// All messages in database.

	
	int						m_width;
	bool					has_scroller;
};

 
#endif
