/*******************************************************
* OpenVG Image wrapper class
* Bitmap()
*
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "icon.hpp"
#include "bitmap.hpp"


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
}	

//Fills a BITMAP structure with information about the bitmap.
//The number of bytes copied to the buffer if the method was successful; otherwise 0.
byte* Bitmap::GetBitmapBits( long dwCount, void* lpBits )
{
}
	
long Bitmap::SetBitmapBits	( long dwCount, const void* lpBits 	)
{
}

VGImage Bitmap::createImageFromPNG  ( const char *filename, struct image_info* II )
{
}

bool	Bitmap::LoadBitmap			( LPCTSTR lpszResourceName  	)
{
}

//Returns the width and height of the bitmap. The height and width are assumed to have been set previously by the SetBitmapDimension member function.
sSize Bitmap::GetBitmapDimension( )
{
}	

sSize Bitmap::SetBitmapDimension( int nWidth,  int nHeight )
{
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


using namespace std;

#define readInt(fd) 

VGImage* Bitmap::createImageFromBMP(const char* filename) 
{
	char text[256];
	int  FileSize;
	FILE* input = fopen(filename, 'r' );
	//input.open(filename, ifstream::binary);
	//assert( !input.fail() || !"Could not find file" );
	char buffer[2];
 	//input.read(buffer, 2);
	int bytes_read = fread( buffer, 1, 2, input );
	
	assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Not a bitmap file");
	//input.ignore(8);
	bytes_read = fread( buffer, 1, 8, input );	// skip 8 bytes 
	int dataOffset;  // = read(input);
	bytes_read = fread( &dataOffset, 1, 1, input );

	// Read the header : 
	int headerSize;
	//int headerSize = readInt(input);
	bytes_read = fread( &headerSize, 1, 1, input );
	
	int width;
	int height;
	short tmp;
	switch(headerSize) {
		case 40:
			//V3
			fread( &width,  1, sizeof(int), input );
			fread( &height, 1, sizeof(int), input );
			//width  = readInt(input);
			//height = readInt(input);
			//input.ignore(2);
			fread( buffer, 1, 2, input );
			fread( &tmp,    1, 2, input );
			//assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			assert(tmp == 24 || !"Image is not 24 bits per pixel");			
			fread( &tmp,    1, 2, input );
			//assert(readShort(input) == 0  || !"Image is compressed");
			assert(tmp == 0  || !"Image is compressed");
			break;
		case 12:
			//OS/2 V1
			fread( &width,  1, sizeof(int), input );
			fread( &height, 1, sizeof(int), input );
			//width  = readShort(input);
			//height = readShort(input);
			//input.ignore(2);
			fread( buffer, 1, 2, input );
			fread( &tmp,    1, 2, input );			
			//assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			assert(tmp == 24 || !"Image is not 24 bits per pixel");
			break;
		case 64:
			//OS/2 V2
			assert(!"Can't load OS/2 V2 bitmaps");
			break;
		case 108:
			//Windows V4
			assert(!"Can't load Windows V4 bitmaps");
			break;
		case 124:
			//Windows V5
			assert(!"Can't load Windows V5 bitmaps");
			break;
		default:
			assert(!"Unknown bitmap format");
	}

	//Read the data
	int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
	int size = bytesPerRow * height;
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
}

