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

#include "ddraw.h"
#include "azaria.h"
#include "includes.h"
#include "paths.h" // for path the palette.
#include "display.h"
#include "../include/fblt.h"

#define Msg

// move this to Display member
PALETTE palPalette;

#define szPalGui "gui.pal"

Display display;	// The one and only video display adapter.

void dumpPalette(PALETTE *a)
{
	int i;

	Msg("  Red Green  Blue");
	for (i=0;i<256;i++)
	{
		Msg("%3d: %5d %5d %5d", i, a->ape[i].peRed, a->ape[i].peGreen, a->ape[i].peBlue);
	}		
}

void PrintPalettes(PALETTE *a, PALETTE *b)
{
	int i;

	Msg("  Red Green  Blue");
    for( i=0; i<256; i++ )
        {
			int j;

			for (j=i+1;j<256;j++)
			{
				if (a->ape[i].peRed == b->ape[j].peRed
				 && a->ape[i].peGreen == b->ape[j].peGreen
				 && a->ape[i].peBlue == b->ape[j].peBlue)
				{
					Msg("%3d: %5d %5d %5d", i, a->ape[i].peRed, a->ape[i].peGreen, a->ape[i].peBlue);
					Msg("  -------> Duplicate color at %d.", j);
					break;
				}
			}		
        }
}

/*
 * intitilize the display manager.
 */
BOOL Display::Init(HWND hWnd,  SIZE GameSize, UINT bPP )
{
  static BOOL havepal = FALSE;

  hWndMain = hWnd;

  m_Size = GameSize;
  m_bPP = bPP;

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

  Clear();      // clear all the backbuffers.

  return TRUE;

} /* InitGame */

//___________________________________________________________________________
//
/*
 * DDEnable
 */
