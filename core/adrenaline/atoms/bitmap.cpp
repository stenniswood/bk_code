/*******************************************************
* Bitmap()
* 	Loads from file.  .PNG, .JPG, .BMP
*	
*
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include <jpeglib.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <fontinfo.h>
#include <shapes.h>

//#include "control.hpp"
//#include "button.hpp"
//#include "display.h"
//#include "icon.hpp"
#include "bitmap.hpp"


using namespace std;


Bitmap::Bitmap()
{

}

// Initializes the object with a device-dependent memory bitmap that has a specified width, height, and bit pattern.
void Bitmap::CreateBitmap( int nWidth, int nHeight, const void* lpBits )
{

}

//Returns a pointer to a Bitmap object when given a handle to a Windows HBITMAP bitmap.
int Bitmap::GetBitmapInfo( BITMAP* pBitMap )
{
	return 0;
}	

//Fills a BITMAP structure with information about the bitmap.
//The number of bytes copied to the buffer if the method was successful; otherwise 0.
uint8_t* Bitmap::GetBitmapBits( long dwCount, void* lpBits )
{
	return NULL;
}
	
long Bitmap::SetBitmapBits	( long dwCount, const void* lpBits 	)
{
	return 0;
}

//Returns the width and height of the bitmap. The height and width are assumed to have been set previously by the SetBitmapDimension member function.
sSize Bitmap::GetBitmapDimension( )
{
	sSize s;
	return s;
}	

sSize Bitmap::SetBitmapDimension( int nWidth,  int nHeight )
{
	sSize s;
	return s;
}


bool	Bitmap::LoadBitmap( const char* mFileName )
{
	struct image_info ii;
	char* extension = (char*)strchr( mFileName, '.' ) + 1;
	if (strcmp(extension, "JPG")==0)
		createImageFromJpeg( mFileName, &ii );
	else if (strcmp(extension, "PNG")==0)
		createImageFromPNG( mFileName, &ii );
	else if (strcmp(extension, "BMP")==0)
		createImageFromBMP(mFileName);
	return true;
}

// createImageFromJpeg decompresses a JPEG image to the standard image format
// source: https://github.com/ileben/ShivaVG/blob/master/examples/test_image.c
VGImage Bitmap::createImageFromJpeg( const char *filename, struct image_info* II )
{
	FILE  *infile;
	struct jpeg_decompress_struct jdc;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY buffer;
	unsigned int bstride;
	unsigned int bbpp;

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
		printf("Failed opening '%s' for reading!\n", filename);
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

	// Create VG image
	img = vgCreateImage(rgbaFormat, II->width, II->height, VG_IMAGE_QUALITY_BETTER);
	vgImageSubData(img, data, II->dstride, rgbaFormat, 0, 0, II->width, II->height);

	// Cleanup
	jpeg_destroy_decompress(&jdc);
	fclose(infile);
	free(data);
	return img;
}

VGImage Bitmap::createImageFromPNG  ( const char *filename, struct image_info* II )
{
	return 0;
}




#define readInt(fd) 

VGImage* Bitmap::createImageFromBMP(const char* filename) 
{
	string errMsg;
	//char text[256];
	char buffer[2];
	//int  FileSize;
	
	FILE* input = fopen(filename, "rb" );
	if (input==NULL)
	{ errMsg = "Could not find file";	throw errMsg;	};

	// "BM"
	int bytes_read = fread( buffer, 1, 2, input );	
	assert((buffer[0] == 'B') && (buffer[1] == 'M'));	//|| !"Not a bitmap file"
	
	// Ignore 8 bytes.	Ignore file size and 4 reserved bytes
	bytes_read = fread( buffer, 1, 8, input );	// skip 8 bytes 

	// Read Offset to the Bitmap data:
	long int dataOffset;  		
	bytes_read = fread( &dataOffset, 1, 4, input );	// 4 bytes

	// READ HEADER : 
	long int headerSize;
	bytes_read = fread( &headerSize, 1, 4, input );
	
	int width;
	int height;
	short planes;
	short bpp;
	//short tmp;
	BITMAPINFOHEADER bih ;
	BITMAPV4HEADER   bv4h;
//	BITMAPV5HEADER   bv5h;
	switch(headerSize) {
		case 12:	/* Windows 2.x, OS/2 V1	*/
			fread( &width,  1, 2, input );
			fread( &height, 1, 2, input );
			fread( &planes,  1, 2, input );	// ignore 2.  Num Color planes.
			fread( &bpp,    1, 2, input );	// bpp
			break;

		case 40:	/* Windows NT, 3.x  :   	BITMAPINFOHEADER */
			//Adds 16bpp and 32bpp formats. Adds RLE compression.
			bytes_read = fread( &bih,    1, sizeof(BITMAPINFOHEADER), input );			
			assert(bytes_read == 40  || !"Image is compressed");
			/*fread( &width,  1, 2, input );
			fread( &height, 1, 2, input );
			fread( &planes,  1, 2, input );	// ignore 2.  Num Color planes.
			fread( &bpp,    1, 2, input );	// bpp			
			fread( &tmp,    1, 2, input );	//  
			assert(tmp == 0  || !"Image is compressed");
			*/
			break;
		case 52: // Adds RGB bit masks.	Undocumented info.
			assert(!"Can't load Adobe Photoshop bitmaps");
			break;			
		case 56: //	Adds alpha channel bit mask.    	BITMAPV3INFOHEADER
			assert(!"Can't load Adobe Photoshop bitmaps");
			break;
		case 64:  // OS/2 V2
			// Adds halftoning. Adds RLE and Huffman 1D compression.
			assert(!"Can't load OS/2 V2 bitmaps");
			break;
		case 108: //Windows NT 4 and 95					BITMAPV4HEADER			
			bytes_read = fread( &bv4h,    1, sizeof(BITMAPV4HEADER), input );			
			assert(bytes_read == 108  || !"Image header not read");
			//	Adds color space type and gamma correction
			// assert(!"Can't load Windows NT4 & 95 bitmaps");
			printf(" Windows NT4 & 95 bitmaps - may have problems. (untested)");
			break;
		case 124: //Windows V5							BITMAPV5HEADER
			bytes_read = fread( &bv4h,    1, sizeof(BITMAPV5HEADER), input );			
			assert(bytes_read == 124  || !"Image header not read");
			printf(" Windows NT5 & 98 bitmaps - may have problems. (untested)");			
			// Adds ICC color profiles
			// assert(!"Can't load Windows NT5 & 98 bitmaps");
			break;
		default:
			assert(!"Unknown bitmap format");
	}

	//Read the data
	//int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
	//int size = bytesPerRow * height;
	//array<char> pixels(new char[size]);
		
	//input.seekg(dataOffset, ios_base::beg);
	// fseek( ... );		NEED THIS LINE!
	//input.read( pixels.get(), size );
	//fread( pixels.get(), 1, size, input );

	//Get the data into the right format
	//auto_array<char> pixels2(new char[width * height * 3]);
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			for(int c = 0; c < 3; c++) {
				//pixels2[3 * (width * y + x) + c] =
				//	pixels[bytesPerRow * y + 3 * x + (2 - c)];
			}
		}
	}
	
	fclose( input );
	//input.close();
	//return new Image(pixels2.release(), width, height);
	return 0;
}

