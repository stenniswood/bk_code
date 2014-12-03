
// Note: ALL Bitmaps will have 32 bits per pixel!
//     & there will be no "Mapped" bitmaps (ie. no color maps to pixels 
//			that was an old method windows used to save memory.  It's 2014)

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
#define LPCTSTR  (char const*)

class CBitmap : public CGdiObject
{
public:
	CBitmap();
	void CreateBitmap			( int nWidth, int nHeight, const void* lpBits );
	void CreateBitmapIndirect	( LPBITMAP lpBitmap 				 		  );
	//void CreateCompatibleBitmap( CDC* pDC,		int nWidth, int nHeight );

	VGImage createImageFromJpeg ( const char *filename, struct image_info* II );
	VGImage createImageFromPNG  ( const char *filename, struct image_info* II );
	Image*  createImageFromBMP	( const char* filename 			);
	bool	LoadBitmap			( LPCTSTR lpszResourceName  	);

	int		GetBitmapInfo		( BITMAP* pBitMap 				);
	byte*	GetBitmapBits		( long dwCount, LPVOID lpBits 	);

	long	SetBitmapBits		( long dwCount, const void* lpBits 	);
	CSize 	SetBitmapDimension	( int nWidth,  int nHeight 			);
	CSize 	GetBitmapDimension 	( 								);
	
private:
	Image		image;
	BITMAP		bm;
	
};

