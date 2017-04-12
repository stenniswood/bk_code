#ifndef _COMPLETE_FILE_BROWSER_
#define _COMPLETE_FILE_BROWSER_

#include "control.hpp"
#include "window.hpp"
#include "scroll_bar.hpp"
#include "scroll_control.hpp"
#include "tabular_listbox.hpp"
#include "icon_bar.hpp"
#include "listbox.hpp"
#include "directory_listbox.hpp"

#include <vector>
#include <list>
#include <string>

using namespace std;

/******************************************************************/
/* Has a favorites pane on left.  And a holding bar on right.*/
class FavoritesPane : public ListBox
{
public:
	FavoritesPane();
	virtual int		onCreate(  );
	
	vector<string>	m_paths;
};
/******************************************************************/

/******************************************************************/
/* Has a favorites pane on left.  And a holding bar on right.*/
class ClippingsPane : public ListBox
{
public:
	ClippingsPane();
	
	void			add_clipping	( string mText );

	virtual int		onCreate(  );	
	vector<string>	m_clippings;
};

/******************************************************************/
class CompleteFileBrowser : public Window
{
public:
	CompleteFileBrowser();
	CompleteFileBrowser( int Left, int Right,  int Top,    int Bottom  );
	CompleteFileBrowser( int Width, int Height );
	~CompleteFileBrowser();
	
	virtual void 	Initialize			( );
	void			set_base_path		( const char* mBasePath );
	
	virtual int		onClick				( int x, int y, bool mouse_is_down=true );
	virtual int		onPlace		  		(  );	// chance to place children.
	virtual int		onCreate	  		(  );	// chance to load resources, 
	
protected:	
	// Hittest will provide the column and item #.
	// How is the window going to know which control is a listbox and which is not?
	// HitTest will tell me which one was clicked.  but I need to know which column it is
	// etc.
	IconBar			m_view_bar;
	IconBar			m_ops_bar;
	
/*	Button			m_detail_view;
	Button			m_icons_view;	
	Button			m_names_and_preview_view;
	Button			m_detail_view;*/
		

	FavoritesPane	m_favorites_pane;		// home, video, audio, music, pictures, documents, etc.
	FileBrowser		m_browser_pane;
	ClippingsPane*	m_clippings_pane;		// holds selected items.

};

 
#endif

