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


static Window		ParentFrame(450, 1050, 500, 100);


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
	
void FileBrowser::show_hidden( bool mShow ) 
{ 
	show_hidden_files = mShow;
}

/* Start with just 1 level */
Window* FileBrowser::create_file_browser( char* mBasePath )
{
	// Show Path across the top:
	path_descriptor = new TextView( -1, -1 );
	path_descriptor->set_text( mBasePath );
	path_descriptor->onCreate();
	pack_control( path_descriptor, PACK_FILL_PARENT, PACKER_ALIGN_TOP );

	// Create the left most file List :
	add_level( mBasePath );
	//print_window_positions();
	return this;
}

/* Extends the latest level.  ie. works from the current BasePath plus
a new item  */
void FileBrowser::add_level( char* mAppendPath )
{
	// Need a pointer to the current latest level:
	// from Window class:

	// Widths of columns : 
	printf( "add_level : %s \n", mAppendPath );
	DirectoryListBox*  tmp_dir  = new DirectoryListBox();
	//tmp_dir->onCreate();
	
	tmp_dir->set_odd_color   ( 0xFFFFFFFF 	);
	tmp_dir->set_width_height( 200, 200 	);		// height will get overwritten
	tmp_dir->populate_directories( mAppendPath, 1 );
	//printf("FileBrowser::add_level populate files: %s\n", mAppendPath );	
	tmp_dir->populate_files  ( mAppendPath, 1 );

	int size = levels.size();
	vector<DirectoryListBox*>::iterator iter;
	//printf("\tlevels=%d\n", size );
	if (size==0) {
		printf("First level::  pack_control \n");
		pack_control( tmp_dir, PACK_LEFT, PACK_FILL_PARENT );
	}
	else {
		printf("previous level: \n" );
		iter = levels.end();	iter--;
		(*iter)->print_positions();
		tmp_dir->set_position_right_of( (*iter), true, 0. );
		tmp_dir->print_positions();
		controls.push_back( tmp_dir );
	}
	levels.push_back(tmp_dir);
}

void FileBrowser::extract_complete_path()		// from the sequence of listboxes.
{
	complete_path = base_path;
	// Add:  if it doesn't end with a /, then append one.
	int size = levels.size();	
	//printf( "Base:%s;  size=%d \n", complete_path.c_str(), size );

	for (int i=0; i<size; i++)
	{
		complete_path += levels[i]->get_item_text() ;
		complete_path += "/";
	}	
	printf("extract_complete_path : %s\n", complete_path.c_str() );
}

int	FileBrowser::which_level_clicked ( float x, float y )
{
	Control* result = NULL;
	int size = levels.size();
	for (int i=0; i<size; i++)
	{
		result = levels[i]->HitTest( x,y );
		if (result)
			return i;
	}
	return -1;
}

void FileBrowser::collapse_to_level( int mLevelIndex )
{
	int size = levels.size();	
	for (int i=size; i>(mLevelIndex+1); i--)
	{
		delete levels[i];
		levels.pop_back();		
	}
}

char str[] = "/home/pi/audio_test/" ;

int	FileBrowser::onCreate(  )
{
	Window::onCreate();	
}

int FileBrowser::onClick( int Mousex, int Mousey, bool mouse_is_down )
{
	printf(" Mousex,Mousey= %d, %d\n",  Mousex,Mousey);		
	//DirectoryListBox* selected_listbox = NULL;
	bool start_closing = false;
	int size = levels.size();

	// WHICH LEVEL:
	int level_index = which_level_clicked( Mousex, Mousey );

	// WHICH ITEM:   (find the item within that level)
	int item_index = levels[level_index]->get_hit_index( Mousex, Mousey );

	printf("You clicked FileBrowser level:%d; item:%d\n", level_index, item_index );

	// SELECT ITEM:
	levels[level_index]->select( item_index );

	// COLLAPSE  open lists deeper than this:
	//collapse_to_level( level_index );

	// Now, if the selected item was a directory...
	// add 1 new listbox showing it's contents.
	extract_complete_path();
	printf("FileBrowser:: extracted path=%s\n", complete_path.c_str());

	if (path_descriptor)
		path_descriptor->set_text(complete_path.c_str());

	bool dir = levels[level_index]->is_item_a_directory();
	if (dir) {
		//printf("item is a directory2!\n");
		add_level( complete_path.c_str() );	
		//printf("add_level done!\n");
	}

	// Now click : 
	//selected_listbox->onClick(Mousex, Mousey);
	Invalidate();
}

// Need to draw a > marker on each list box's selected item.
