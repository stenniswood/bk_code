


 CBitmap::CreateBitmap(int nWidth,	  int nHeight,
					   const void* lpBits  )
// Initializes the object with a device-dependent memory bitmap that has a specified width, height, and bit pattern.
{
}

CBitmap::CreateBitmapIndirect( LPBITMAP lpBitmap 		)
{
}	

//Initializes the object with a bitmap with the width, height, and bit pattern (if one is specified) given in a BITMAP structure.
CBitmap::CreateCompatibleBitmap
{
}	

//Initializes the object with a bitmap so that it is compatible with a specified device.
CBitmap::CreateDiscardableBitmap
{
}	

//Initializes the object with a discardable bitmap that is compatible with a specified device.
CBitmap::FromHandle
{
}	

//Returns a pointer to a CBitmap object when given a handle to a Windows HBITMAP bitmap.

CBitmap::GetBitmap
{
}	

//Fills a BITMAP structure with information about the bitmap.
//The number of bytes copied to the buffer if the method was successful; otherwise 0.
CBitmap::GetBitmapBits
{
}	

//Copies the bits of the specified bitmap into the specified buffer.
CBitmap::GetBitmapDimension
{
}	

//Returns the width and height of the bitmap. The height and width are assumed to have been set previously by the SetBitmapDimension member function.
CBitmap::LoadBitmap
{
}	

//Initializes the object by loading a named bitmap resource from the application's executable file and attaching the bitmap to the object.
CBitmap::LoadMappedBitmap
{
}	

//Loads a bitmap and maps colors to current system colors.
CBitmap::LoadOEMBitmap
{
}	

//Initializes the object by loading a predefined Windows bitmap and attaching the bitmap to the object.
CBitmap::SetBitmapBits
{
}	

//Sets the bits of a bitmap to the specified bit values.
CBitmap::SetBitmapDimension 
{
}

VGImage CBitmap::createImageFromPNG( const char *filename, struct image_info* II )
{

}

// createImageFromJpeg decompresses a JPEG image to the standard image format
// source: https://github.com/ileben/ShivaVG/blob/master/examples/test_image.c
VGImage CBitmap::createImageFromJpeg( const char *filename, struct image_info* II )
{
	FILE *infile;
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
