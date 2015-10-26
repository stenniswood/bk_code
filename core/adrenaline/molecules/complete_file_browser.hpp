#ifndef _FILE_BROWSER_
#define _FILE_BROWSER_


#include "control.hpp"
#include "window.hpp"
#include "scroll_bar.hpp"
#include "scroll_control.hpp"
#include "tabular_listbox.hpp"

#include <vector>
#include <list>
#include <string>
using namespace std;

/* Has a favorites pane on left.  And a holding bar on right.

*/
class CompleteFileBrowser : public Window
{
public:
	CompleteFileBrowser();
	CompleteFileBrowser( int Left, int Right,  int Top,    int Bottom  );
	CompleteFileBrowser( int Width, int Height );
	~CompleteFileBrowser();
	
	virtual void 	Initialize			( );
	void			set_base_path		( char* mBasePath );
	

	virtual int		onClick				( int x, int y, bool mouse_is_down=true );
	virtual int		onPlace		  		(  );	// chance to place children.
	virtual int		onCreate	  		(  );	// chance to load resources, 
		

protected:	
	// Hittest will provide the column and item #.
	// How is the window going to know which control is a listbox and which is not?
	// HitTest will tell me which one was clicked.  but I need to know which column it is
	// etc.
		// Need a pointer to the current latest level:
	ListBox			m_favorites_pane;		// home, video, audio, music, pictures, documents, etc.
	FileBrowser		m_browser_pane;
	ListBox			m_clippings_pane;		// holds selected items.


};

 
#endif

