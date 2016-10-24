// Dib.cpp: implementation of the CDib class.
//
//////////////////////////////////////////////////////////////////////
//  Source file for Device-Independent Bitmap (DIB) API.  Provides
//  the following functions:
//
//  PaintDIB()          - Painting routine for a DIB
//  CreateDIBPalette()  - Creates a palette from a DIB
//  FindDIBBits()       - Returns a pointer to the DIB bits
//  DIBWidth()          - Gets the width of the DIB
//  DIBHeight()         - Gets the height of the DIB
//  PaletteSize()       - Gets the size required to store the DIB's palette
//  DIBNumColors()      - Calculates the number of colors
//                        in the DIB's color table
//  CopyHandle()        - Makes a copy of the given global memory block
//
// This was taken from dibapi.c  from the Microsoft DibLook sample program.
// Converted into a C++ class by Stephen Tenniswood.
//

//#include "stdafx.h"
#include "Dib.h"
//#include "resource.h"
#include <stdio.h>
#include <errno.h>
//#include "utility.hpp"



/*
 * Dib Header Marker - used in writing DIBs to files
 */
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDib::CDib()
{
  m_BottomUp = false;
  m_Mode = 0;
  m_palDIB = NULL;
  ShouldDelete = true;
}

CDib::CDib(string mFileName)
{
  m_BottomUp = false;
  m_Mode = 0;
  m_palDIB = NULL;
  ShouldDelete = true;
  CFile DibFile;
  try {
    DibFile.Open(mFileName,CFile::modeRead);
    ReadDIBFile(DibFile);
	InitDIBData();
	DibFile.Close();
  }
  catch (...)
  {
	  AfxMessageBox("Could not open Dib file!");
  }
//  m_palDIB = new CPalette();
//  CreateDIBPalette();
}


CDib::CDib(const CDib& src)
{
// IsResHandle = false; from bc5.

  m_BottomUp = false;
  ASSERT(src.m_lpDIB != 0);

//  long size = ::GlobalSize(src.Handle);  
  DWORD size = src.SizeDib();
  m_hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE, size);
  if (m_hDIB == 0)
  {
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);

	  CString Msg;
	  Msg = "OUT OF MEMORY : GLOBALALLOC()\n";
	  Msg += ((char*)lpMsgBuf);
	  AfxMessageBox ( Msg );

	  throw ("Out of Memory!");
  }
  m_lpDIB = (char*) ::GlobalLock(m_hDIB);
//  char*  src_m_lpDIB = (char*) ::GlobalLock(src.m_hDIB);	// new
  memcpy (m_lpDIB, src.m_lpDIB, size);

  m_palDIB = NULL;
/*  Mask = (DWORD far*)(src.Mask ?
           (char HUGE*)m_lpDIB + ((char HUGE*)src.Mask - (char HUGE*)src.Info) : 0);*/

//  m_Colors = (TRgbQuad far*)((char*)Info + ((char HUGE*)src.Colors - (char HUGE*)src.Info));

//  m_Bits = m_lpDIB + (src.m_Bits - src.m_lpDIB);
   InfoFromHandle();
  //   InitDIBData();
}


//
// Given a BITMAPINFOHEADER, allocate a global buffer for Handle & init Info
//
void CDib::InfoFromHeader(const BITMAPINFOHEADER& infoHeader)
{
  // Allocate & lock down the actual single memory block used by this DIB
  //
  int maskAlloc = infoHeader.biCompression==BI_BITFIELDS ? 3*sizeof(DWORD) : 0;
  int colorAlloc = int(infoHeader.biClrUsed * sizeof(RGBQUAD));   // Color table size in mem
//  IsResHandle = false;
  DWORD size = sizeof(BITMAPINFOHEADER) + maskAlloc + colorAlloc +
                infoHeader.biSizeImage;
  m_hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE, size);
  if (!m_hDIB)
    throw("Out of memory for DIB!");

  // Setup Info ptr & copy over working infoHeader. Finish by using
  // InfoFromHandle to update all members from Info.
  //
  m_lpDIB = (char*)::GlobalLock(m_hDIB);
  ((LPBITMAPINFO)m_lpDIB)->bmiHeader = infoHeader;

  InfoFromHandle();
}

