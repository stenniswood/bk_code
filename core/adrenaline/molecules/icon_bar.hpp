#ifndef _ICON_BAR_HPP_
#define _ICON_BAR_HPP_


#include "button.hpp"
#include "control.hpp"


/*
An icon bar:
	Icon names added programatically or - 
		specifying a resource group name.

*/

class IconBar : public Control
{
public: 
	IconBar();
	virtual int onPlace(  );
	
	void			wrap_to_icons    (          );
	void			reserve_num_icons( int mNum );
	
	
	// Buttons, or RadioButtons
	vector<Button*>	m_buttons;
	
	bool	m_horizontal_buttons;
};


#endif
