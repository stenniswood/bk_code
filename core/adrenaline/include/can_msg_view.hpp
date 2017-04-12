#ifndef _CAN_MSG_VIEW_H_
#define _CAN_MSG_VIEW_H_

#include "tabular_listbox.hpp"

/*
	This is a tabular list view of the CAN messages.
*/

const int CAN_MESSAGE_BUFFER_SIZE = 5000 * 60 * 30;		// approximately 30 minutes worth.


const int CAN_MSG_MODE_HISTORY  = 1;
const int CAN_MSG_MODE_MESSAGES = 2;

class CANMessageView : public TabularListBox
{
public:
	CANMessageView ();
	CANMessageView ( int Left, int Right, int Top, int Bottom );
	CANMessageView ( int Width, int Height 					  );
	~CANMessageView();

	virtual void 	Initialize  		(	);
	int				calc_metrics		(	);
 
 	void			setup_headers		( );
	void			add_message 		( struct sCAN* msg );
	int				handle_incoming_msg	( struct sCAN* msg );

	void 			order_by_time		( );
	void 			order_by_msg_id		( );

	void   			draw_entry 			( int mState	);	
	virtual int   	draw		 		(				);
	vector<string>* convert_to_string	( struct sCAN* msg );

	void			save				( string mFilename );		// Save all messages!
	int				get_hit_index		( int Mousex, int Mousey );
	virtual int		onClick(int x, int y, bool mouse_is_down=true);

protected:
	std::vector<struct sCAN*>	m_msgs;
	int				m_width;
	bool			has_scroller;	
	int				m_mode;
};

 
#endif
