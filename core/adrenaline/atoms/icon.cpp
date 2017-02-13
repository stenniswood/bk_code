/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <string.h>
#include <fontinfo.h>
#include <shapes.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "icon.hpp"


#define margin_percent 0.07
#define Debug 0


VGImage createImageFromJpeg(const char *filename, struct image_info* II);


IconView::IconView( int Left, int Right, int Top, int Bottom, char* mFileName )
:Control(Left, Right, Top, Bottom)
{
	Initialize();	
	set_file(mFileName);
}
IconView::IconView(  )
{
	Initialize();
	set_position( 0, 0, 0, 0 );
}
IconView::IconView( int Left, int Bottom )
{
	Initialize();
	set_position( Left, 0, 0, Bottom );
	// want to load resource here
	style |= WRAP_IMAGE_SIZE;
}

void IconView::set_file( const char* mFileName )
{
	if (mFileName==NULL) return;

	Filename = new char[strlen(mFileName)];
	strcpy (Filename, mFileName );
	if (Debug) printf("IconView: Filename=%s\n", Filename);
}

void IconView::Initialize()
{
	Control::Initialize();
	
	style     		= 0;
	file_loaded 	= false;

	ImageInfo.width   = 0;
	ImageInfo.height  = 0;
	ImageInfo.dstride = 0;
	ImageInfo.dbpp    = 0;
	ImageInfo.image   = 0;

	text_color		 = 0xFF9f9f0f;
	border_color 	 = 0xFFffffff;
	background_color = 0xFf202020;
}

IconView::~IconView()
{
	vgDestroyImage(image);
	delete Filename;
}

void IconView::set_position ( int Left, int Right, int Top, int Bottom )
{
	Control::set_position(Left, Right, Top, Bottom);
	calc_margins();
}

void  IconView::move_to( int mLeft, int mBottom )
{
	left   = mLeft;
	bottom = mBottom;
}

void IconView::calc_margins( )
{
	left_margin   = width  - ImageInfo.width;
	bottom_margin = height - ImageInfo.height;
	if (left_margin  <0) left_margin   = 0;
	if (bottom_margin<0) bottom_margin = 0;
	
	start_left   = left+left_margin;
	start_bottom = bottom+bottom_margin;	
	
	unsigned int w = width;
	unsigned int h = height;
	min_w = std::min(ImageInfo.width, w);
	min_h = std::min(ImageInfo.height, h);	
}

void IconView::read_from_jpeg_file ( )
{
}

void convertToUpper(char *str)
{
    char *p = str;
    while( (*p=toupper(*p)) )  { p++; };
}

/* Parse based on the file extension.  And then read. */
/* Note the file extension can only be 15 chars long  */
void IconView::read_from_file( )
{
	if (Filename==NULL) return;
	
	char* extension = strrchr( Filename, '.' )+1;
	char  CapExtension[15];

    strcpy( CapExtension, extension );
    convertToUpper( CapExtension );
	if (Debug) 
		printf("IconView::read_from_file(): %s; Extension=%s; width=%d height=%d; \n", 
					Filename, CapExtension, ImageInfo.width, ImageInfo.height );

	if (strcmp(CapExtension, "JPG")==0)	
		image = createImageFromJpeg( Filename, &ImageInfo );
/*	else if (strcmp(CapExtension, "PNG")==0)
		image = createImageFromPNG ( Filename, &ImageInfo );
	else if (strcmp(extension, "BMP")==0)
		image = createImageFromPNG ( Filename, &ImageInfo ); */

	file_loaded = true;
	if (style & WRAP_IMAGE_SIZE)
	{
		set_width_height(ImageInfo.width, ImageInfo.height);
	}
	file_loaded = true;
	calc_margins();
}

void IconView::load_resource( )
{
	read_from_file( );
	if (Debug) printf("IconView:load_resource(): FN=%s;  image=%d\n", Filename, image );	
}

void IconView::set_image( VGImage* mImage, struct image_info* mImageInfo )
{
	image     = *mImage;
	ImageInfo = *mImageInfo;
}

