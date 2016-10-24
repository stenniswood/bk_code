// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _DIB_H__
#define _DIB_H__

#include <string>
using namespace std;

#define WORD 	unsigned short
#define DWORD 	unsigned long
#define BOOL  	unsigned char
#define BYTE  	unsigned char
//#define RGBQUAD unsigned long

//provides general information about the file
typedef struct tagBITMAPFILEHEADER {
  WORD	  bfType;
  DWORD   bfSize;
  WORD	  bfReserved1;
  WORD	  bfReserved2;
  DWORD   bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

//Bitmap information header
//provides information specific to the image data
typedef struct tagBITMAPINFOHEADER{
  DWORD  biSize;
  long int   biWidth;
  long   biHeight;
  WORD   biPlanes;
  WORD   biBitCount;
  DWORD  biCompression;
  DWORD  biSizeImage;
  long   biXPelsPerMeter;
  long   biYPelsPerMeter;
  DWORD  biClrUsed;
  DWORD  biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

//Colour palette
typedef struct tagRGBQUAD {
  BYTE	rgbBlue;
  BYTE	rgbGreen;
  BYTE	rgbRed;
  BYTE	rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD          bmiColors[1];
} BITMAPINFO, *PBITMAPINFO;


/* DIB constants */
#define PALVERSION   0x300

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
DWORD NColors(WORD mBitCount);
WORD NBits(DWORD colors);


class CDib  
{
public:
	bool IsBottomUp ();
	CDib();
	CDib(string mFileName);
	CDib(const CDib& src);
	CDib(int width, int height, int nColors, WORD mode);
	virtual ~CDib();

/*	BOOL PaintDIB(HDC     hDC,
				  LPRECT  lpDCRect = NULL,
				  LPRECT  lpDIBRect = NULL); */

	const char*		GetBits() const;
	char*			GetBits();
	//HDIB			GetHDIB();

	//HDIB			ReadDIBFile(CFile& file);
	void			ReadDIBFile(FILE& file);
	void			InitDIBData();

	BOOL			CreateDIBPalette();
	DWORD			Width() const;		// Width in pixels.
	int				Height() const;		// Height in pixels.
	DWORD			SizeImage() const;	// Size of the DIB image bits in bytes.
	DWORD			SizeDib() const;		// Memory size of DIB in bytes.
	WORD			NumColors();
	DWORD			PaletteSize();

	// inlines:
	// Change top/bottom. FlipY  :  0 => Toggle;  1 = BottomUp;  2 = TopUp;
	void			FlipY   (short mThreeState = 0);
	int				FlippedY(int y) const;
	//CSize			Size() const;
	int				ScanBytes(long w, int bpp) const;
	inline int		ScanBytes() const;
	int				BitsPixel() const;
	int				Pitch() const;
	DWORD			Compression() const;

	DWORD			StartScan() const;
	DWORD			NumScans() const;

	//CPalette*		m_palDIB;
	WORD			Usage() const;
	int				XOffset(WORD x) const;	
	int				YOffset(WORD y) const;
	inline BYTE*	PixelPtr(WORD x, WORD y);
	//inline CPoint	IndexToPixel(DWORD index);

	RGBQUAD			GetColor(int entry) const;
    void			SetColor(int entry, const RGBQUAD& color);
//	void			SetColor(int entry, const COLORREF& color);
    int				FindColor(const RGBQUAD& color);
    int				MapColor(const RGBQUAD& fromColor, const RGBQUAD& toColor, bool doAll = false);

protected:
	void			InfoFromHeader(const BITMAPINFOHEADER& infoHeader);
	void			InfoFromHandle();
	char*			m_Bits;

private:
	bool		m_BottomUp;	    // Bottom Up Dib.
	//HDIB		m_hDIB;
	//CSize		m_sizeDoc;
	bool		ShouldDelete;
	char*		m_lpDIB;

	WORD		m_Mode;
	RGBQUAD*	m_lpColors;
	int			m_NumClrs;
};



//
//  Flip the DIB.
//
inline void CDib::FlipY( short mThreeState )
{
   switch (mThreeState)
   {
	   case 0 : m_BottomUp = !m_BottomUp;  break;
	   case 1 : m_BottomUp = true;		   break;
	   case 2 : m_BottomUp = false;		   break;
	   default : ;
   }
}

//
// Return the coordinate of y if the direction of the y-axis were reversed.
//
inline int CDib::FlippedY(int y) const
{
  if (m_BottomUp)
	  return int(((BITMAPINFO*)m_lpDIB)->bmiHeader.biHeight /*- 1*/ - y);  
							// biHeight must not be inclusive.  
  return y;
}

//
// Return the dimension of the DIB.
//
/*inline CSize CDib::Size() const
{
  return CSize(Width(), Height());
}*/

//
// Return the number of bytes used to store a scanline for the DIB.
// Rounded up to the nearest 32-bit boundary.
//
inline int CDib::ScanBytes(long w, int bpp) const
{
  return (int)((w*bpp+31)&(~31))/8;
}

inline int CDib::ScanBytes() const
{

  return (int)((Width()*BitsPixel()+31)&(~31))/8;
}

//
// Return the number of bits used to store a pixel for the DIB.
//
inline int CDib::BitsPixel() const
{
  return ((BITMAPINFO*)m_lpDIB)->bmiHeader.biBitCount;
}

//
// Size of scan in bytes =
//   Pixel Width * bits per pixel rounded up to a uint32 boundary
//
inline int CDib::Pitch() const
{
  return ScanBytes(Width(), BitsPixel());
}

//
// Return type of compression for bottom-up DIBs.
//
inline DWORD CDib::Compression() const
{
  return ((BITMAPINFO*)m_lpDIB)->bmiHeader.biCompression;
}

//
// Return number of bytes used to store the image.
//
inline DWORD CDib::SizeImage() const
{
  return ((BITMAPINFO*)m_lpDIB)->bmiHeader.biSizeImage;
}

//
// Return number of colors used by the DIB.
//
/*inline long CDib::NumColors() const
{
  return NumClrs;
}*/

//
// Return the starting scan line.
// Always 0 because all DIBs are normalized.
//
inline DWORD CDib::StartScan() const
{
  return 0;
}

//
// Return number of scan lines.
// Always same as height of the DIB.
//
inline DWORD CDib::NumScans() const
{
  return Height();
}

//
// Return number of colors times the size of each entry in the table,
// whether it is an RGB color table or palette index table.
//

/*inline int32 CDib::SizeColors() const
{
  return Mode == DIB_RGB_COLORS ?
           (long)NumColors() * sizeof(RGBQUAD) :  // RGB color table
           (long)NumColors() * sizeof(uint16);    // Palette index color table
}*/

//
// Size of dib is measured as the end of the bits minus the start of the block
//
inline DWORD CDib::SizeDib() const
{
  return (m_Bits + SizeImage()) - m_lpDIB;
}

//
// Return whether the DIB has palette color entries or RGB color entries.
//
inline WORD CDib::Usage() const
{
  return m_Mode;
}

//
// Return the byte offset from the start of the scan line to the xth pixel.
//
inline int CDib::XOffset(WORD x) const
{
  return int((DWORD)x * BitsPixel() / 8);
}

//
// Return the starting position of the scan line.
//
inline int CDib::YOffset(WORD y) const
{
  return int((DWORD)Pitch() * y);
}

//
// Return the byte of where the pixel is located.
//
/*inline BYTE* CDib::PixelPtr(WORD x, WORD y)
{
  return (BYTE*)m_Bits + XOffset(x) + YOffset(y);
}*/

//
// Return the pixel where the index is located.
//
/*inline CPoint CDib::IndexToPixel(DWORD index)
{
  CPoint Coord;
  Coord.x = index % ScanBytes();
  Coord.y = index / ScanBytes();
  return Coord;
}*/

#endif 