BOOL Display::Enable( void )
{
    HRESULT             ddrval;
    IDirectDraw *lpdd = nullptr;

    Msg("Enable");
    bFullscreen = TRUE;
    
    // TODO:
    ddrval = DirectDrawCreate( NULL, &lpdd, NULL );

    m_flipping_pages = 2;
    nBufferCount = 2;
    m_DirtyFrameMask = 0x01;

    Msg("Using %d buffers with %d flipping pages",  nBufferCount, m_flipping_pages);
    
    lpDD = lpdd;
    
    return TRUE;
    
error:
    return FALSE;

} /* DDEnable */

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
void Display::FlipToGdi(BOOL bIsActive)
{
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
 * DDClear
 *
 * clear the front buffer and all backbuffers.
 */
BOOL Display::Clear( void )
{
    return TRUE;
} /* DDClear */

HRESULT IDirectDraw::CreateSurface(
                                     LPDDSURFACEDESC lpDDSurfaceDesc,
                                     LPDIRECTDRAWSURFACE FAR* lplpDDSurface,
                                     IUnknown FAR* pUnkOuter
                                     )
{
    return DD_OK;
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
    HRESULT             ddrval;
    DDSURFACEDESC       ddsd;

    memset( &ddsd, 0, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );
    
    ddrval = lpDD->CreateSurface(&ddsd, &lpFrontBuffer, NULL );
    
    if( ddrval != DD_OK )
    {
        Msg( "CreateSurface FAILED - %s", GetDdError(ddrval) );
        return FALSE;
    }
    
    lpBackBuffer = CreateSurface( m_Size.cx, m_Size.cy, FALSE, FALSE );
    
    if( lpBackBuffer == NULL )
    {
        Msg( "Can't create the backbuffer - %s", GetDdError(ddrval) );
        return FALSE;
    }
    

    return TRUE;
} /* DDCreateFlippingSurface */

//___________________________________________________________________________
//
/*
 * CreateSurface
 */
IDirectDrawSurface *Display::CreateSurface(
                DWORD width,
                DWORD height,
                BOOL sysmem,
                BOOL trans )
{
    DDSURFACEDESC       ddsd;
    HRESULT             ddrval;
    LPDIRECTDRAWSURFACE psurf;
    
    
    /*
     * fill in surface desc
     */
    memset( &ddsd, 0, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwHeight = height;
    ddsd.dwWidth = width;
    
    ddrval = lpDD->CreateSurface(&ddsd, &psurf, NULL );
    
    if( ddrval != DD_OK )
    {
        Msg( "CreateSurface FAILED - %s", GetDdError( ddrval ) );
        psurf = NULL;
    }
    
    return psurf;

} /* DDCreateSurface */

//___________________________________________________________________________
//
BOOL Display::CreatePalette()
{
#ifdef _WIN32
    lpDD->CreatePalette(DDPCAPS_8BIT,m_palette,&lpDDPalette,NULL );

	if( lpDDPalette == NULL )
	{
		Msg("Palette create failed");
		return FALSE;
	}
#endif
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
void Display::PrintPalette()
{
	int i;

	Msg("  Red Green  Blue");
    for( i=0; i<256; i++ )
        {
			int j;

			for (j=i+1;j<256;j++)
			{
				if (m_palette[i].peRed == m_palette[j].peRed
				 && m_palette[i].peGreen == m_palette[j].peGreen
				 && m_palette[i].peBlue == m_palette[j].peBlue)
				{
					Msg("%3d: %5d %5d %5d", i, m_palette[i].peRed, m_palette[i].peGreen, m_palette[i].peBlue);
					Msg("  -------> Duplicate color at %d.", j);
					break;
				}
			}		
        }
}

//----------------------------------------------------------------------------
// attach palette to the primary surface
BOOL Display::AttachPalette()
{
	return true;
} /* doAttach */

//----------------------------------------------------------------------------
void Display::ReleasePalette()
{

}

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
/*
 * Display::ddxFlip
 *
 * this is called when the game loop has rendered a frame into
 * the backbuffer, its goal is to display something for the user to see.
 *
 * there are three cases...
 *
 * Fullscreen:
 *      we just call IDirectDrawSurface::Flip(lpFrontBuffer)
 *      being careful to handle return code right.
 *
 * Window mode (foreground palette):
 *      in this case we call IDirectDrawSurface::Blt to copy
 *      the back buffer to the window.
 *
 * Window mode (background palette):
 *      in this case we are in a window, but we dont own the
 *      palette. all our art was loaded to a specific palette
 *      IDirectDrawSurface::Blt does not do color translation
 *      we have a few options in this case...
 *
 *          use GDI to draw the backbuffer, GDI will handle
 *          the color conversion so things will look correct.
 *
 *          pause the game.
 *
 */
int Display::Flip( void )
{
    HRESULT     ddrval;

    if (m_Paused)
        return m_current_buffer;
    
    Restore();
    
    if( bFullscreen && (m_flipping_pages>1))
    {
        ddrval = lpFrontBuffer->Flip(NULL, DDFLIP_WAIT );
        if( ddrval != DD_OK )
        {
            Msg("*** Display::Flip FAILED - $s", GetDdError(ddrval) );
        }
        else
        {
            m_current_buffer++;
            if (m_current_buffer >= m_flipping_pages)
                m_current_buffer = 0;
        }
    }

    if (m_PalChanged)
    {
        if (blindness)
            lpDDPalette->SetEntries(0, 0, 256, m_blind_palette);
        else
            lpDDPalette->SetEntries(0, 0, 256, m_palette);
        
        m_PalChanged = FALSE;
    }
    
    return m_current_buffer;

} /* ddxFlip */


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
void Display::Restore()
{
#if 0 // IOS
    if ((lpFrontBuffer && lpFrontBuffer->IsLost() == DDERR_SURFACELOST) ||
       (lpBackBuffer && lpBackBuffer->IsLost() == DDERR_SURFACELOST))
    {
		Msg(" ******************************* Restore surfaces!");
        if (!Restore(TRUE))
        {
            errorCriticalStop("***** local_restore failed.");
        }
    }
#endif
}


const char *Display::GetDdError(HRESULT hr)
{
	  return "unknown error";
}

static int DeltaColor(int color, int blindness)
{
  if (blindness>color)
	return 0;
  else
	return color-blindness;
}

static void UniqueColor(int low_index, int high_index, PALETTEENTRY *pal, PALETTEENTRY *entry)
{
  int i, giveup;
  BOOL r=TRUE,g=FALSE, dup;

  for (giveup=0; giveup<32; giveup++) // give up after awhile . . .
  {
	dup = FALSE;
	for (i=low_index;i<=high_index;i++)
	{
		if (pal[i].peRed == entry->peRed
		 && pal[i].peGreen == entry->peGreen
		 && pal[i].peBlue == entry->peBlue)
		{ // found a duplicate color:
		  // diddle one color at a time:
		  if (r)
		  {
			r=FALSE;
			g = TRUE;
			entry->peRed++;
		  }
		  else if (g)
		  {
			entry->peGreen++;
			g=FALSE;
		  }
		  else
		  {
			entry->peBlue++;
			r=TRUE;
		  }
		  dup = TRUE;
		  break;
		}
	}
	if (!dup)
	  return; 	// finally got a unique color!
  }
}

int Display::SetBlind(int delta)
{
  // turning up the blindness (mmmmwwwaaaaahahahaha)
  if (delta >= 0)
  {
	blindness+=delta;
  }
  else
  {
	if (blindness + delta <= 0)
	{
	  blindness = 0;
	}
	else
	{
	  blindness += delta;
	}
  }

  for (int i=0;i<256;i++)
  {
	if (i>10 && i<246)
	{
		m_blind_palette[i].peRed   = DeltaColor(m_palette[i].peRed, blindness);
		m_blind_palette[i].peGreen = DeltaColor(m_palette[i].peGreen,blindness);
		m_blind_palette[i].peBlue  = DeltaColor(m_palette[i].peBlue,blindness);
		m_blind_palette[i].peFlags = m_palette[i].peFlags;
		UniqueColor(0, i-1, m_blind_palette, &m_blind_palette[i]);
	}
	else
	{
		m_blind_palette[i].peRed   = m_palette[i].peRed;
		m_blind_palette[i].peGreen = m_palette[i].peGreen;
		m_blind_palette[i].peBlue  = m_palette[i].peBlue;
		m_blind_palette[i].peFlags = m_palette[i].peFlags;
	}
  }

  m_PalChanged = TRUE;
  return blindness;
}

