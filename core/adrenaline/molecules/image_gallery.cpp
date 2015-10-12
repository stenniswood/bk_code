// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <pthread.h>
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

#define Debug 1

std::vector<std::string>		Filelist;				// Contains all found images.
std::vector<VGImage> 			images;
std::vector<struct image_info> 	ImageInfo;
std::vector<int> 				ImageIndex;		// into Filelist.


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
	close_thread = true;
	while(thread_closed==false) {};
	
	delete PrevImage;
	delete NextImage;
}

void ImageGallery::Initialize()
{
	Window::Initialize();
	PrevImage = new Button(20,-1);
	NextImage = new Button(20,-1);	
	ImageView = new IconView();
	
	current_image_index  = 1;
	last_loaded_index    = 1;
	close_thread  = false;
	thread_closed = false;		// acknowledgement.
	
	images.clear();
	ImageInfo.clear();	
	ImageIndex.clear();
}

int ImageGallery::onCreate()
{
	PrevImage->set_text("<");		// font has to be loaded before this will work!!
	NextImage->set_text(">");		// It is loaded in init_screen();

	Rectangle* rect   = MainDisplay.get_useable_rect( );
	printf("rect = ");  rect->print_positions();  printf("\n ");
	set_position(rect);
	print_positions();

	// Should already have a preloaded buffer of images.
	//std::list<VGImage>::iterator iter = images.begin();	
//	while (images.empty()) {};
//	ImageView->set_image( &(*images.begin()), &(*ImageInfo.begin()) );
//	if (Debug)
//		printf("setImage:  %s %d \n", Filelist[0].c_str(), vgGetError() );

	pack_control( PrevImage, PACK_LEFT,  		PACK_FILL_PARENT );
	pack_control( NextImage, PACK_RIGHT, 		PACK_FILL_PARENT );
	pack_control( ImageView, PACK_FILL_PARENT, 	PACK_FILL_PARENT ); 
	return Window::onCreate();
}

void ImageGallery::set_directory( char* mDirectoryName )
{
	DirectoryName = mDirectoryName;
	if (Debug) printf( "ImageGallery::set_directory()  %s\n", DirectoryName.c_str() );
	retrieve_name_list( mDirectoryName, false );
}

void ImageGallery::retrieve_name_list(char* mPath, bool include_hidden_files )
{
	DIR           *d;
	struct dirent *dir;
	d 	= opendir(mPath);  
	string 	Pathfilename;
	bool	is_hidden_file = false;
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
	return Window::draw();
}



pthread_t 	load_thread_id;

 VGImage image;
 struct image_info ii;

void* image_load_function(void* msg)
{
	ImageGallery* ig = (ImageGallery*) msg;	

	int fs = Filelist.size();
	while (ig->close_thread==false)
	{
		int to_end = fs - ig->current_image_index;
		int number = min( NUMBER_TO_READ_AHEAD, to_end );
		int expected_last_index = ig->current_image_index + NUMBER_TO_READ_AHEAD;
		// Assuming forward direction only for now!
		
		// Load images ahead : 		
		if (ig->last_loaded_index < expected_last_index)	// need to load another!		
		{
			ig->last_loaded_index++;
			image = createImageFromJpeg( Filelist[ig->last_loaded_index].c_str(), &ii );
			images.push_back(image);
			ImageInfo.push_back(ii);
			ImageIndex.push_back(ig->last_loaded_index);
			if (Debug) 
				printf("Loaded: %d %s : %x %x   %x\n", ig->last_loaded_index, Filelist[ig->last_loaded_index].c_str(),
						images.back(), images.size(), ImageInfo.end() );
				
		} else { printf("THREAD STOPPED!!\n");   ig->close_thread= true;  }
	}	
	ig->thread_closed = true;
	return NULL;
}

/* This should preload the initial buffer */
void ImageGallery::load_resources( )
{
	VGImage image;
	struct image_info ii;
	
	images.clear();
	ImageInfo.clear();

	int fs = Filelist.size() - current_image_index;
	int number = min( NUMBER_TO_READ_AHEAD, fs );
	if (Debug) {
		printf("ImageGallery: total files=%d \n", Filelist.size() );
		printf("			  loading #[%d..%d] \n", current_image_index, 
													 current_image_index+number );
	}
	// Load images ahead:
	int i;
	for (i=0; i<Filelist.size(); i++)
	{
		image = createImageFromJpeg( Filelist[i].c_str(), &ii );
		images.push_back(image);
		ImageInfo.push_back(ii);
		ImageIndex.push_back(i);
		last_loaded_index = i;
		if (Debug) printf("Loaded: %s  %x: \n", Filelist[i].c_str(), images.back() );
	}
}

