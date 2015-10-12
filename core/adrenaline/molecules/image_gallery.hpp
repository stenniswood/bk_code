#ifndef _IMAGE_GALLERY_
#define _IMAGE_GALLERY_

#include <list>
#include <vector>
#include <string>

#include "button.hpp"
#include "icon.hpp"
#include "control.hpp"
#include "window.hpp"


#define NUMBER_TO_READ_AHEAD 5

// For starters, we'll view the images in a folder.
// latter add an image gallery.
class ImageGallery : public Window
{
public:
	ImageGallery( );
	ImageGallery( int Left, int Right,  int Top, int Bottom, char* mDirectoryName );
	ImageGallery( int Left, int Bottom, char* mDirectoryName );
	~ImageGallery();
	void 			Initialize();

	void			set_directory     ( char* mDirectoryName );
	void 			retrieve_name_list( char* mPath, bool include_hidden_files=false );
	virtual int		onCreate		  ( );

	void			create_load_thread();
	virtual void 	load_resources    ( );

	int				find_index( int mFileListIndex );
	
	virtual int   	draw   	(	);
	virtual int		onClick	(int x, int y, bool mouse_is_down=true);

protected:
	Button*			PrevImage;
	Button*			NextImage;
	IconView*		ImageView;
	string			DirectoryName;
	
public:
	bool						close_thread;
	bool						thread_closed;	
	int							last_loaded_index;		//
	int							current_image_index;	//	index within the Filelist


private:
	word		style;
};

 
#endif
