#ifndef _VERTICAL_MENU_H_
#define _VERTICAL_MENU_H_

#include <string>
//#include <string.h>
#include <vector>
#include "control.hpp"
using namespace std;
#include "listbox.hpp"

#include "horizontal_menu.hpp"


class VerticalMenu;

struct stVertMenuInfo {
	char	text[50];
	char	alt_key[4];				// to activate from keyboard once menu is already active.
	char	short_cut_key[4];		// to directly activate from keyboard 
	float	width;
	int		state;
	int		(*callback)(void*, int);		// called when the item is selected.
	VerticalMenu*	menu;
};

#define MENU_STATE_NORMAL 		1
#define MENU_STATE_GRAYED 		2
#define MENU_STATE_SELECTED 	3
#define MENU_STATE_CIRCLED 		4
#define MENU_STATE_EMPHASIZED 	5


class VerticalMenu : public ListBox
{
public:
	VerticalMenu ( );
	VerticalMenu ( int Left, int Right, int Top, int Bottom );
	VerticalMenu ( int Width, int Height 				   );
	~VerticalMenu( );

	virtual void 	Initialize			(	);
	int				calc_metrics		(	);
	int  			create_std_file_menu(	);

	int				add_simple_command	( char* mText, char* mShortcut=NULL );
	int				add_sub_menu		( char* mText, VerticalMenu* mSubMenu=NULL );	
	int				add_callback		( int  mIndex, int (*callback)(void*, int)=NULL );	
	int				add_callback_all_items( int (*callback)(void*, int, Application*) );	
	int				add_entry			( stVertMenuInfo mEntry 	 );

//	int				get_id				( )	{ return selected_item; };
	int				set_state			( int mState, int mIndex );
	int				set_h_parent		( HorizontalMenu* mMenu  );
	Application*	get_application		(   );
		
	// This will move it's upper left corner to the horizontal menu:
	int				attach_at			( float x, float y );

	int   			show	 			( bool mVisible	);	
	virtual void 	draw_one_row		( int mRow, float mY );	
	int			   	draw_triangle 		( int mRow, float mY );	// indicator of sub menu.
	virtual int   	draw		 		(				);


	int				get_hit_index		( int Mousex, int Mousey );
	virtual int		onHover				( int x, int y, bool mouse_is_down=true );
//	int				set_on_click_listener( void (void*) );
	virtual int		onClick				( int x, int y, bool mouse_is_down=true );
	void			clear_all()   { m_entries.clear(); };

protected:
	HorizontalMenu*			m_horiz_parent;

	int		(*callback_all_items)(void*, int, Application*);		// called when any item is selected.

	std::vector<stVertMenuInfo> m_entries;
	int						m_selection;
	bool					has_scroller;
	bool					is_visible;
};
 
#endif

