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
    uint32_t        dwRiff;
    uint32_t           dwFileSize;
    uint32_t           dwPal;
    uint32_t           dwData;
    uint32_t           dwDataSize;
    uint16_t            palVersion;
    uint16_t            palNumEntries;
    PALETTEENTRY    ape[256];
} PALETTE;

// globals:
extern PALETTE palPalette;

#endif
