#ifndef _LISTBOX_ICON_
#define _LISTBOX_ICON_


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
class IconListBox : public ListBox
{
public:
	IconListBox();
	IconListBox( int Left, int Right,  int Top,    int Bottom  );
	IconListBox( int Width, int Height );
	~IconListBox();
	
	virtual void 	Initialize			( );

	virtual int		onClick				( int x, int y, bool mouse_is_down=true );
	virtual int		onPlace		  		(  );	// chance to place children.
	virtual int		onCreate	  		(  );	// chance to load resources, 
		

protected:	
	vector< int >		icon_id;
	

};

 
#endif

