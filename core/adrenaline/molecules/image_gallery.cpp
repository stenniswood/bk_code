// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include <vector>
#include <dirent.h> 
#include <math.h>
#include "CAN_Interface.h"
#include "adrenaline_windows.h"
#include "display_manager.hpp"
#include "visual_memory.h"
#include "test_layouts.hpp"

/*#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
#include "button.hpp"
#include "image_gallery.hpp" */

#define Debug 1

ImageGallery::ImageGallery( )
: Window(  )
{
	Initialize();
}
ImageGallery::ImageGallery( int Left, int Right, int Top, int Bottom, char* mDirectoryName )
: Window (Left, Right, Top, Bottom)
{
	DirectoryName = mDirectoryName;
	Initialize();
}
ImageGallery::ImageGallery( int Width, int Height, char* mDirectoryName )
: Window (Width,Height)
{
	DirectoryName = mDirectoryName;
	Initialize();
}
ImageGallery::~ImageGallery()
{
	delete PrevImage;
	delete NextImage;
}

void ImageGallery::Initialize()
{
	Window::Initialize();
	PrevImage = new Button(20,-1);
	NextImage = new Button(20,-1);	
	ImageView = new IconView();
	//if (Debug) printf("ImageGallery::Initialize()1\n");
	current_image_index = 0;
		
}


int ImageGallery::onCreate()
{
	PrevImage->set_text("<");		// font has to be loaded before this will work!!
	NextImage->set_text(">");		// It is loaded in init_screen();

	//ImageView->set_file("/home/pi/abkInstant/media/golf_1.jpg");
	//ImageView->load_resources();

	Rectangle* rect = MainDisplay.get_useable_rect( );
	printf("rect = ");  rect->print_positions();  printf("\n ");
	set_position(rect);
	print_positions();	

	pack_control( PrevImage, PACK_LEFT,  		PACK_FILL_PARENT );
	pack_control( NextImage, PACK_RIGHT, 		PACK_FILL_PARENT );
	pack_control( ImageView, PACK_FILL_PARENT, 	PACK_FILL_PARENT ); 
}


void ImageGallery::set_directory( char* mDirectoryName )
{
	DirectoryName = mDirectoryName;
	if (Debug) printf( "ImageGallery::set_directory()  %s\n", DirectoryName.c_str() );
	retrieve_name_list( mDirectoryName, false );

	//ImageView->set_file("/home/pi/abkInstant/media/golf_1.jpg");
	load_resources();
}

void ImageGallery::load_resources( )
{
	VGImage image;
	struct image_info ii;
	
	images.clear();
	ImageInfo.clear();

	int fs = Filelist.size();
	int number = min( NUMBER_TO_READ_AHEAD,   fs);
	if (Debug) {
		printf("ImageGallery: total files=%d \n", fs );
		printf("			  loading #[%d..%d] \n", current_image_index, current_image_index+number );
	}
	int i;
	// Load images ahead:
	for (i=current_image_index; i<current_image_index+number; i++)
	{
		image = createImageFromJpeg( Filelist[i].c_str(), &ii );
		if (Debug) printf("Loaded: %s : \n", Filelist[i].c_str() );
		images.push_back(image);
		ImageInfo.push_back(ii);
	}
	ImageView->set_image( &images[1], &(ImageInfo[1]) );
	if (Debug)
		printf("setImage:  %s %d \n", Filelist[0].c_str(), vgGetError() );
}

void ImageGallery::retrieve_name_list(char* mPath, bool include_hidden_files )
{
	DIR           *d;
	struct dirent *dir;
	d 	= opendir(mPath);  
	string 	Pathfilename;
	bool				is_hidden_file = false;
	if (d)
	{		
		// Go thru all FILES:
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type==DT_REG)  
			{
				is_hidden_file = (dir->d_name[0]=='.')?true:false;
				if ((include_hidden_files==false) && (is_hidden_file))
				{
					continue;		// skip!
				}
				Pathfilename = DirectoryName + dir->d_name;
				if (Debug) printf( "%s\n", Pathfilename.c_str() );
				Filelist.push_back( Pathfilename );
			}
		}
		//rewinddir(d);
		closedir(d);
	} else {
		if (Debug) printf("ImageGaller::Retrieve_name_list: Error path is not a directory: %s \n", mPath );
	}
}

int  ImageGallery::draw( )
{
	Window::draw();
}

int	 ImageGallery::onClick(int x, int y, bool mouse_is_down)
{
	Control* hit = PrevImage->HitTest( x,y );	
	if (hit)
	{
		// Prev image:
		current_image_index--;
		if (current_image_index<0)  
			current_image_index=0;

		if (Debug) printf("PrevImage Button clicked\n");
		ImageView->set_image( &images[current_image_index], &(ImageInfo[current_image_index]) );
	}

	hit = NextImage->HitTest( x,y );
	if (hit)
	{
		// Next image:
		current_image_index++;
		if (current_image_index>=images.size())  
			current_image_index=images.size()-1;

		if (Debug) printf("NextImage Button clicked\n");
		ImageView->set_image( &images[current_image_index], &(ImageInfo[current_image_index]) );
	}	
}