//
// Lock the global/res handle if needed & extract the pointers and cached info
// maintained as member variables from m_lpDIB (Info in bc5).
//
// Needs only Info setup, or if Info is 0 then Handle & IsResHandle setup
//
void CDib::InfoFromHandle()
{
  // Get the BITMAPINFO Info from the handle if needed
  //
  if (!m_lpDIB) {
//    if (IsResHandle)								////
//      m_lpDIB = (char*)::LockResource(m_hDIB);	//// This from bc5.
//    else											////
      m_lpDIB = (char*)::GlobalLock(m_hDIB);
  }
  // Fail if the header does not look OK
  //
  if (((LPBITMAPINFO)m_lpDIB)->bmiHeader.biSize != sizeof(BITMAPINFOHEADER)) {
    ::GlobalUnlock(m_hDIB);
    throw("Invalid DIB Handle");
  }

  // Determine if there is a bit mask (used by 16 & 32 bpp bmps) and where
  // the color table starts
  //

  if (((LPBITMAPINFO)m_lpDIB)->bmiHeader.biCompression == BI_BITFIELDS) {
    //Mask = (DWORD far*)m_lpDIB->bmiColors;                 // Not sure what to do with this, so... SJT
    m_lpColors = (RGBQUAD far*)((char far*)((LPBITMAPINFO)m_lpDIB)->bmiColors + 3 * sizeof(DWORD));
  }
  else {
    //Mask = 0;
    m_lpColors = (RGBQUAD far*)((LPBITMAPINFO)m_lpDIB)->bmiColors;
  }

  // Figure out the number of colors in the color table using biClrsUsed &
  // biBitCount.
  //
  m_NumClrs = ((LPBITMAPINFO)m_lpDIB)->bmiHeader.biClrUsed
              ? ((LPBITMAPINFO)m_lpDIB)->bmiHeader.biClrUsed
              : NColors(((LPBITMAPINFO)m_lpDIB)->bmiHeader.biBitCount);


  if (!m_NumClrs)
    m_lpColors = 0;

  if (((LPBITMAPINFO)m_lpDIB)->bmiHeader.biBitCount > 8)   // 64k colors or more:
      m_Bits = (char*) (char*)m_lpColors;
  else
	  m_Bits = (m_lpDIB + *(LPDWORD)m_lpDIB + PaletteSize()); 
//  m_Bits = (char*) (char*)m_lpColors + (int)m_NumClrs * sizeof(RGBQUAD);

  // Zero out Colors if there is no color table.
  //
  CreateDIBPalette();

  if (m_palDIB == NULL)
	{
		// we must be really low on memory
		//::GlobalFree((HGLOBAL) m_hDIB);
		//m_hDIB = NULL;
		throw ("No Memory for CPalette!");
		return;
	}
}


//
// Construct a Dib given dimensions and color depth
//
CDib::CDib(int width, int height, int nColors, WORD mode)
{
  //PRECONDITION(width && height && nColors);

  m_palDIB = NULL;
  BITMAPINFOHEADER      infoHeader;
  infoHeader.biSize       = sizeof(BITMAPINFOHEADER);
  infoHeader.biWidth      = width;
  infoHeader.biHeight     = height;
  infoHeader.biPlanes     = 1;
  infoHeader.biBitCount   = NBits(nColors);
  infoHeader.biCompression   = BI_RGB;
  DWORD bytes = ScanBytes(width, infoHeader.biBitCount);
  infoHeader.biSizeImage = bytes * height;
  infoHeader.biXPelsPerMeter = 0;
  infoHeader.biYPelsPerMeter = 0;
  infoHeader.biClrUsed       = nColors;
  infoHeader.biClrImportant  = 0;

  // Setup all members based on infoHeader
  //
  InfoFromHeader(infoHeader);

  // Initialize the color table--either Palette or RGB
  //
  m_Mode = mode;
  if (m_Mode == DIB_PAL_COLORS) {
    // Generate a 1:1 palette relative color table- it can later be translated
    // to RGB given a palette.
    //
    for (WORD i = 0; i < nColors; i++)
      ((WORD*)m_lpColors)[i] = i;
  }
  else {
    // Get the system palette and convert to RGB quad format.
    //
	HDC tmp_dc = ::GetDC(NULL);
    ::GetSystemPaletteEntries(tmp_dc, 0, nColors, (LPPALETTEENTRY)m_lpColors);
    for (int i = 0; i < nColors; i++) {
      Swap(m_lpColors[i].rgbRed, m_lpColors[i].rgbBlue);
      m_lpColors[i].rgbReserved = 0;
    }
	::ReleaseDC(NULL,tmp_dc);
  }
}

