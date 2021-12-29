/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; 
; File:       ddraw.cpp
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "display.h"
#include "palette.h"
#include "one.h"

#include "paths.h" // for path the palette.
#include "../include/fblt.h"

// mixing SDL2 and OpenGL
// https://gist.github.com/anonymous/6450139
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
// #include <GL/freeglut.h>
#include <stddef.h>
#include <stdlib.h>
#include <cstring>

#define Msg

// move this to Display member
PALETTE palPalette;

#define szPalGui "gui.pal"

Display display;	// The one and only video display adapter.

static GLubyte back_buffer[640*480*4];
static GLuint rgb_buffer[640*480];

typedef uint32_t   PIXEL;

typedef struct _RGBA8 {
    size_t width;
    size_t height;
    size_t bytesPerRow;
    size_t bpp;
    PIXEL *bitmapData;
} RGBA8Image;

static RGBA8Image BackBuffer = {
    .width = 640,
    .height = 480,
    .bytesPerRow = 640,
    .bpp = sizeof(PIXEL),
    .bitmapData = (PIXEL*)back_buffer
};

typedef struct tagRGBV {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peAlpha;
} RGBV;

void copybuffer(uint32_t *a, uint32_t *b) {
    RGBV rgbv;

    for (int r=0; r<480; r++) {
        for (int c=0; c<640; c++) {
            b[((479-r)*640)+c] = *(uint32_t*)(&a[(r*640)+c]);
        }
    }
}

void __paint__(unsigned char *data);

void paintBuffer(RGBA8Image * _surface)
{
    // glClear(GL_COLOR_BUFFER_BIT);
    // glRasterPos2i (0, 0);
    // copybuffer((uint32_t*)_surface->bitmapData, rgb_buffer);
    // glPixelZoom( 2, 2 );
    // glDrawPixels(640, 480,  GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, rgb_buffer);
    __paint__((unsigned char *)_surface->bitmapData);
}

/*
 * intitilize the display manager.
 */
BOOL Display::Init()
{
  static BOOL havepal = FALSE;

  /*
   * init DirectDraw, set mode, ...
   */
  if( !Begin() )
	  return FALSE;

  // setup the default palette
  if (!havepal)
  {
    ReadPalFile(PalettesPath, szPalGui, &palPalette);
    SetPalette(palPalette.ape, TRUE);
    //dumpPalette(&palPalette);
  }

  havepal = TRUE;
  if (!CreatePalette())
		  return FALSE;

  return TRUE;

} /* InitGame */

//___________________________________________________________________________
//
/*
 * DDEnable
 */
BOOL Display::Enable( void )
{
    Msg("Enable");
    
    m_flipping_pages = 2;
    nBufferCount = 2;
    m_DirtyFrameMask = 0x01;

    Msg("Using %d buffers with %d flipping pages",  nBufferCount, m_flipping_pages);
    
    return TRUE;
}

//___________________________________________________________________________
//
/*
 * DDDisable
 */
 BOOL Display::Disable( BOOL fFinal )
{
    return TRUE;
}

//___________________________________________________________________________
//
BOOL Display::ReInit()
{
    return true;
}

//___________________________________________________________________________
//
/*
 * Restore()
 */
BOOL Display::_Restore()
{
	// All backbuffers need repainting
	SetAllDirty();
    return TRUE;
}

//___________________________________________________________________________
//
BOOL Display::Restore(int active_app)
{
	Msg(" * * * * * * Restore");
	if (_Restore())
		return TRUE;
	else
		return ReInit();
}

//___________________________________________________________________________
//
/*
 * DDCreateFlippingSurface
 *
 * create a FrontBuffer and a BackBuffer(s)
 *
 */
BOOL Display::CreateFlippingSurface( void )
{
  //  lpBackBuffer = new OpenglDirectDrawSurface();
  //  assert(lpBackBuffer);
    return TRUE;
} /* DDCreateFlippingSurface */

//___________________________________________________________________________
//
BOOL Display::CreatePalette()
{
	AttachPalette();
	return TRUE;
}

//----------------------------------------------------------------------------
BOOL Display::SetPalette(PALETTEENTRY *pal, BOOL bAll)
{
	if (bAll)
		memcpy(m_palette, pal, sizeof(m_palette));
	else
		memcpy(m_palette+121, pal+121, sizeof(m_palette)-sizeof(pal[0])*121);

	m_PalChanged = TRUE;

	return TRUE;
}

//----------------------------------------------------------------------------
// attach palette to the primary surface
BOOL Display::AttachPalette()
{
	return true;
} /* doAttach */

//----------------------------------------------------------------------------
//
/*
 * ReadPalFile
 *
 * Create a DirectDrawPalette from a palette file
 *
 * if the palette files cant be found, make a default 332 palette
 */
void Display::ReadPalFile( const char *path, const char *filename, PALETTE *pal)
{
    int                 fh;
	char lpszPathName[_MAX_PATH];

    pal->dwRiff = 0;

    if (filename)
    {
		// build complete path the the file
		makepath( lpszPathName, NULL, path, filename, NULL);

        fh = open( lpszPathName, O_RDONLY);

        if (fh != -1)
        {
            read(fh, pal, sizeof(PALETTE));
            close(fh);
        }
		else
		{
			Msg("Palette file (%s) is missing.", lpszPathName);
			throw(ERROR_FILE_NOT_FOUND);
		}
    }

    if (pal->dwRiff != 0x46464952 || // 'RIFF'
        pal->dwPal  != 0x204C4150 || // 'PAL '
        pal->dwData != 0x61746164 || // 'data'
        pal->palVersion != 0x0300 ||
        pal->palNumEntries > 256  ||
        pal->palNumEntries < 1)
    {
        Msg("Palette file is corrupt.");
        Msg("pal->palVersion = %x\n",pal->palVersion);

        throw(ERROR_FILE_NOT_FOUND);
    }
} /* ReadPalFile */

//----------------------------------------------------------------------------
//
int Display::Flip( void )
{
    HRESULT     ddrval;

    if(m_flipping_pages>1)
    {
        paintBuffer(&BackBuffer);

        if( true ) {
            m_current_buffer++;
            if (m_current_buffer >= m_flipping_pages)
                m_current_buffer = 0;
        }
    }
    return m_current_buffer;

}

//----------------------------------------------------------------------------
// ddxBegin
BOOL Display::Begin( void )
{
    if( !Enable() )
    {
        return FALSE;
    }

    if( !CreateFlippingSurface() )
    {
        Disable(TRUE);
        return FALSE;
    }
    return TRUE;
} /* ddxBegin */

//----------------------------------------------------------------------------
void Display::Blt(ONE *src, FBLT_DSCR *fbd)
{
  HRESULT ddrval;

  	if (src->Validate())
	{
			ONE oneDst(BackBuffer.bitmapData, BackBuffer.width, BackBuffer.height, BackBuffer.bytesPerRow);
			try
			{
				fbd->Blt(&oneDst, src);
			}
			catch(...)
			{
				errorCriticalStop("exception in fbd.blt");
			}
	} else {
		printf("%s(%d): Validate failed\n", __FILE__, __LINE__);
	}
}
