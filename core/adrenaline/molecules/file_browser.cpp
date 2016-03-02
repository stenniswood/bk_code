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

#define Debug 0


FileBrowser::FileBrowser()
: Control()
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
	if (Debug) printf("\n\tFileBrowser::Initialize()::\n");
	//Control::Initialize();
	path_descriptor   = NULL;
	show_hidden_files = false;
	if (Debug) printf("\tFileBrowser::Initialize()::done!\n");
}

void FileBrowser::set_base_path( char* mBasePath )
{
	if (Debug) printf("Set base_path( %s )\n", mBasePath );
	base_path = mBasePath;
	if (Debug) printf("FileBrowser:: base_path = %s\n", base_path.c_str() );
	path_descriptor->Invalidate();
	collapse_to_level(0);
	add_level( base_path.c_str() );
	onPlace();
	Invalidate();
}

void FileBrowser::show_hidden( bool mShow ) 
{ 
	show_hidden_files = mShow;
}

// width in pixels needed to show all directory list boxes
float FileBrowser::compute_width_all_dirs(  )		
{
	float total_width = 0;
	vector<DirectoryListBox*>::iterator iter = levels.begin();
	while (iter != levels.end())
	{
		total_width += (*iter)->get_width();		
		iter++;	
	}
	return total_width;
}

int FileBrowser::onPlace	(  )	// chance to place children.
{
	// if views take more room than available:
	if (Debug) printf("FileBrowser::onPlace\n");
	
	float total_width = compute_width_all_dirs();
	if (Debug) printf("total_width=%6.2f, width=%6.2f\n", total_width, width );
	
	if (total_width > width)
	{
		// Start packing from the right.
		float last_right = left; // (*levels.begin())->get_width() + left;		// Save room for the first (root) directory
		//printf("last_right=%6.2f\n", last_right);
		float left_edge = (left+width);
		int iter = levels.size()-1;
		if (Debug) printf("last_right=%6.2f;  edge=%6.2f\n", last_right, left_edge );
		while (iter >= 0)
		{
			left_edge -= levels[iter]->get_width();
			if (Debug) printf("left_edge=%6.2f;\n", left_edge );			
			// Until no more room (leaving favorites)
			if (left_edge > last_right)
				levels[iter]->move_to( left_edge, bottom);
			else
				levels[iter]->hide();		// don't show those early ones that don't fit on the window.
			iter--;	
		}
		Invalidate();
	} else {
		vector<DirectoryListBox*>::iterator prev = levels.begin();
		vector<DirectoryListBox*>::iterator iter = levels.begin();
		(*iter)->move_to( left, bottom );
		iter++;
		while (iter != levels.end())
		{
			(*iter)->set_position_right_of( (*prev), true, 0. );
			if (Debug) (*iter)->print_positions();
			prev = iter;
			iter++;	
		}
		Invalidate();		
	}
	if (Debug) printf("FileBrowser::onPlace()  done\n");
	return 1;	
}

/* Start with just 1 level */
void FileBrowser::create_file_browser( )
{
	// Show Path across the top : 
	if (Debug) printf("FileBrowser::create_file_browser() %s \n", base_path.c_str() );
	path_descriptor = new TextView	( width, 32 );
	path_descriptor->set_text		( base_path.c_str() );
	path_descriptor->set_text_size  ( 14 );
	path_descriptor->onCreate		( );
	
	float h = bottom+height - path_descriptor->get_height();

	if (Debug) printf(" %s \n", base_path.c_str() );	
	path_descriptor->move_to		(left, h );
	path_descriptor->print_positions();
	register_child( path_descriptor );

	// Create the first file List :
	//add_level( base_path );
	//onPlace();
	//print_window_positions();
}

int	FileBrowser::onCreate(  )
{
	// Width & Height should be set by now!
	if (Debug) printf("FileBrowser::onCreate() \n");
	create_file_browser( );
	return Control::onCreate  ( );
	//if (Debug) printf("FileBrowser::onCreate(  ) done\n");	
}

 
/* Extends the latest level.  ie. works from the current BasePath plus
a new item  */
void FileBrowser::add_level( string mAppendPath )
{
	if (Debug) printf( "add_level : %s \n", mAppendPath.c_str() );
	DirectoryListBox*  tmp_dir  = new DirectoryListBox();
	tmp_dir->set_odd_color       ( 0xFF9FFFFF 	);
	tmp_dir->populate_directories( mAppendPath.c_str(), 1 );

	float ht = height - path_descriptor->get_height();
	tmp_dir->set_width_height( 200, ht );		// height will get overwritten
	tmp_dir->populate_files( mAppendPath.c_str(), 1 );

	register_child  (tmp_dir);
	levels.push_back(tmp_dir);	
	tmp_dir->z_order = -1;
}

void FileBrowser::extract_complete_path()		// from the sequence of listboxes.
{
	complete_path = base_path;
	// Add:  if it doesn't end with a /, then append one.
	int size = levels.size();	
	//printf( "Base:%s;  size=%d\n", complete_path.c_str(), size );

	for (int i=0; i<size; i++)
	{
		complete_path += levels[i]->get_item_text() ;
		complete_path += "/";
	}	
	if (Debug) printf("extract_complete_path : %s\n", complete_path.c_str() );
}

int	FileBrowser::which_level_clicked ( float x, float y )
{
	Control* result = NULL;
	int size = levels.size();
	for (int i=0; i<size; i++)
	{
		if (levels[i]->is_visible())
		{	
			result = levels[i]->HitTest( x,y );
			if (result)
				return i;
		}
	}
	return -1;
}

void FileBrowser::collapse_to_level( int mLevelIndex )
{
	if (Debug) printf("FileBrowser::collapse_to_level( %d )\n", mLevelIndex );
	int size = levels.size();
	for (int i=size-1; i>=mLevelIndex; i--)
	{
		unregister_child( levels[i] );
		delete levels[i];
		levels.pop_back();		
	}
}


int FileBrowser::onClick( int Mousex, int Mousey, bool mouse_is_down )
{
	int retval = 0;
	//if (Debug) 
	if (Debug) printf("FileBrowser::onClick() Mousex,Mousey= %d, %d\n",  Mousex,Mousey);		
	bool start_closing = false;
	int  size = levels.size();

	// WHICH LEVEL:
	int level_index = which_level_clicked( Mousex, Mousey );
	if (Debug) printf("which_level_clicked()=%d\n", level_index );
	if (level_index==-1) return 0;

	// COLLAPSE  open lists deeper than this:
	collapse_to_level( level_index+1 );
	if (Debug) printf("collapse_to_level() done\n");

	// WHICH ITEM:   (find the item within that level)
	int item_index = levels[level_index]->get_hit_index( Mousex, Mousey );
	if (Debug) printf("You clicked FileBrowser level:%d; item:%d\n", level_index, item_index );

	// SELECT ITEM:
	levels[level_index]->select( item_index );

	// Now, if the selected item was a directory...
	// add 1 new listbox showing it's contents.
	extract_complete_path();
	if (Debug) printf("FileBrowser:: extracted path=%s\n", complete_path.c_str());

	if (path_descriptor)
		path_descriptor->set_text(complete_path.c_str());

	bool dir = levels[level_index]->is_item_a_directory();
	if (dir) {
		//printf("item is a directory2!\n");
		add_level( complete_path.c_str() );	
		if (Debug) printf("add_level done!\n");
	}

	onPlace();
	Invalidate();
	return retval;
}

// Need to draw a > marker on each list box's selected item.
