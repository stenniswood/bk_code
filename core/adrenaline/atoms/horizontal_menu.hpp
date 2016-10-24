#ifndef _HORIZ_MENU_H_
#define _HORIZ_MENU_H_

#include "control.hpp"
#include <vector>

//forward:
class VerticalMenu;
class Application;

struct stHorizMenuInfo {
	char*	text;
	char	alt_key;		// to activate from keyboard.
	float	width;			// computed in calc_metrics()
	float 	sx;				// computed in calc_metrics()
	float 	ex;				// computed in calc_metrics()
	VerticalMenu*	menu;
};

class HorizontalMenu : public Control
{
public:
	HorizontalMenu ();
	HorizontalMenu ( int Left, int Right, int Top, int Bottom );
	HorizontalMenu ( int Width, int Height 					 );
	~HorizontalMenu();

	virtual void 	Initialize			(	);
	int				calc_metrics		(	);
	int				get_id				(   );

	int				add_entry_text		( const char* mMenuText 				  );
	int				add_sub_menu		( const char* mMenuText, VerticalMenu* vm );
	int				add_entry			( stHorizMenuInfo* mEntry 			);
	void			clear_all			( 									);
	int				select				( int mSelected 					);

	virtual int   	draw		 		(	);
	bool			is_selection_valid	( 	);

	int				 get_hit_index		( int Mousex, int Mousey );
	virtual Control* HitTest			( int x, int y 	);
	virtual int		 onClick			( int x, int y, bool mouse_is_down=true );

//	int				set_on_click_listener( void (void*) );
//	a extract menu items from a .xml menu resource
//	int 		  	load_from_resource	(	);

	std::vector<stHorizMenuInfo>		m_entries;
	size_t								m_selection;
	Application*						m_application;   // associated with the menu.
	
protected:
	long					m_selected_color;
	int						m_text_size;
	int						m_height;

	bool					m_has_scroller;
	bool					m_has_add_button;

	float					m_menu_padding;
};

 
#endif
