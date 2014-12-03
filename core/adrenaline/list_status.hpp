
#ifndef _ADREN_NETWORK_VIEW_H_
#define _ADREN_NETWORK_VIEW_H_

#include "list.hpp"

/* This is a list view with a pane on the right showing
   another Control (usually textview)

*/


class StatusList : public ListControl
{
public:
	StatusList(  );

	int		populate_with_boards( );
	int		draw_board		( );
	int		draw_board_info	( );
	virtual int	draw  	( );

private:
	vector<Control*>	m_status_panes;

};




#endif
