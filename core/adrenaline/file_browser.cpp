// line graph OpenVG program
// Steve Tenniswood
#include <shapes.h>
#include <fontinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"
#include "line_graph.hpp"
#include "dataset.hpp"
#include "histogram.hpp"
#include "line_graph.hpp"
#include "leveler.hpp"
#include "display.h"
#include "display_manager.hpp"
#include "button.hpp"
#include "listbox.hpp"
#include "checkbox.hpp"
#include "progressbar.hpp"
#include "text_view.hpp"
#include "scroll_bar.hpp"
#include "control.hpp"
#include "icon.hpp"
#include "visual_memory.h"
#include "listbox.hpp"
#include "tabular_listbox.hpp"
#include "window.hpp"
#include <vector>
#include "radiobutton.hpp"
#include "test_layouts.hpp"
#include "directory_listbox.hpp"
#include "file_browser.hpp"


//static FrameWindow	ParentWindow(450, 1050, 500, 100);
static Window		ParentFrame(450, 1050, 500, 100);
//static vector<ListBox>  Levels;

FileBrowser::FileBrowser()
: Window( )
{
	Initialize( );
}
FileBrowser::FileBrowser( int Left, int Right,  int Top, int Bottom  )
: Window(  Left, Right, Top, Bottom  )
{
	Initialize( );
}
FileBrowser::FileBrowser( int Width, int Height )
: Window( Width, Height )
{
	Initialize( );
}
FileBrowser::~FileBrowser()
{
}

void FileBrowser::Initialize( )
{
	printf("\tFileBrowser::Initialize()::\n");
	Window::Initialize();
	path_descriptor = NULL;
	show_hidden_files = false;
	//base_path = ;
	printf("\tFileBrowser::Initialize()::done!\n");	
}

void FileBrowser::set_base_path( char* mBasePath )
{
	create_file_browser( mBasePath );
	base_path = mBasePath;
}

void FileBrowser::select_item( int mColumn, int mItem )
{
}
	
/* Start with just 1 level */
Window* FileBrowser::create_file_browser( char* mBasePath )
{
	// Show Path across the top:
	path_descriptor = new TextView( -1, -1 );
	path_descriptor->set_text( mBasePath );
	pack_control( path_descriptor, PACK_FILL_PARENT, PACKER_ALIGN_TOP );

	// Create the left most file List :
	add_level( mBasePath );
	//print_window_positions();
	return this;
}

/* Extends the latest level.  ie. works from the current BasePath plus
a new item
*/
void FileBrowser::add_level( char* mAppendPath )
{
	// Need a pointer to the current latest level:
	// from Window class:
	//vector<Control*>::iterator iter = levels.end();
	//int size = levels.Size();
	
	// Widths of columns : 
	DirectoryListBox*  tmp_dir  = new DirectoryListBox();
	tmp_dir->set_odd_color   ( 0xFFFFFFFF 	);
	tmp_dir->set_width_height( 200, 200 	);		// height will get overwritten
	tmp_dir->populate_directories( mAppendPath, 1 );
	tmp_dir->populate_files  ( mAppendPath, 1 );

	pack_control( tmp_dir, PACK_LEFT, PACK_FILL_PARENT );
	levels.push_back(tmp_dir);
	printf("DirectoryListBox::level1 Positions:\n");
	tmp_dir->print_positions();	
}

void FileBrowser::extract_complete_path()		// from the sequence of listboxes.
{
	complete_path = base_path;
	//vector<Control*>::iterator iter =  levels.begin();
	int size = levels.size();
	int i=0;
	while( i<size  )
	{
		complete_path += levels[i]->get_item_text();
		i++;	
	}	
}
	

int FileBrowser::onClick( int Mousex, int Mousey, bool mouse_is_down )
{
	printf("You clicked a FileBrowser::\n");
	DirectoryListBox* selected_listbox = NULL;
	bool start_closing = false;
	int size = levels.size();
	int i=0;
	while( i<size )
	{
	
		if (start_closing)
			delete levels[i];		// collapse the open lists.
			
		printf(" Mousex,Mousey= %d, %d\n",  Mousex,Mousey);
		
		// Safe cast since levels only contains directory lists:
		selected_listbox = (DirectoryListBox*)levels[i]->HitTest( Mousex,Mousey );	
		if (selected_listbox)
		{
			selected_listbox->print_positions();
			printf("You clicked a level #%d\n", i);
			// close up any lists right of this one.
			start_closing = true;
			selected_listbox->onClick(Mousex, Mousey);
		}
		i++;
	}
	// Now, if the selected item was a directory...
	// add 1 new listbox showing it's contents.
	extract_complete_path();
	path_descriptor->set_text(complete_path.c_str());
	
	printf("FileBrowser:: extracted path=%s\n", complete_path.c_str());
	bool dir = selected_listbox->is_item_a_directory();
	if (dir)
		add_level( complete_path.c_str() );
	Invalidate();
}

