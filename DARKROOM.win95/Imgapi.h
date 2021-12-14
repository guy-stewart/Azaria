// dibapi.h
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#ifndef _INC_DIBAPI
#define _INC_DIBAPI

typedef struct {
    DWORD           dwRiff;
    DWORD           dwFileSize;
    DWORD           dwPal;
    DWORD           dwData;
    DWORD           dwDataSize;
    WORD            palVersion;
    WORD            palNumEntries;
    PALETTEENTRY    ape[256];
} PALETTE;

/* Handle to a DIB */
DECLARE_HANDLE(HDIB);

/* DIB constants */
#define PALVERSION   0x300

/* DIB Macros*/

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

/* Function prototypes */
BOOL      WINAPI  PaintDIB (HDC, LPRECT, HDIB, LPRECT, CPalette* pPal);
BOOL      WINAPI  CreateDIBPalette(HDIB hDIB, CPalette* cPal);
BOOL      WINAPI CompressedDIB(HDIB hDib);
DWORD     WINAPI  SizeDIB(HDIB hDib);
DWORD     WINAPI  SizeDIBBITS(HDIB hDib);
LPSTR     WINAPI  FindDIBBits (LPSTR lpbi);
DWORD     WINAPI  DIBWidth (LPSTR lpDIB);
DWORD     WINAPI SetDIBWidth(LPSTR lpDIB, DWORD new_width);
DWORD     WINAPI  AddDIBWidth (LPSTR lpDIB, DWORD add_width);
DWORD     WINAPI DIBPitch(LPSTR lpDIB);
DWORD     WINAPI  DIBHeight (LPSTR lpDIB);
void      WINAPI FlipDIB_H_W(LPSTR lpDIB);
DWORD     WINAPI SetDIBHeight(LPSTR lpDIB, DWORD new_height);
DWORD     WINAPI  AddDIBHeight (LPSTR lpDIB, DWORD add_height);
WORD      WINAPI  PaletteSize (LPSTR lpbi);
WORD      WINAPI  DIBNumColors (LPSTR lpbi);
WORD	  WINAPI DIBBitCount(LPSTR lpbi);
HGLOBAL   WINAPI  CopyHandle (HGLOBAL h);

BOOL      WINAPI  SaveDIB (HDIB hDib, CFile& file);
HDIB      WINAPI CreateDIB(LPSTR bits, PALETTEENTRY *pal, DWORD height, DWORD width, DWORD pitch);
HDIB      WINAPI  ReadDIBFile(CFile& file);
void ReadPalFile( char *path, char *filename, PALETTE *pal);

#endif //!_INC_DIBAPI
