
#ifndef _ADREN_NETWORK_VIEW_H_
#define _ADREN_NETWORK_VIEW_H_

#include "list.hpp"

/* This is a list view with a pane on the right showing
   another Control (usually textview)
					|  
					|  
		/-----------|---\-------\
		|		|		|		|
		|		|		|		|		
*/


class VerticalLineGroup : public Control
{
public:
	VerticalLineGroup(  );
	
	int		add_source				( Control* mItem 		);
	int		add_source_empty		( float mx, float my 	);		

	int		place_destination		( float mx, float my 	);
	int		place_destination_center( 						);
	int		place_destination_item	( Control* mItem 		);
		
	virtual int	draw  	( );

private:
	vector<Control*>	m_items;
	
};

#endif
