
// Note: ALL Bitmaps will have 32 bits per pixel!
//     & there will be no "Mapped" bitmaps (ie. no color maps to pixels 
//			that was an old method windows used to save memory.  It's 2014)

typedef struct tagBITMAP {
  long   bmType;
  long   bmWidth;
  long   bmHeight;
  long   bmWidthBytes;
  word   bmPlanes;
  word   bmBitsPixel;
  long*  bmBits;
} BITMAP;


class CBitmap : public CGdiObject
{
public:
	CBitmap();
	void CreateBitmap			( int nWidth, int nHeight, const void* lpBits );
	void CreateBitmapIndirect	( LPBITMAP lpBitmap 				 	);
	//void CreateCompatibleBitmap ( CDC* pDC,		int nWidth, int nHeight );								  

	VGImage createImageFromJpeg ( const char *filename, struct image_info* II );
	VGImage createImageFromPNG  ( const char *filename, struct image_info* II );

	int		GetBitmap			( BITMAP* pBitMap 				);
	byte*	GetBitmapBits		( long dwCount, LPVOID lpBits 	);

	CSize 	GetBitmapDimension 	( 								);
	bool	LoadBitmap			( LPCTSTR lpszResourceName  	);
	bool	LoadBitmap			( long nIDResource  			);
								  
	BOOL LoadOEMBitmap			( long nIDBitmap 				);

	long SetBitmapBits			( long dwCount, const void* lpBits );
	CSize SetBitmapDimension	( int nWidth,  int nHeight );

private:
	Image		image;
	
	// Deprecated functions:
	//void CreateDiscardableBitmap( CDC* pDC,		int nWidth,  int nHeight );
	//static CBitmap* FromHandle  ( HBITMAP hBitmap  );
}