#ifndef _ICON_VIEW_
#define _ICON_VIEW_

#include "control.hpp"

// For starters, we'll view the images in a folder.
// latter add an image gallery.


class ImageGallery : public IconView
{
public:
	ImageGallery( char* DirectoryName );
	ImageGallery( int Left, int Right, int Top, int Bottom, char* mDirectoryName );
	ImageGallery( int Left, int Bottom, char* mDirectoryName );
	~ImageGallery();
	void Initialize();
	
	virtual void load_resources( );
		
	virtual int   	draw   (	);	
	virtual int		onClick(	);

protected:
	char*		DirectoryName;
	char*		Filename;
	BOOL		file_loaded;
	
private:
	word		style;
	VGImage 	image;
	struct image_info ImageInfo;

};

 
#endif
