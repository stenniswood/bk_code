// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <dirent.h> 
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "listbox.hpp"
#include "display.h"
#include "icon.hpp"
#include "tabular_listbox.hpp"
#include "directory_listbox.hpp"


static 	struct image_info ImageInfo;
static	VGImage folder_image; //= createImageFromJpeg( "./resources/folder.jpg", &ImageInfo );


DirectoryListBox::DirectoryListBox()
:TabularListBox()
{
	printf("DirectoryListBox::ctor()\n");
	Initialize(); 
	printf("DirectoryListBox::initialized\n");	
}

DirectoryListBox::DirectoryListBox( int Left, int Bottom, int mWidth, int mNumber_items_shown, int mItem_height )
:TabularListBox(  Left,  Bottom,  mWidth,  mNumber_items_shown,  mItem_height )
{
	Initialize();
}

DirectoryListBox::DirectoryListBox( int Left, int Right, int Top, int Bottom  )
:TabularListBox( Left,  Right,  Top,  Bottom )
{
	Initialize();
}

DirectoryListBox::~DirectoryListBox( )
{
	//delete Headers;
}

void DirectoryListBox::Initialize()
{
	width=200; height=100;
	show_hidden_files = false;
	set_odd_color 		 ( 0xFFCFCFFF );
	set_even_color		 ( 0xFFFFFFFF );
	setup_headings		 ( 			);		// should place start_x!

	// Load Directory Folder icon:
	if (folder_image==0)
		folder_image = createImageFromJpeg( "./resources/folder.jpg", &ImageInfo );
}

void DirectoryListBox::setup_headings( )
{
	vector<struct HeaderItemInfo>*  Headers = new vector<struct HeaderItemInfo>;
	struct HeaderItemInfo hi;
	hi.text      = "Filename";
	hi.alignment = HEADER_ALIGN_LEFT;

	Headers->push_back( hi );
	set_headings( Headers );
}

void DirectoryListBox::draw_one_cell( int mRow, int mCol, float mY )
{
	if (Headings==NULL) return;
	//printf("draw_one_cell: col=%d;  %d\n", mCol, mRow );
	
	if (icon_id[mRow]==DIRECTORY_ID)
		vgSetPixels( (*Headings)[mCol].start_x, mY, folder_image, 0, 0, 
						ImageInfo.width, ImageInfo.height );

	// Compute measurements for correctly placing the text:
	float text_width = TextWidth( LineData[mRow][mCol].c_str(), SerifTypeface, text_size );
	int   col_space  = ((*Headings)[mCol].width - text_width - ImageInfo.width);
	int   indent     = col_space / 2.0;
	float above_line_offset = (LineHeight-text_size)/2.0;
	int   x;

	// Determine the X start pixel:
	if 		((*Headings)[mCol].alignment == HEADER_ALIGN_LEFT)
	{
		x = (*Headings)[mCol].start_x + ImageInfo.width;
		Text( x, mY+above_line_offset, (char*)LineData[mRow][mCol].c_str(), SerifTypeface, text_size );	
	} else if ((*Headings)[mCol].alignment == HEADER_ALIGN_CENTER)
	{
		x = (*Headings) [mCol].start_x + indent;
		TextMid( x, mY+above_line_offset, (char*)LineData[mRow][mCol].c_str(), SerifTypeface, text_size );			
	} else if ((*Headings)[mCol].alignment == HEADER_ALIGN_RIGHT)
	{
		x = (*Headings)[mCol].end_x - text_width;
		TextEnd( x, mY+above_line_offset, (char*)LineData[mRow][mCol].c_str(), SerifTypeface, text_size );			
	}
}

bool	DirectoryListBox::is_item_a_directory( int mIndex )
{
	if (mIndex==-1)	mIndex = selected_item;
	return (icon_id[mIndex]==DIRECTORY_ID) ? true: false;
}
char*	DirectoryListBox::get_item_text ( int mIndex )
{
	if (mIndex==-1)	mIndex = selected_item;
	return LineData[mIndex][0].c_str();
}
int	DirectoryListBox::get_item_id	( int mIndex )
{		// file type
	if (mIndex==-1)	mIndex = selected_item;
	return icon_id[mIndex];
}

void DirectoryListBox::populate_files( char* mPath, int Column )
{
	DIR           *d;
	struct dirent *dir;
	d 	= opendir(mPath);  
	vector<string>* 	line_text = NULL;
	bool				is_hidden_file = false;

	if (d)
	{
		// First, go thru all FILES:
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type==DT_REG)  
			{
				//printf("%s\n", dir->d_name);
				is_hidden_file = (dir->d_name[0]=='.')?true:false;
				if ((show_hidden_files==false) && (is_hidden_file))
				{
					continue;		// skip!
				}
				line_text = new vector<string>;
				line_text->push_back( dir->d_name );
				add_row( line_text );
				icon_id.push_back( FILE_ID );
			}        
		}
		//rewinddir(d);
		closedir(d);
		//calc_widths_from_text();
		calc_column_positions_from_widths();
	}
}

void DirectoryListBox::populate_directories( char* mPath, int Column )
{
	DIR           *d;
	struct dirent *dir;
	d 	= opendir(mPath);  
	vector<string>* 	line_text = NULL;
	bool				is_hidden_file = false;

	if (d)
	{
		// Next, go thru all DIRECTORIES:	
		while ((dir=readdir(d)) != NULL)
		{	
			// Next go thru all directories:
			// if a directory, then recurse:
			if (dir->d_type==DT_DIR)  
			{
				//printf("%s\n", 		dir->d_name);
				is_hidden_file = (dir->d_name[0]=='.') ? true:false;
				if ((show_hidden_files==false) && (is_hidden_file))
				{
					continue;		// skip!
				}
				line_text = new vector<string>;
				line_text->push_back( dir->d_name );
				add_row( line_text );
				icon_id.push_back( DIRECTORY_ID );				
			}
		}
		closedir(d);
	}	
}


	