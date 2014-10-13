#ifndef _ICON_VIEW_
#define _ICON_VIEW_

#include "control.hpp"

#define CENTER_HORIZONTAL 0x01
#define CENTER_VERTICAL   0x02
#define WRAP_IMAGE_SIZE	  0x10

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
	IconView( char* FileName );
	IconView( int Left, int Right, int Top, int Bottom, char* mFileName );
	IconView( int Left, int Bottom, char* mFileName );
	~IconView();
	void Initialize();
	
	void	set_file			( char* FileName );
	void	calc_margins		( );
	void	read_from_jpeg_file	( );
	void	read_from_file     	( );

	virtual void load_resources( );
	
	virtual void  set_position	( int Left, int Right, int Top, int Bottom );
	void  	move_to( int mLeft, int mBottom );
	
	void center_vertical  ( BOOL on )	{ if (on) style |= CENTER_VERTICAL;	else style &= ~CENTER_VERTICAL;	};
	void center_horizontal( BOOL on )	{ if (on) style |= CENTER_HORIZONTAL; else style &= ~CENTER_HORIZONTAL;	};
		
	virtual int   	draw   (	);	
	virtual int		onClick(	);

protected:
	char*		Filename;
	BOOL		file_loaded;
	
private:
	word		style;
	VGImage 	image;
	struct image_info ImageInfo;

	int			left_margin;
	int			bottom_margin;
	int			font_height;
};

 
#endif
