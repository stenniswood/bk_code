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
#include "dataset.hpp"
#include "adrenaline_windows.h"
#include "display_manager.hpp"
#include "visual_memory.h"
#include <vector>
#include "test_layouts.hpp"
#include "file_browser.hpp"


FileBrowser::FileBrowser()
: Control( )
{
	Initialize( );
}
FileBrowser::FileBrowser( int Left, int Right,  int Top, int Bottom  )
: Control(  Left, Right, Top, Bottom  )
{
	Initialize( );
}
FileBrowser::FileBrowser( int Width, int Height )
: Control( Width, Height )
{
	Initialize( );
}
FileBrowser::~FileBrowser()
{
}

void FileBrowser::Initialize( )
{
	printf("\n\tFileBrowser::Initialize()::\n");
	Control::Initialize();
	path_descriptor   = NULL;
	show_hidden_files = false;
	printf("\tFileBrowser::Initialize()::done!\n");
}

void FileBrowser::set_base_path( char* mBasePath )
{
	base_path = mBasePath;
}

void FileBrowser::show_hidden( bool mShow ) 
{ 
	show_hidden_files = mShow;
}

char str[] = "/home/pi/";
int	FileBrowser::onCreate(  )
{
	printf("FileBrowser::onCreate(  )\n");
	Control::onCreate();
	create_file_browser( );	
	printf("FileBrowser::onCreate(  ) done\n");	
}

/* Start with just 1 level */
Control* FileBrowser::create_file_browser( )
{
	// Show Path across the top : 
	printf("FileBrowser::create_file_browser()\n");
	path_descriptor = new TextView	( width, -1 );
	path_descriptor->set_text		( base_path.c_str() );
	path_descriptor->set_text_size	(14);	
	path_descriptor->onCreate();
	path_descriptor->move_to		(left, bottom+height-path_descriptor->get_height() );
	//register_child( path_descriptor );
	path_descriptor->print_positions();
	// Create the left most file List :
	add_level( base_path.c_str() );
	//print_window_positions();
	return this;
}

/* Extends the latest level.  ie. works from the current BasePath plus
a new item  */
void FileBrowser::add_level( string mAppendPath )
{
	printf( "add_level : %s \n", mAppendPath.c_str() );
	DirectoryListBox*  tmp_dir  = new DirectoryListBox();
	float ht = height - path_descriptor->get_height();

	tmp_dir->set_odd_color   ( 0xFF9FFFFF 	);
	tmp_dir->set_width_height( 200, ht 	);		// height will get overwritten
	tmp_dir->populate_directories( mAppendPath.c_str(), 1 );
	//tmp_dir->populate_files  ( mAppendPath, 1 );
	printf( "add_level : populated directories. \n" );

	int size = levels.size();	
	vector<DirectoryListBox*>::iterator iter;
	if (size==0) {
		printf("First level::  put at left\n");
		tmp_dir->move_to(left, bottom);
	}
	else {
		printf("previous level: \n" );
		iter = levels.end();	iter--;
		(*iter)->print_positions();
		tmp_dir->set_position_right_of( (*iter), true, 0. );
		tmp_dir->print_positions();
	}
	register_child  (tmp_dir);
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


int FileBrowser::onClick( int Mousex, int Mousey, bool mouse_is_down )
{
	printf(" Mousex,Mousey= %d, %d\n",  Mousex,Mousey);		
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