//
// Destruct a TDib by unlocking & freeing its global memory handle as required
//
CDib::~CDib()
{
  if (m_hDIB)
    ::GlobalUnlock(m_hDIB);
    if (ShouldDelete)
      ::GlobalFree(m_hDIB);
}

HDIB CDib::GetHDIB()
{
	if (m_hDIB)
	  ::GlobalUnlock(m_hDIB);
	return m_hDIB;
}


/*************************************************************************

  Function:  ReadDIBFile (CFile&)

   Purpose:  Reads in the specified DIB file into a global chunk of
			 memory.

   Returns:  A handle to a dib (hDIB) if successful.
			 NULL if an error occurs.

  Comments:  BITMAPFILEHEADER is stripped off of the DIB.  Everything
			 from the end of the BITMAPFILEHEADER structure on is
			 returned in the global memory handle.

*************************************************************************/

HDIB CDib::ReadDIBFile(CFile& file)
{
	BITMAPFILEHEADER bmfHeader;
	DWORD dwBitsSize;
	/*
	 * get length of DIB in bytes for use when reading
	 */
	dwBitsSize = file.GetLength();

	/*
	 * Go read the DIB file header and check if it's valid.
	 */
	if (file.Read((char*)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return NULL;

	if (bmfHeader.bfType != DIB_HEADER_MARKER)
		return NULL;

	/*
	 * Allocate memory for DIB
	 */
	m_hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (m_hDIB == 0)
	{
		return NULL;
	}
	m_lpDIB = (char*) ::GlobalLock((HGLOBAL) m_hDIB);

	/*
	 * Go read the bits.
	 */
	if (file.ReadHuge(m_lpDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) !=
		dwBitsSize - sizeof(BITMAPFILEHEADER) )
	{
		::GlobalUnlock((HGLOBAL) m_hDIB);
		::GlobalFree((HGLOBAL) m_hDIB);
		return NULL;
	}
	::GlobalUnlock((HGLOBAL) m_hDIB);
	return m_hDIB;
}


void CDib::InitDIBData()
{
	if (m_palDIB != NULL)
	{
		delete m_palDIB;
		m_palDIB = NULL;
	}
	if (m_hDIB == NULL)
	{
		return;
	}
	// Set up document size
	m_lpDIB = (char*) ::GlobalLock((HGLOBAL) m_hDIB);
	if (Width() > INT_MAX || Height() > INT_MAX)
	{
		::GlobalUnlock((HGLOBAL) m_hDIB);
		::GlobalFree((HGLOBAL) m_hDIB);
		m_hDIB = NULL;
		CString strMsg;
		strMsg = "Cannot load DIB, too big!";
		AfxMessageBox(strMsg, MB_ICONINFORMATION | MB_OK);
		return;
	}
	m_sizeDoc = CSize((int) Width(), (int) Height());
//::GlobalUnlock((HGLOBAL) m_hDIB);

	if (CreateDIBPalette() == NULL)
	{
		// DIB may not have a palette
		//delete m_palDIB;
		//m_palDIB = NULL;
		//return;
	}
	

  // Determine if there is a bit mask (used by 16 & 32 bpp bmps) and where
  // the color table starts
  //
  if (((LPBITMAPINFO)m_lpDIB)->bmiHeader.biCompression == BI_BITFIELDS) {
    //Mask = (DWORD far*)m_lpDIB->bmiColors;                 // Not sure what to do with this, so... SJT
    m_lpColors = (RGBQUAD far*)((char far*)((LPBITMAPINFO)m_lpDIB)->bmiColors + 3 * sizeof(DWORD));
  }
  else {
    //Mask = 0;
    m_lpColors = (RGBQUAD far*)((LPBITMAPINFO)m_lpDIB)->bmiColors;
  }

  // Figure out the number of colors in the color table using biClrsUsed &
  // biBitCount.
  //
  m_NumClrs = ((LPBITMAPINFO)m_lpDIB)->bmiHeader.biClrUsed
              ? ((LPBITMAPINFO)m_lpDIB)->bmiHeader.biClrUsed
              : NColors(((LPBITMAPINFO)m_lpDIB)->bmiHeader.biBitCount);

  // Zero out Colors if there is no color table.
  //
  if (!m_NumClrs)
    m_lpColors = 0;

  if (((LPBITMAPINFO)m_lpDIB)->bmiHeader.biBitCount > 8)   // 64k colors or more:
      m_Bits = (char*) (char*)m_lpColors; //+ (int)m_NumClrs * sizeof(RGBQUAD);
  else
	  m_Bits = (m_lpDIB + *(LPDWORD)m_lpDIB + PaletteSize()); 

  m_Mode = DIB_RGB_COLORS;
}															


/*************************************************************************
 *
 * PaintDIB()
 *
 * Parameters:
 *
 * HDC hDC          - DC to do output to
 *
 * LPRECT lpDCRect  - rectangle on DC to do output to
 *
 * HDIB hDIB        - handle to global memory with a DIB spec
 *                    in it followed by the DIB bits
 *
 * LPRECT lpDIBRect - rectangle of DIB to output into lpDCRect
 *
 * CPalette* pPal   - pointer to CPalette containing DIB's palette
 *
 * Return Value:
 *
 * BOOL             - TRUE if DIB was drawn, FALSE otherwise
 *
 * Description:
 *   Painting routine for a DIB.  Calls StretchDIBits() or
 *   SetDIBitsToDevice() to paint the DIB.  The DIB is
 *   output to the specified DC, at the coordinates given
 *   in lpDCRect.  The area of the DIB to be output is
 *   given by lpDIBRect.
 *
 ************************************************************************/
/*
BOOL CDib::PaintDIB(HDC     hDC,
					LPRECT  lpDCRect,
					LPRECT  lpDIBRect)
{
	BOOL     bSuccess=FALSE;      // Success/fail flag
	HPALETTE hPal=NULL;           // Our DIB's palette
	HPALETTE hOldPal=NULL;        // Previous palette

	// Check for valid DIB handle *
	if (m_hDIB == NULL)
		return FALSE;
	
	// Get the DIB's palette, then select it into DC
	if (m_palDIB != NULL)
	{
		hPal = (HPALETTE) m_palDIB->m_hObject;

		// Select as background since we have
		// already realized in forground if needed
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}

    int cw = Width();
	int ch = Height();

	// Make sure to use the stretching mode best for color pictures 
	::SetStretchBltMode(hDC, COLORONCOLOR);

	CRect rcDIB;
	CRect rcDest;
	if ((lpDCRect == NULL) || (lpDIBRect == NULL))
	{
		rcDIB.top = rcDIB.left = 0;
		rcDIB.right = cw;
		rcDIB.bottom = ch;
		rcDest = rcDIB;

		lpDCRect = rcDest;
		lpDIBRect = rcDIB;
	}
 	 // Determine whether to call StretchDIBits() or SetDIBitsToDevice() 
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
  	     (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
		bSuccess = ::SetDIBitsToDevice(hDC,                    // hDC
								   lpDCRect->left,             // DestX
								   lpDCRect->top,              // DestY
								   RECTWIDTH(lpDCRect),        // nDestWidth
								   RECTHEIGHT(lpDCRect),       // nDestHeight
								   lpDIBRect->left,            // SrcX
								   (int)Height() -
									  lpDIBRect->top -
									  RECTHEIGHT(lpDIBRect),   // SrcY
								   0,                          // nStartScan
								   (WORD)Height(),			   // nNumScans
								   m_Bits,		               // lpBits
								   (LPBITMAPINFO)m_lpDIB,      // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage
   else
   {
	  int DestHeight = RECTHEIGHT(lpDCRect);
	  int SrcY       = lpDIBRect->top;
	  int SrcHeight  = RECTHEIGHT(lpDIBRect);
	  if (m_BottomUp)
	  {
		  //DestHeight = - DestHeight;
		  SrcHeight = - SrcHeight;
		  // keep SrcHeight where it is:
		  SrcY       = lpDIBRect->bottom;
	  }

	  bSuccess = ::StretchDIBits(hDC,                          // hDC
							   lpDCRect->left,                 // DestX
							   lpDCRect->top,						   // DestY
							   RECTWIDTH(lpDCRect),            // nDestWidth
							   DestHeight,			           // nDestHeight
							   lpDIBRect->left,                // SrcX
							   SrcY,						   // SrcY
							   RECTWIDTH(lpDIBRect),           // wSrcWidth
							   SrcHeight,					   // wSrcHeight
							   m_Bits,	                       // lpBits
							   (LPBITMAPINFO)m_lpDIB,          // lpBitsInfo
							   DIB_RGB_COLORS,                 // wUsage
							   SRCCOPY);                       // dwROP

	}

//    DWORD Err = GetLastError();

   //::GlobalUnlock((HGLOBAL) hDIB);

	// Reselect old palette 
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}

   return bSuccess;
}*/

/*************************************************************************
 *
 * CreateDIBPalette()
 *
 * Parameter:
 *
 * HDIB hDIB        - specifies the DIB
 *
 * Return Value:
 *
 * HPALETTE         - specifies the palette
 *
 * Description:
 *
 * This function creates a palette from a DIB by allocating memory for the
 * logical palette, reading and storing the colors from the DIB's color table
 * into the logical palette, creating a palette from this logical palette,
 * and then returning the palette's handle. This allows the DIB to be
 * displayed using the best possible colors (important for DIBs with 256 or
 * more colors).
 *
 ************************************************************************/


BOOL CDib::CreateDIBPalette()
{
	LPLOGPALETTE lpPal;      // pointer to a logical palette
	HANDLE hLogPal;          // handle to a logical palette
	HPALETTE hPal = NULL;    // handle to a palette
	int i;                   // loop index
	WORD wNumColors;         // number of colors in color table
	LPBITMAPINFO lpbmi;      // pointer to BITMAPINFO structure (Win3.0)
	LPBITMAPCOREINFO lpbmc;  // pointer to BITMAPCOREINFO structure (old)
	BOOL bWinStyleDIB;       // flag which signifies whether this is a Win3.0 DIB
	BOOL bResult = FALSE;

	/* if handle to DIB is invalid, return FALSE */

	if (m_hDIB == NULL)
	  return FALSE;

    if (m_palDIB != NULL)
	{
		delete m_palDIB;
		m_palDIB = NULL;
	}

   /* get pointer to BITMAPINFO (Win 3.0) */
   lpbmi = (LPBITMAPINFO)m_lpDIB;

   /* get pointer to BITMAPCOREINFO (old 1.x) */
   lpbmc = (LPBITMAPCOREINFO)m_lpDIB;

   /* get the number of colors in the DIB */
   wNumColors = NumColors();

   if (wNumColors != 0)
   {
		/* allocate memory block for logical palette */
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
									+ sizeof(PALETTEENTRY)
									* wNumColors);

		/* if not enough memory, clean up and return NULL */
		if (hLogPal == 0)
		{
			::GlobalUnlock((HGLOBAL) m_hDIB);
			return FALSE;
		}

		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL) hLogPal);

		/* set version and number of palette entries */
		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD)wNumColors;

		/* is this a Win 3.0 DIB? */
		bWinStyleDIB = IS_WIN30_DIB(m_lpDIB);
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else
			{
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}

		/* create the palette and get handle to it */
  	    m_palDIB = new CPalette;

		bResult = m_palDIB->CreatePalette(lpPal);
		::GlobalUnlock((HGLOBAL) hLogPal);
		::GlobalFree((HGLOBAL) hLogPal);
	}

