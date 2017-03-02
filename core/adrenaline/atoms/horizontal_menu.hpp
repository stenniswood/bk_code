#ifndef _HORIZ_MENU_H_
#define _HORIZ_MENU_H_

#include "control.hpp"
#include <vector>


class VerticalMenu;
class Application;
class Menu;

enum eMenuState
{
	MENU_STATE_NORMAL,
	MENU_STATE_GRAYED,
	MENU_STATE_SELECTED,
	MENU_STATE_CIRCLED,
	MENU_STATE_EMPHASIZED
};

enum eEntryType {
	REGULAR_ENTRY,
	SUB_MENU_ENTRY
};

class MenuEntry 
{
public:
	MenuEntry();
	~MenuEntry();
	
	virtual enum eEntryType	getEntryType();
	virtual Menu*		getSubMenu()	{ return NULL; };
	
	void		set_text  ( const char* mText );
	void		calc_width( const char* mText );
	int			draw();

	float 			LineHeight;
	char			text[50];
	char			alt_key[4];				// to activate from keyboard once menu is already active.
	char			short_cut_key[4];		// to directly activate from keyboard 
	float			width;
	float			Y;
	float 			sx;				// computed in calc_metrics()
	float 			ex;				// computed in calc_metrics()	
	enum eMenuState	state;
	int				(*callback)(void*, int);		// called when the item is selected.
	float			text_size;
};


class CascadeMenuEntry : public MenuEntry
{
public:
	CascadeMenuEntry();
	~CascadeMenuEntry();
	
	virtual enum eEntryType	getEntryType();
	virtual Menu*		getSubMenu()	{ return submenu; };
		
	void		calc_width( const char* mText );
	int			draw();
	Menu*		submenu;
};

class Menu : public Control
{
public:
	Menu ();
	Menu ( int Left, int Right, int Top, int Bottom );
	Menu ( int Width, int Height 					);
	~Menu();

	virtual void 	Initialize	(	);	
	virtual int		calc_metrics(	);	
	int				add_sub_menu( const char* mMenuText,  Menu* vm );
	int				add_entry	( MenuEntry mEntry 	 		  );
	int				attach_at	( float x, float y );	
	void			clear_all	( 								  );
	Menu*			set_parent	( Menu* mMenu  );
	int				select				( int mSelected 		);
	bool			is_selection_valid	( 	);
		
	Menu*						m_parent;					
	std::vector<MenuEntry>		m_entries;	
	size_t						m_selection;
	Application*				m_application;   // associated with the menu.
	long						m_selected_color;
};


class HorizontalMenu : public Menu
{
public:
	HorizontalMenu ();
	HorizontalMenu ( int Left, int Right, int Top, int Bottom );
	HorizontalMenu ( int Width, int Height 					 );
	~HorizontalMenu();

	virtual void 	Initialize			(	);
	virtual int		calc_metrics		(	);
	int				get_id				(   );
	int				add_entry_text		( const char* mMenuText );
	virtual int   	draw		 		(	);

	int				 get_hit_index		( int Mousex, int Mousey	);
	virtual Control* HitTest			( int x, int y 				);
	virtual int		 onClick			( int x, int y, bool mouse_is_down=true );

protected:
	int						m_text_size;
	int						m_height;
	bool					m_has_scroller;
	bool					m_has_add_button;
	float					m_menu_padding;
};

 
#endif

//	int				set_on_click_listener( void (void*) );
//	a extract menu items from a .xml menu resource
//	int 		  	load_from_resource	(	);
//	int				add_callback( int  mIndex, int (*callback)(void*, int)=NULL );