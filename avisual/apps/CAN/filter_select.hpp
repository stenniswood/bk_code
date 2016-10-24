#ifndef _FILTER_SELECTOR_H_
#define _FILTER_SELECTOR_H_



#include "window.hpp"


class FilterSelector : public Window
{
public:
	FilterSelector();
	
	virtual void 	Initialize(	);
	int				calc_metrics();
	void			place_views();

	
};



#endif
