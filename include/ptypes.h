//
//  ptypes.h
//  Azaria
//
//  Created by guy on 11/11/17.
//  Copyright © 2017 guy. All rights reserved.
//

#ifndef ptypes_h
#define ptypes_h

enum {
    WS_POPUP,
    WS_SYSMENU,
    WS_POPUPWINDOW,
};

#define WS_CAPTION 0

#define FAR
#define PASCAL

#define BOOL bool
#define TRUE true
#define FALSE false
#define HRESULT int
#define BYTE unsigned char
#define UINT unsigned int
#define LPSTR char *
#define HWND int
#define LONG long
#define SIZE int
#define UCHAR unsigned char
#define VOID void

#define ERROR_FILE_NOT_FOUND 0x02
#define ERROR_OUT_OF_STRUCTURES 0x54

typedef unsigned long DWORD;
typedef unsigned short WORD;
#define LOWORD(a) ((WORD)(a))
#define HIWORD(a) ((WORD)(((DWORD)(a) >> 16) & 0xFFFF))

typedef UINT * UINT_PTR;
typedef UINT_PTR WPARAM;
typedef long  * LONG_PTR;
typedef LONG_PTR LPARAM;

typedef struct _GUID {
    DWORD Data1;
    WORD  Data2;
    WORD  Data3;
    BYTE  Data4[8];
} GUID;

typedef GUID * LPGUID;

class DIRECTDRAW {
};
typedef DIRECTDRAW * LPDIRECTDRAW;

class DIRECTDRAWSURFACE {
};
typedef DIRECTDRAWSURFACE * LPDIRECTDRAWSURFACE;

class DIRECTDRAWPALETTE {
};
typedef DIRECTDRAWPALETTE * LPDIRECTDRAWPALETTE;

class DIRECTDRAWCLIPPER {
};
typedef DIRECTDRAWCLIPPER * LPDIRECTDRAWCLIPPER;

class DIRECTPLAY {
    
};
typedef DIRECTPLAY * LPDIRECTPLAY;

typedef struct {
    int nada;
} DPSESSIONDESC;

typedef UINT HANDLE;

typedef DWORD COLORREF;
typedef DWORD* LPCOLORREF;

typedef DWORD DPID;

typedef void * LPVOID;

typedef void * HBITMAP;

typedef struct tagPALETTEENTRY {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
} PALETTEENTRY;

typedef struct tagLOGPALETTE {
    WORD         palVersion;
    WORD         palNumEntries;
    PALETTEENTRY palPalEntry[1];
} LOGPALETTE;

typedef void * HPALETTE;

typedef struct {
    int x;
    int y;
} POINT;
typedef POINT * LPPOINT;

// TODO: yep
#define O_BINARY 0
#define O_TEXT 0
#define _MAX_DRIVE 1024
#define _MAX_DIR   1024
#define _MAX_FNAME 1024
#define _MAX_PATH  1024

#define MAX_DRIVE 1024
#define MAX_DIR   1024
#define MAX_FNAME 1024
#define MAX_PATH  1024


#endif /* ptypes_h */
