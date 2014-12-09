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
	void			setup_heading_details( 						  );
	void 			compose_detail_line	( char* path, char* d_name );
	
	void			populate			( char* mPath			  );
	void			populate_files		( char* mPath, int Column );
	void			populate_directories( char* mPath, int Column );

	bool			is_item_a_directory	( int mIndex=-1 );
	char*			get_item_text		( int mIndex=-1 );
	int				get_item_id			( int mIndex=-1 ); 	// file type
		
	virtual void	draw_one_cell		( int mRow, int mCol, float mY );

protected:	
	bool			show_hidden_files;
	string			longest_path;		// compiled path of the selected items.

private:
	vector< int >		icon_id;

};

 
#endif