/********************************************************************
 Comment on RoundRect & TextMid!!!!
	RoundRect Uses width / height as 2nd and 3rd parameters.
	Text characters come out the color of the Fill, not stroke!!
*********************************************************************/
int IconView::draw()
{
	Control::draw();		// Draw border if selected.

	if (image != 0) {
		vgSetPixels(start_left, start_bottom, image, 0, 0, min_w, min_h);
		return 1;
	}
	return 0;
}

int	IconView::onClick(int x, int y, bool mouse_is_down)
{	
	draw();
	return -1;
}

// createImageFromJpeg decompresses a JPEG image to the standard image format
// source: https://github.com/ileben/ShivaVG/blob/master/examples/test_image.c
VGImage createImageFromJpeg(const char *filename, struct image_info* II)
{
	FILE   *infile;
	struct jpeg_decompress_struct jdc;
	struct jpeg_error_mgr 		  jerr;
	JSAMPARRAY   buffer;
	unsigned int bstride;
	unsigned int bbpp;

	if (Debug) printf("createImageFromJpeg()\n");

	VGImage img;
	VGubyte *data;

	VGubyte *brow;
	VGubyte *drow;
	unsigned int x;
	unsigned int lilEndianTest = 1;
	VGImageFormat rgbaFormat;

	// Check for endianness
	if (((unsigned char *)&lilEndianTest)[0] == 1)
		rgbaFormat = VG_sABGR_8888;
	else
		rgbaFormat = VG_sRGBA_8888;

	// Try to open image file
	infile = fopen(filename, "rb");
	if (infile == NULL) {
		if (Debug) printf("Failed opening '%s' for reading!\n", filename);
		return VG_INVALID_HANDLE;
	}
	// Setup default error handling
	jdc.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&jdc);

	// Set input file
	jpeg_stdio_src(&jdc, infile);

	// Read header and start
	jpeg_read_header(&jdc, TRUE);
	jpeg_start_decompress(&jdc);
	II->width = jdc.output_width;
	II->height = jdc.output_height;

	// Allocate buffer using jpeg allocator
	bbpp = jdc.output_components;
	bstride = II->width * bbpp;
	buffer = (*jdc.mem->alloc_sarray)
	    ((j_common_ptr) & jdc, JPOOL_IMAGE, bstride, 1);

	// Allocate image data buffer
	II->dbpp = 4;
	II->dstride = II->width * II->dbpp;
	data = (VGubyte *) malloc(II->dstride * II->height);

	// Iterate until all scanlines processed
	while (jdc.output_scanline < II->height) {

		// Read scanline into buffer
		jpeg_read_scanlines(&jdc, buffer, 1);
		drow = data + (II->height - jdc.output_scanline) * II->dstride;
		brow = buffer[0];
		// Expand to RGBA
		for (x = 0; x < II->width; ++x, drow += II->dbpp, brow += bbpp) {
			switch (bbpp) {
			case 4:
				drow[0] = brow[0];
				drow[1] = brow[1];
				drow[2] = brow[2];
				drow[3] = brow[3];
				break;
			case 3:
				drow[0] = brow[0];
				drow[1] = brow[1];
				drow[2] = brow[2];
				drow[3] = 255;
				break;
			}
		}
	}

	if (Debug) printf("createImageFromJpeg() 2\n");
	// Create VG image
	img = vgCreateImage(rgbaFormat, II->width, II->height, VG_IMAGE_QUALITY_BETTER);
	if (Debug) printf("read jpeg, %d w=%d; h=%d\n", img, II->width, II->height);
	vgImageSubData(img, data, II->dstride, rgbaFormat, 0, 0, II->width, II->height);
	if (Debug) printf("read jpeg, %d %p w=%d; h=%d\n", img, data, II->width, II->height);

	// Cleanup
	jpeg_destroy_decompress(&jdc);
	fclose(infile);
	free(data);

	if (Debug) printf("createImageFromJpeg() done\n");
	return img;
}