//	::GlobalUnlock((HGLOBAL) m_hDIB);

	return bResult;
}

/*************************************************************************
 *
 * GetBits()
 *
 * Parameter:
 *
 * char* lpDIB       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * char*            - pointer to the DIB bits
 *
 * Description:
 *
 * This function calculates the address of the DIB's bits and returns a
 * pointer to the DIB bits.
 *
 ************************************************************************/


char* CDib::GetBits()
{
	return m_Bits;
		// from MFC dibapi (lpbi + *(LPDWORD)lpbi + ::PaletteSize(lpbi));
}

const char* CDib::GetBits() const
{
	return m_Bits;
		// from MFC dibapi (lpbi + *(LPDWORD)lpbi + ::PaletteSize(lpbi));
}


/*************************************************************************
 *
 * Width()
 *
 * Parameter:
 *
 * char* m_lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * DWORD            - width of the DIB
 *
 * Description:
 *
 * This function gets the width of the DIB from the BITMAPINFOHEADER
 * width field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER
 * width field if it is an other-style DIB.
 *
 ************************************************************************/


DWORD CDib::Width() const
{
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether Win 3.0 and old) */

	lpbmi = (LPBITMAPINFOHEADER)m_lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)m_lpDIB;

	/* return the DIB width if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(m_lpDIB))
		return lpbmi->biWidth;
	else  /* it is an other-style DIB, so return its width */
		return (DWORD)lpbmc->bcWidth;
}


