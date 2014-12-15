#ifndef _CAN_MSG_VIEW_H_
#define _CAN_MSG_VIEW_H_

#include "tabular_listbox.hpp"

/*
	This is a tabular list view of the CAN messages.
*/

class CANMessageView : public TabularListBox
{
public:
	CANMessageView ();
	CANMessageView ( int Left, int Right, int Top, int Bottom );
	CANMessageView ( int Width, int Height 					  );
	~CANMessageView();

	virtual void 	Initialize  (	);
	int				calc_metrics(	);
 
 	int				setup_headers		( );
	int				add_message 		( struct sCAN* msg );

	int 			order_by_time		( );
	int 			order_by_msg_id		( );

	int   			draw_entry 			( int mState	);	
	virtual int   	draw		 		(				);

	int				get_hit_index		( int Mousex, int Mousey );
	virtual int		onClick(int x, int y, bool mouse_is_down=true);

protected:
	std::vector<sCAN*>	m_msgs;
	int				m_width;
	bool			has_scroller;	
};

 
#endif
