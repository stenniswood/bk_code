#ifndef _FILE_BROWSER_
#define _FILE_BROWSER_


#include "control.hpp"
#include "window.hpp"
#include "scroll_bar.hpp"
#include "scroll_control.hpp"
#include "tabular_listbox.hpp"
#include "directory_listbox.hpp"
#include "text_view.hpp"

#include <vector>
#include <list>
#include <string>
using namespace std;

/* This is the center portion of the Complete file Browser.
	It is a number of individual listboxes populated with directory entries.	

*/
class FileBrowser : public Control
{
public:
	FileBrowser();
	FileBrowser( int Left, int Right,  int Top,    int Bottom  );
	FileBrowser( int Width, int Height );
	~FileBrowser();
	
	virtual void 	Initialize			( );
	void			set_base_path		( const char* mBasePath );	
	void	 		create_file_browser (  );
	void 			add_level			( string mAppendPath );
	void			show_hidden			( bool mShow=true   );
	float			compute_width_all_dirs(  );		// width in pixels needed to show all directory list boxes.  

	virtual int		onClick				( int x, int y, bool mouse_is_down=true );

	virtual int		onPlace		  		(  );	// chance to place children.
	virtual int		onCreate	  		(  );	// chance to load resources, 

	void			extract_complete_path();		// from the sequence of listboxes.

	int				which_level_clicked ( float x, float y	);
	void 			collapse_to_level	( int mLevelIndex 	);

protected:
	// Hittest will provide the column and item #.
	// How is the window going to know which control is a listbox and which is not?
	// HitTest will tell me which one was clicked.  but I need to know which column it is
	// etc.
	// Need a pointer to the current latest level:
	vector<DirectoryListBox*> levels;	// stored here in addition to packed into the window.

	TextView*		path_descriptor;	
	bool			show_hidden_files;
	string			base_path;			// our starting point for the first level.
										// we cannot go above this. for now.
	string			complete_path;		// compiled path of the selected items.


};

 
#endif

