#ifndef _ICON_VIEW_
#define _ICON_VIEW_

#include "control.hpp"

#define CENTER_HORIZONTAL 0x01
#define CENTER_VERTICAL   0x02
#define WRAP_IMAGE_SIZE	  0x10

/* Use open CV
		to read files png, jpg, etc.
*/
struct image_info
{
	unsigned int width;
	unsigned int height;
	unsigned int dstride;
	unsigned int dbpp;
	VGImage  image;	
};
extern VGImage createImageFromJpeg(const char *filename, struct image_info* II);

class IconView : public Control 
{
public:
	IconView( );
	IconView( int Left, int Right, int Top, int Bottom, char* mFileName );
	IconView( int Left, int Bottom );
	~IconView();
	void 			Initialize();
	
	void			set_file			( const char* FileName );
	void			set_image			( VGImage* mImage, struct image_info* mImageInfo );
	void			calc_margins		( );
	void			read_from_jpeg_file	( );
	void			read_from_file     	( );

	virtual void 	load_resource( );
	void			match_image_size()  {  set_width_height( ImageInfo.width, ImageInfo.height);  };

	virtual void  	set_position	( int Left, int Right, int Top, int Bottom );
	virtual void  	move_to( int mLeft, int mBottom );
	
	void 			center_vertical  ( BOOL on )	{ if (on) style |= CENTER_VERTICAL;	else style &= ~CENTER_VERTICAL;	};
	void 			center_horizontal( BOOL on )	{ if (on) style |= CENTER_HORIZONTAL; else style &= ~CENTER_HORIZONTAL;	};
		
	virtual int   	draw   (	);	
	virtual int		onClick(int x, int y, bool mouse_is_down=true );

	float			get_image_width()  { return ImageInfo.width;  };
	float			get_image_height() { return ImageInfo.height; };

protected:
	char*		Filename;
	BOOL		file_loaded;

private:
	word				style;
	VGImage 			image;
	struct image_info 	ImageInfo;

	VGfloat 	start_left;
	VGfloat 	start_bottom;
	int 		min_w;
	int 		min_h;
		
	int			left_margin;
	int			bottom_margin;
	int			font_height;
};

 
#endif
