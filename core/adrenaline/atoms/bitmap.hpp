#ifndef _BITMAP_H_
#define _BITMAP_H_

// Note: ALL Bitmaps will have 32 bits per pixel!
//     & there will be no "Mapped" bitmaps (ie. no color maps to pixels 
//			that was an old method windows used to save memory.  It's 2014)
typedef struct stSize {
	int width;
	int height;
} sSize;

typedef struct tagBITMAP {
  long   Type;
  long   Width;
  long   Height;
  long   WidthBytes;
  word   Planes;
  word   BitsPixel;
  long*  Bits;
} BITMAP;

#define LPBITMAP (BITMAP*)
#define LPCTSTR  char const*


class Bitmap //: public CGdiObject
{
public:
	Bitmap();
	void CreateBitmap			( int nWidth, int nHeight, const void* lpBits );

	VGImage createImageFromJpeg ( const char *filename, struct image_info* II );
	VGImage createImageFromPNG  ( const char *filename, struct image_info* II );
	VGImage* createImageFromBMP	( const char* filename 			);
	bool	LoadBitmap			( LPCTSTR lpszResourceName  	);

	int		GetBitmapInfo		( BITMAP* pBitMap 				);
	byte*	GetBitmapBits		( long dwCount, void* lpBits 	);


	long	SetBitmapBits		( long dwCount, const void* lpBits 	);
	sSize 	SetBitmapDimension	( int nWidth,  int nHeight 			);
	sSize 	GetBitmapDimension 	( 									);
	
private:
	VGImage		image;
	BITMAP		bm;
};

#endif
