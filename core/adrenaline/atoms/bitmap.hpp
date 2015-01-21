#ifndef _BITMAP_H_
#define _BITMAP_H_

/* Make this file as stand alone as possible.  However it will need 
		libpng, lib jpg, and openVG (could do without)
		

 Note: ALL Bitmaps will have 32 bits per pixel!
     & there will be no "Mapped" bitmaps (ie. no color maps to pixels 
		that was an old method windows used to save memory.  It's 2014)
*/

typedef struct stSize {
	int width;
	int height;
} sSize;

typedef struct tagBITMAP {
  long   HeaderSize;
  long   Width;
  long   Height;
//  long   WidthBytes;
  uint16_t   Planes;
  uint16_t   BitsPixel;
  long	 Compression;
  long	 ImageSize;
  long*  Bits;
} BITMAP;


typedef struct tagBITMAPINFOHEADER { 
  long int biSize; 
  long int biWidth; 
  long int biHeight; 
  uint16_t biPlanes; 
  uint16_t biBitCount;
  long int biCompression; 
  long int biSizeImage; 
  long int biXPelsPerMeter; 
  long int biYPelsPerMeter; 
  long int biClrUsed; 
  long int biClrImportant; 
} BITMAPINFOHEADER; 

#define FXPT2DOT30 uint32_t
typedef struct tagCIEXYZ {
  FXPT2DOT30 ciexyzX;		// fixed point 2 bits integer and 30 bits fractional.
  FXPT2DOT30 ciexyzY;
  FXPT2DOT30 ciexyzZ;
} CIEXYZ;

typedef struct {
  uint32_t        bV4Size;
  int32_t         bV4Width;
  int32_t         bV4Height;
  uint16_t        bV4Planes;
  uint16_t        bV4BitCount;
  uint32_t        bV4V4Compression;
  uint32_t        bV4SizeImage;
  int32_t         bV4XPelsPerMeter;
  int32_t         bV4YPelsPerMeter;
  uint32_t        bV4ClrUsed;
  uint32_t        bV4ClrImportant;
  uint32_t        bV4RedMask;
  uint32_t        bV4GreenMask;
  uint32_t        bV4BlueMask;
  uint32_t        bV4AlphaMask;
  uint32_t        bV4CSType;
  //CIEXYZTRIPLE 	  bV4Endpoints;
  CIEXYZ 		ciexyzRed;
  CIEXYZ 		ciexyzGreen;
  CIEXYZ 		ciexyzBlue;  
  
  uint32_t        bV4GammaRed;
  uint32_t        bV4GammaGreen;
  uint32_t        bV4GammaBlue;
} BITMAPV4HEADER, *PBITMAPV4HEADER;


typedef struct {
  uint32_t        bV5Size;
  int32_t         bV5Width;
  int32_t         bV5Height;
  uint16_t        bV5Planes;
  uint16_t        bV5BitCount;
  uint32_t        bV5Compression;
  uint32_t        bV5SizeImage;
  int32_t         bV5XPelsPerMeter;
  int32_t         bV5YPelsPerMeter;
  uint32_t        bV5ClrUsed;
  uint32_t        bV5ClrImportant;
  uint32_t        bV5RedMask;
  uint32_t        bV5GreenMask;
  uint32_t        bV5BlueMask;
  uint32_t        bV5AlphaMask;
  uint32_t        bV5CSType;
  CIEXYZ 		ciexyzRed;
  CIEXYZ 		ciexyzGreen;
  CIEXYZ 		ciexyzBlue;  
  uint32_t        bV5GammaRed;
  uint32_t        bV5GammaGreen;
  uint32_t        bV5GammaBlue;
  uint32_t        bV5Intent;
  uint32_t        bV5ProfileData;
  uint32_t        bV5ProfileSize;
  uint32_t        bV5Reserved;
} BITMAPV5HEADER, *PBITMAPV5HEADER;

#define LPBITMAP (BITMAP*)
#define LPCTSTR  char const*

// SJT made to extract results from read JPG
// Can be reduced to class variales:
struct image_info
{
	unsigned int width;
	unsigned int height;
	unsigned int dstride;
	unsigned int dbpp;
	VGImage  image;	
};


class Bitmap 
{
public:
	Bitmap();
	void CreateBitmap			( int nWidth, int nHeight, const void* lpBits );

	VGImage createImageFromJpeg ( const char* filename, struct image_info* II );
	VGImage createImageFromPNG  ( const char* filename, struct image_info* II );
	VGImage* createImageFromBMP	( const char* filename 			);
	bool	LoadBitmap			( const char* mFileName  		);

	int		GetBitmapInfo		( BITMAP* pBitMap 				);
	uint8_t*	GetBitmapBits		( long dwCount, void* lpBits 	);

	long	SetBitmapBits		( long dwCount, const void* lpBits 	);
	sSize 	SetBitmapDimension	( int nWidth,  int nHeight 			);
	sSize 	GetBitmapDimension 	( 									);

	uint16_t	width;
	uint16_t	height;
	
private:
	VGImage		image;
	BITMAP		bm;
	
};

#endif
