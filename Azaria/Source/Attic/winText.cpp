/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; 
; File:       winText.c
; Description:    Basic windows text io THEGAME.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/29/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "includes.h"

#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#include <windowsx.h> 
#include <mmsystem.h>
#include <ddraw.h>
#include <dsound.h>
#endif

#include "display.h"

/*
static COLORREF BackColor = RGB(64,64,255);
static COLORREF TextColor = RGB(64,64,255);
*/
#ifdef _WIN32

typedef struct {
	int nHeight;
	char szFace[24];
} TBL_FONTS;

static TBL_FONTS tbl_fonts[] = {
	{14, "Arial"},
	{18, "Garamond Bold"},
	{48, "Time New Roman"},
	{16, "Courier"},
	{18, "Time New Roman Bold"},
};
static int tbl_fonts_count = sizeof(tbl_fonts) / sizeof(TBL_FONTS);

HFONT fontCreate(int font_id)
{
	HFONT hFont;
	TBL_FONTS *font;

	if ((font_id >= tbl_fonts_count)
	  || (font_id < 0))
		font_id = 0;	// select default font.

	font = &tbl_fonts[font_id];

    hFont = CreateFont(
        font->nHeight, // fontsize
        0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        NONANTIALIASED_QUALITY, // DEFAULT_QUALITY,
        VARIABLE_PITCH,
        font->szFace);

    return hFont;

}

/*
* Create a surface to copy our bits to.
*/
static BOOL ddtextValidateSurface()
{
	LPDIRECTDRAWSURFACE		lpTextSurface = display.GetTextSurface();

	if (!lpTextSurface)
		return FALSE;
	if (lpTextSurface->IsLost() == DDERR_SURFACELOST)
	{
	    if (lpTextSurface->Restore() != DD_OK)
		{
			return FALSE;
		}
	}
	return TRUE;
}

int GetTextExt(HFONT hFont, char *buff, SIZE *size)
{
    HDC                 hdc;
	LPDIRECTDRAWSURFACE		lpTextSurface = display.GetTextSurface();

	if (!ddtextValidateSurface())
		return FALSE;

	if( lpTextSurface->GetDC(&hdc) == DD_OK )
	{
		SelectObject(hdc, hFont);
	    GetTextExtentPoint32(hdc, buff, strlen(buff), size);
		lpTextSurface->ReleaseDC(hdc);
		return TRUE;
	}
	return FALSE;
}

int oneTextOut(ONE *one, int font_id, COLORREF color, int sx, int sy, char *buff)
{
	HDC hdc;
	HGDIOBJ old_obj;
	HFONT hFont;
	int rc = TRUE;
	COLORREF cRef;
	RECT rect;
	FBLT_DSCR fbd;
	LPDIRECTDRAWSURFACE		lpTextSurface = display.GetTextSurface();

	if (!ddtextValidateSurface())
		return FALSE;

	/* copy the  surface onto the lp surface so we can write on it: */
	fbd.m_src_x = sx;
	fbd.m_src_y = sy;
	fbd.m_dst_x = 0;
	fbd.m_dst_y = 0;
	fbd.m_dx = 400; // ### must compute this
	fbd.m_dy = 80;
	fbd.m_style = BLT_RECT;
	DDoneBlt(lpTextSurface, one, &fbd);

	if(lpTextSurface->GetDC(&hdc) == DD_OK )
	{
		hFont = fontCreate(font_id);
		if (hFont)
		{
			old_obj = SelectObject(hdc, hFont);
			if (!old_obj || old_obj == (HGDIOBJ)GDI_ERROR)
				rc = FALSE;

			cRef = SetTextColor(hdc, color);
			if (cRef == CLR_INVALID)
				rc = FALSE;

	        SetBkMode(hdc, TRANSPARENT);

			if (!TextOut(hdc, 0, 0, buff, strlen(buff)))
					rc = FALSE;

			DeleteObject(hFont);
		}
		lpTextSurface->ReleaseDC(hdc);

	    SetRect(&rect, 0, 0, 640, 100);

		fbd.m_src_x = 0;
		fbd.m_src_y = 0;
		fbd.m_dst_x = sx;
		fbd.m_dst_y = sy;
  		fbd.m_style = BLT_RECT;
		oneDDBlt(&rect, one, lpTextSurface, &fbd);
	}
	return rc;
}
#endif