/*************************************************************************
 *
 * Height()
 *
 * Parameter:
 *
 * char* m_lpDIB       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * DWORD            - height of the DIB
 *
 * Description:
 *
 * This function gets the height of the DIB from the BITMAPINFOHEADER
 * height field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER
 * height field if it is an other-style DIB.
 *
 ************************************************************************/


int CDib::Height() const
{
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether old or Win 3.0 */

	lpbmi = (LPBITMAPINFOHEADER)m_lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)m_lpDIB;

	/* Start assuming it is an other-style DIB, so return its height */
	int height = (DWORD)lpbmc->bcHeight;

	/* return the DIB height if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(m_lpDIB))
		height = lpbmi->biHeight;

//	if (m_BottomUp)
//		return -height;
//	else
	return height;
}


/*************************************************************************
 *
 * PaletteSize()
 *
 * Parameter:
 *
 * char* lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * WORD             - size of the color palette of the DIB
 *
 * Description:
 *
 * This function gets the size required to store the DIB's palette by
 * multiplying the number of colors by the size of an RGBQUAD (for a
 * Windows 3.0-style DIB) or by the size of an RGBTRIPLE (for an other-
 * style DIB).
 *
 ************************************************************************/


DWORD CDib::PaletteSize() 
{
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (m_lpDIB))
   {
//	   if ((((BITMAPINFOHEADER*)m_lpDIB)->biClrUsed == 0) &&
//		   (((LPBITMAPINFOHEADER)m_lpDIB)->biBitCount == 16))
//		  return 12;   							    // Not 100% sure if this is always right but need
													// for at least one example BMP.
													// Why 12?  Don't know, but my eye tells me its 
													// needed.

	   return (DWORD)(NumColors() * sizeof(RGBQUAD));
   }
   else
	  return (DWORD)(NumColors() * sizeof(RGBTRIPLE));
}


