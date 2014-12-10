#ifndef _VERTICAL_MENU_H_
#define _VERTICAL_MENU_H_

#include <vector>
#include "control.hpp"

class VerticalMenu;

struct stVertMenuInfo {
	char	text[50];
	char	alt_key[4];				// to activate from keyboard once menu is already active.
	char	short_cut_key[4];		// to directly activate from keyboard 
	float	width;
	int		state;
	int		(*callback)(void*);		// called when the item is selected.
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

	virtual void 	Initialize(	);
	int				calc_metrics();
 
	int				add_simple_command	( char* mText, char* mShortcut=NULL );
	int				add_sub_menu		( char* mText, VerticalMenu* mSubMenu=NULL );	
	int				add_entry			( stVertMenuInfo mEntry 	 );

	int				get_id				( )	{ return selected_item; };
	int				set_state			( int mState, int mIndex );
	
	// This will move it's upper left corner to the horizontal menu:
	int				attach_at			( float x, float y );

	int   			show	 			( bool mVisible	);	
	virtual void 	draw_one_row		( int mRow, float mY );	
	virtual int   	draw		 		(				);

	int				get_hit_index		( int Mousex, int Mousey );
//	int				set_on_click_listener( void (void*) );	
	virtual int		onClick(int x, int y, bool mouse_is_down=true);

protected:
	std::vector<stVertMenuInfo*> m_entries;
	int						m_selection;
	bool					has_scroller;
	bool					is_visible;
};
 
#endif

