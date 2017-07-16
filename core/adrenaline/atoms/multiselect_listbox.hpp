#ifndef _MULTISELECT_LISTBOX_
#define _MULTISELECT_LISTBOX_

#include "control.hpp"


class MultiListBox : public ListBox 
{
public:
	MultiListBox();
	MultiListBox( int Left, int Right, int Top, int Bottom );
	MultiListBox( int Width, int Height 				   );
	~MultiListBox();
	
	virtual void 	Initialize(	);

	void			select		( int mIndex );
	int				deselect	( int mIndex );

	virtual int   	draw 		(	);	
//	int				set_on_click_listener( void (void*) );
	
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	
private:
	std::vector<int>		m_selections;
	
};

 
#endif