void ImageGallery::create_load_thread()
{
	//image_load_function( (void*)this );
	close_thread = false;
	int iret1 = pthread_create( &load_thread_id, NULL, image_load_function, (void*) this);
	if (iret1)
	{
		fprintf(stderr,"\n****Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	} 
	while (close_thread==false) {};
}

int  ImageGallery::find_index( int mFileListIndex )
{
	int count = 0;
	std::vector<int>::iterator 			   iter = ImageIndex.begin();
	//printf("\n\nFind_index : %d\n", *iter );
	while (((*iter) != mFileListIndex) && (iter != ImageIndex.end()))
	{
		printf(" linkedList %d = %d\n", *iter, count );		
		iter++;
		count++;
	}
	printf(" %d is the %d entry within the linked list!\n", mFileListIndex, count );
	return count;
}

int	 ImageGallery::onClick(int x, int y, bool mouse_is_down)
{
	// This demonstrates an alternate way of handling Button presses.  
	// Instead of the PrevImage->set_on_click_listener();
	static char str[255];
//	std::list<VGImage>::iterator 		   iter    = images.begin();
//	std::list<struct image_info>::iterator ii_iter = ImageInfo.begin();
//	std::list<int>::iterator 			   iindex_iter = ImageIndex.begin();

	Control* hit = PrevImage->HitTest( x,y );		
	if (hit)
	{
		// Prev image:
		current_image_index--;
		if (current_image_index<0)  {
			current_image_index=0;
			MainDisplay.m_status.set_text( "At beginning.");
		} else  if (current_image_index<=last_loaded_index)  {
			// have to wait!
			current_image_index--;		// hold position (ie back off last increment)
			sprintf(str, "loading %d ...", last_loaded_index );
			MainDisplay.m_status.set_text(str);			
		} else {

			sprintf(str, "%d / %d : %s", current_image_index, Filelist.size(), Filelist[current_image_index].c_str() );
			MainDisplay.m_status.set_text(str);

			int count = find_index(current_image_index);		
//			ImageView->set_image( &(*iter), &(*ii_iter) );
			Invalidate();
			images.pop_back();
			ImageInfo.pop_back();			
		}
		if (Debug) printf("PrevImage Button clicked\n");
		return 1;
	}

	hit = NextImage->HitTest( x,y );
	if (hit)
	{
		// Next image:
		current_image_index++;
		if (current_image_index>=Filelist.size())  {
			current_image_index=Filelist.size()-1;
			MainDisplay.m_status.set_text( "At end.");			
		} else if (current_image_index>=last_loaded_index) {
			// have to wait!
			current_image_index--;		// hold position (ie back off last increment)
			sprintf(str, "loading %d ...", last_loaded_index );
			printf("%s\n", str);
			MainDisplay.m_status.set_text(str);
		} else {
			sprintf(str, "%d/%d:%s", current_image_index, Filelist.size(), 
										 Filelist[current_image_index].c_str());
			printf("NEXT IMAGE %s\n", str );
			MainDisplay.m_status.set_text(str);
			//MainDisplay.m_status.Invalidate();

/*			int count = find_index(current_image_index);		
			for (int j=0; j<count; j++)
			{
				iter++;
				ii_iter++;
				iindex_iter++;  // = iindex_iter + 2; //current_image_index;
			} */
//			ImageView->set_image( &(*iter), &(*ii_iter) );
			printf(" %d \n", current_image_index );
			ImageView->set_image( &(images[current_image_index]), &(ImageInfo[current_image_index] ) );
//			ImageView->set_image( &image, &ii );
			//printf("\n\t ImageView->set_image %x,  %x \n\n", &(*iter), &(*ii_iter) );
			
			//ImageView->Invalidate();
			Invalidate();

//			images.pop_front();
//			ImageInfo.pop_front();
		}
		if (Debug) printf("NextImage Button clicked\n");
		return 1;
	}	
	return 0;
}


