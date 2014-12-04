
#ifndef _ADREN_NETWORK_VIEW_H_
#define _ADREN_NETWORK_VIEW_H_

#include "list.hpp"

/* This is a list view with a pane on the right showing
   another Control (usually textview)
   					E
					|  
					|  
		/-----------|---\-------\
		|		|		|		|
		|		|		|		|	
		S  		S  		S  		S	
*/

struct point {
	float sx;
	float sy;
	char  point_type;		// 'S' for source point.  'E' for end point
};

class VerticalLineGroup : public Control
{
public:
	VerticalLineGroup(  );
	
	int		add_source				( Control* mItem 		);
	int		add_source_point		( float mx, float my 	);		

	int		place_destination		( float mx, float my 	);
	int		place_destination_center( 						);
	int		place_destination_item	( Control* mItem 		);
		
	virtual int	draw  	( );

private:
	vector<Control*>	m_items;
	vector<point*>		m_source_points;
	vector<point*>		m_destination_points;
		
	long int 			m_line_color;
	
};

#endif