/*************************************************************************
 *
 * NumColors()
 *
 * Parameter:
 *
 *
 * Return Value:
 *
 * WORD             - number of colors in the color table
 *
 * Description:
 *
 * This function calculates the number of colors in the DIB's color table
 * by finding the bits per pixel for the DIB (whether Win3.0 or other-style
 * DIB). If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256,
 * if 24, no colors in color table.
 *
 ************************************************************************/


WORD CDib::NumColors()
{
	WORD wBitCount;  // DIB bit count

	/*  If this is a Windows-style DIB, the number of colors in the
	 *  color table can be less than the number of bits per pixel
	 *  allows for (i.e. lpbi->biClrUsed can be set to some value).
	 *  If this is the case, return the appropriate value.
	 */

	if (IS_WIN30_DIB(m_lpDIB))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)m_lpDIB)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	/*  Calculate the number of colors in the color table based on
	 *  the number of bits per pixel for the DIB.
	 */
	if (IS_WIN30_DIB(m_lpDIB))
		wBitCount = ((LPBITMAPINFOHEADER)m_lpDIB)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)m_lpDIB)->bcBitCount;

	/* return number of colors based on bits per pixel */
	return NColors(wBitCount);
}


DWORD NColors(WORD mBitCount)
{
	/* return number of colors based on bits per pixel */
	switch (mBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;
		case 16:
			return 65535;

//		case 24:
//			return 16777216;

		default:
			return 0;
	}
}

