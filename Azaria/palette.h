/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source code contained in this file is the 
; confidential property of Walter Bennett Advanced Technologies.
;
;
; 
; File:       ddx.c
; Description:    Direct Draw eXtensions.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/

#ifndef __PALETTE_INCLUDED__
#define __PALETTE_INCLUDED__

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

// globals:
extern PALETTE palPalette;

#endif
