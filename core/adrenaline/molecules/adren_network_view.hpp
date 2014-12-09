#ifndef _ADREN_NETWORK_VIEW_H_
#define _ADREN_NETWORK_VIEW_H_

#include "icon.hpp"

extern char const* e_card_names[];


class NetworkView : public Control
{
public:
	NetworkView(  );

	int		populate_with_boards(	);
	
	int	draw_board		( );
	int	draw_board_info	( );
	virtual int	draw  	( );

private:


};


#endif