//
// Return the number of bits required to represent a given number of colors
//
WORD NBits(DWORD colors)
{
  if (colors <= 2)
    return 1;
  if (colors <= 16)
    return 4;
  if (colors <= 256)
    return 8;
  if (colors <= 65536L)
    return 16;
  return 24;
}


//////////////////////////////////////////////////////////////////////////
//// Clipboard support

//---------------------------------------------------------------------
//
// Function:   CopyHandle (from SDK DibView sample clipbrd.c)
//
// Purpose:    Makes a copy of the given global memory block.  Returns
//             a handle to the new memory block (NULL on error).
//
//             Routine stolen verbatim out of ShowDIB.
//
// Parms:      h == Handle to global memory to duplicate.
//
// Returns:    Handle to new global memory block.
//
//---------------------------------------------------------------------
/*
HGLOBAL WINAPI CopyHandle (HGLOBAL h)
{
	if (h == NULL)
		return NULL;

	DWORD dwLen = ::GlobalSize((HGLOBAL) h);
	HGLOBAL hCopy = ::GlobalAlloc(GHND, dwLen);

	if (hCopy != NULL)
	{
		void* lpCopy = ::GlobalLock((HGLOBAL) hCopy);
		void* lp     = ::GlobalLock((HGLOBAL) h);
		memcpy(lpCopy, lp, dwLen);
		::GlobalUnlock(hCopy);
		::GlobalUnlock(h);
	}

	return hCopy;
}
*/


//
// Get, set, find and map the color table entries as colors (RGBQUADs)
//
RGBQUAD CDib::GetColor(int entry) const
{
  if (entry >= 0 && entry < m_NumClrs)
    if (m_Mode == DIB_RGB_COLORS)
      return m_lpColors[entry];

  RGBQUAD tmp;
  tmp.rgbBlue = 0;
  tmp.rgbGreen = 0;
  tmp.rgbRed   = 0;
  tmp.rgbReserved = 0;
  return tmp;
}

//
// Set the color table entry to a specified color.
//
void CDib::SetColor(int entry, const RGBQUAD& color)
{
  if (entry >= 0 && entry < m_NumClrs)
    if (m_Mode == DIB_RGB_COLORS)
      m_lpColors[entry] = color;
}

void CDib::SetColor(int entry, const COLORREF& color)
{
  RGBQUAD tmp;
  const BYTE* tmpCol = (BYTE*) &color;
  tmp.rgbReserved = *tmpCol++;
  tmp.rgbBlue     = *tmpCol++;
  tmp.rgbGreen    = *tmpCol++;
  tmp.rgbRed      = *tmpCol++;

  if (entry >= 0 && entry < m_NumClrs)
    if (m_Mode == DIB_RGB_COLORS)
      m_lpColors[entry] = tmp;
}

//
// Return the entry in the table that matches this color.
//
int CDib::FindColor(const RGBQUAD& color)
{
  for (int entry = 0; entry < m_NumClrs; entry++)
    if (*((DWORD*)&color) == *((DWORD*)(m_lpColors+entry)))  // Small data model requires this order
      return entry;
  return -1;
}

//
// Map first or all occurances of a given palette color to a new color
//
int
CDib::MapColor(const RGBQUAD& fromColor, const RGBQUAD& toColor, bool mapAll)
{
  int count = 0;
  for (int entry = 0; entry < m_NumClrs; entry++)
    if (*((DWORD*)&fromColor) == *((DWORD*)(m_lpColors+entry))) {  // Small data model requires this order
      m_lpColors[entry] = toColor;
      count++;
      if (!mapAll)
        break;
    }
  return count;
}

bool CDib::IsBottomUp()
{
   return m_BottomUp;
}
