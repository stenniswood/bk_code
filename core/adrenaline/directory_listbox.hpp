#ifndef _DIRECTORY_LISTBOX_
#define _DIRECTORY_LISTBOX_

#include "control.hpp"
#include "scroll_bar.hpp"
#include "scroll_control.hpp"
#include "tabular_listbox.hpp"

#include <vector>
#include <list>
#include <string>
using namespace std;


#define FILE_ID 	 1
#define DIRECTORY_ID 2

class DirectoryListBox : public TabularListBox
{
public:
	DirectoryListBox();
	DirectoryListBox( int Left, int Bottom, int mWidth, int mNumber_items_shown, int mItem_height );
	DirectoryListBox( int Left, int Right,  int Top,    int Bottom  );
	~DirectoryListBox();
	void 			Initialize			( );
	
	void			setup_headings		( 						  );
	void			populate_files		( char* mPath, int Column );
	void			populate_directories( char* mPath, int Column );

	virtual void	draw_one_cell		( int mRow, int mCol, float mY );

protected:	
	bool			show_hidden_files;


private:
	vector< int >		icon_id;

};

 
#endif

