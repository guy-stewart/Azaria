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

#include "includes.h"
#include "paths.h" // for path the palette.
#include "display.h"

#define Flow Msg
//#define Flow

// move this to Display member
PALETTE palPalette;

#define szPalGui "gui.pal"

Display display;	// The one and only video display adapter.

#ifdef IOS
#else
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
  }

  havepal = TRUE;
  if (!CreatePalette())
		  return FALSE;

  Clear();      // clear all the backbuffers.

  return TRUE;

} /* InitGame */


//___________________________________________________________________________
//
// Are they running windows in 256 color mode?
static BOOL is256Color()
{
	HDC hdc;
	int bpp;

	hdc = GetDC(NULL);
	bpp = GetDeviceCaps(hdc, PLANES) * GetDeviceCaps(hdc, BITSPIXEL);
	ReleaseDC(NULL, hdc);

	return bpp == 8;
}

//___________________________________________________________________________
//
/*
 * DDEnable
 */
BOOL Display::Enable( void )
{
	DWORD dwStyle;

    LPDIRECTDRAW        lpdd;
    DDCAPS              ddcaps;
    HRESULT             ddrval;

	Flow("Enable");

	//
	// We cannot, will not run in hi-color on the desktop.
	// If we are not in 256 color mode,  then run full-screen
	if (!is256Color())
		bFullscreen = TRUE;

    if (lpDD == NULL)
    {
		Flow("DirectDrawCreate()");
		ddrval = DirectDrawCreate( NULL, &lpdd, NULL );
		if( ddrval != DD_OK )
		{
			Msg("DirectDrawCreate failed err=%d", ddrval);
			goto error;
		}
    }
    else
    {
        lpdd = lpDD;
        ddrval = DD_OK;
        Msg("DDEnable is re-using lpDD");
    }

    /*
     * grab exclusive mode if we are going to run as fullscreen
     * otherwise grab normal mode.
     */
    //if (lpDD == NULL)
    {
        if (bFullscreen)
        {
            ddrval = lpdd->SetCooperativeLevel(
							hWndMain,
                            DDSCL_EXCLUSIVE
                            |DDSCL_FULLSCREEN
							|DDSCL_ALLOWREBOOT
							);

        }
        else
        {
			if (bExclusive)
			{
		        ddrval = lpdd->SetCooperativeLevel(
								hWndMain,
		                        DDSCL_EXCLUSIVE 
								|DDSCL_FULLSCREEN
								|DDSCL_NOWINDOWCHANGES
								);
			}
			else
			{
		        ddrval = lpdd->SetCooperativeLevel(
								hWndMain,
		                        DDSCL_NORMAL
								|DDSCL_NOWINDOWCHANGES
								);
			}

        }

        if( ddrval != DD_OK )
        {
            Msg("SetCooperativeLevel failed err=%d", ddrval);
            goto error;
        }
    }

    if (bFullscreen)
    {
        dwStyle = GetWindowStyle(hWndMain);
        dwStyle &= ~(WINDOWED_STYLE);
        dwStyle |= FULLSCREEN_STYLE;
        SetWindowLong(hWndMain, GWL_STYLE, dwStyle);
/*
        SetWindowPos(hWndMain, HWND_NOTOPMOST, 0, 0, 0, 0,
            SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
*/

		Flow("SetDisplayMode(0x%X, %d, %d, %d)",lpdd,m_Size.cx, m_Size.cy, m_bPP);
        ddrval = lpdd->SetDisplayMode(m_Size.cx, m_Size.cy, m_bPP);
		Flow("Display mode has been set");
        if( ddrval != DD_OK )
        {
            Msg("SetMode failed err=%d", ddrval);
            goto error;
        }
    }
    else
    {
		RECT rcWork;
		RECT rc;

        //
        // if we are still a WS_POPUP window we should convert to a
        // normal app window so we look like a windows app.
        //
        dwStyle = GetWindowStyle(hWndMain);
        dwStyle &= ~(FULLSCREEN_STYLE);
        dwStyle |= WINDOWED_STYLE;
        SetWindowLong(hWndMain, GWL_STYLE, dwStyle);

        SetWindowPos(hWndMain, NULL, 0, 0, 0, 0,
            SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

		rc.left = GetSystemMetrics(SM_CXSCREEN)/2 - m_Size.cx/2;
		rc.top  = GetSystemMetrics(SM_CYSCREEN)/2 - m_Size.cy/2;
		rc.right = rc.left + m_Size.cx;
		rc.bottom = rc.top + m_Size.cy;

        AdjustWindowRectEx(&rc,
            GetWindowStyle(hWndMain),
            GetMenu(hWndMain) != NULL,
            GetWindowExStyle(hWndMain));

		/*
        SetWindowPos(hWndMain, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,
            SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		*/

        SetWindowPos(hWndMain, 
						NULL,
						rc.left,
						rc.top,
						rc.right - rc.left,
						rc.bottom - rc.top,
						SWP_NOZORDER | SWP_NOACTIVATE);


        SetWindowPos(hWndMain, HWND_NOTOPMOST, 0, 0, 0, 0,
            SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

        //
        //  make sure our window does not hang outside of the work area
        //  this will make people who have the tray on the top or left
        //  happy.
        //
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
        GetWindowRect(hWndMain, &rc);
        if (rc.left < rcWork.left) rc.left = rcWork.left;
        if (rc.top  < rcWork.top)  rc.top  = rcWork.top;
        SetWindowPos(hWndMain, NULL, rc.left, rc.top, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    /*
     * check capabilites
     */ 
    ddcaps.dwSize = sizeof( ddcaps );
    ddrval = lpdd->GetCaps(&ddcaps, NULL );

    if( ddrval != DD_OK )
    {
        Msg("GetCaps failed err=%d", ddrval);
        goto error;
    }

    if( ddcaps.dwCaps & DDCAPS_NOHARDWARE )
    {
        Msg( "No hardware support." );
    }

    /*
     * on systems with fewer than 2 pages of vram,  flipping is
	 * accomplished by copying from system memory.
     * default to double buffered on 1mb, triple buffered
     * on > 1mb
     */
    Msg( "VidMemTotal=%d, VidMemFree=%d", (int)ddcaps.dwVidMemTotal, (int)ddcaps.dwVidMemFree );
    Msg( "Used = %d", (int)(ddcaps.dwVidMemTotal- ddcaps.dwVidMemFree));

	if (!bFullscreen)
	{
	  m_flipping_pages = 1;
      nBufferCount = 2;
	  m_DirtyFrameMask = 0x01;
	}
	else
	{
      if(ddcaps.dwVidMemTotal <= (640L*480L*2L))
	  {
		  m_flipping_pages = 1;
          nBufferCount = 2;
		  m_DirtyFrameMask = 0x01;
	  }
      else if( ddcaps.dwVidMemTotal <= 1024L*1024L)
      {
		  m_flipping_pages = 2;
          nBufferCount = 2;
		  m_DirtyFrameMask = 0x03;
      }
      else
      {
		  m_flipping_pages = 3;
          nBufferCount = 3;
		  m_DirtyFrameMask = 0x07;
      }
	}

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
    if( lpClipper )
    {
        lpClipper->Release();
        lpClipper = NULL;
    }

    if( lpBackBuffer )
    {
        lpBackBuffer->Release();
        lpBackBuffer = NULL;
    }

    if( lpFrontBuffer )
    {
        lpFrontBuffer->Release();
        lpFrontBuffer = NULL;
    }

    //
    // fFinal is TRUE when the app is exiting, FALSE if we are
    // just seting a new game size..
    //
    if ( fFinal )
    {
        if( lpDD != NULL )
        {
            lpDD->Release();
            lpDD = NULL;
        }
    }

    return TRUE;
}

//___________________________________________________________________________
//
void Display::FlipToGdi(BOOL bIsActive)
{
  if (bIsActive)
  {
	ShowCursor(FALSE);
	m_Paused = FALSE;
	SetAllDirty();
  }
  else
  {
	ShowCursor(TRUE);
	Flow("FlipToGDISurface");
	lpDD->FlipToGDISurface();
	m_Paused = TRUE;
  }
}

//___________________________________________________________________________
//
BOOL Display::ReInit()
{
	int rc;
	Disable(FALSE);
	rc = Begin();
	AttachPalette();
	return rc;
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

    if (lpFrontBuffer == NULL || lpFrontBuffer->Restore() != DD_OK)
    {
        Msg("***** cant restore FrontBuffer");
        return FALSE;
    }

    if (IsFullscreen())

    {
        if (lpBackBuffer == NULL || lpBackBuffer->Restore() != DD_OK)
        {
            Msg("***** cant restore BackBuffer");
            return FALSE;
        }
    }

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
// this is used strictly to find a color to erase DD surfaces.
static DWORD DDColorMatch(IDirectDrawSurface *pdds, COLORREF rgb)
{
    COLORREF rgbT;
    HDC hdc;
    DWORD dw = CLR_INVALID;
    DDSURFACEDESC ddsd;
    HRESULT hres;

    if (pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);
        SetPixel(hdc, 0, 0, rgb);
        pdds->ReleaseDC(hdc);
    }

    ddsd.dwSize = sizeof(ddsd);
    hres = pdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

    if (hres == DD_OK)
    {
        dw  = *(DWORD *)ddsd.lpSurface;
        dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)-1;
        pdds->Unlock(NULL);
    }

    if (pdds->GetDC(&hdc) == DD_OK)
    {
        SetPixel(hdc, 0, 0, rgbT);
        pdds->ReleaseDC(hdc);
    }

    return dw;
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
    DDBLTFX     ddbltfx;
    int         i;
    HRESULT     ddrval;
    UpdateWindow(hWndMain);

    ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = DDColorMatch(lpBackBuffer, RGB(0, 128, 128));

	// All backbuffers need repainting
	SetAllDirty();

    if (bFullscreen)
    {
        /*
         * do it for all buffers, we either have 1 or 2 back buffers
         * make sure we get them all, 4 is plenty!
         */
        for( i=0; i<4; i++ )
        {
            ddrval = lpBackBuffer->Blt(           // dest surface
                            NULL,                 // dest rect
                            NULL,                 // src surface
                            NULL,                 // src rect
                            DDBLT_COLORFILL | DDBLT_WAIT,
                            &ddbltfx);

            if( ddrval != DD_OK )
            {
                Msg("Fill failed ddrval =0x%08lX", ddrval);
                return FALSE;
            }

            ddrval = lpFrontBuffer->Flip(NULL, DDFLIP_WAIT);

            if( ddrval != DD_OK )
            {
                Msg("Flip failed ddrval =0x%08lX", ddrval );
                return FALSE;
            }
        }
    }
    else
    {
        ddrval = lpFrontBuffer->Blt(          // dest surface
                        &rcWindow,            // dest rect
                        NULL,                 // src surface
                        NULL,                 // src rect
                        DDBLT_COLORFILL | DDBLT_WAIT,
                        &ddbltfx);

        if( ddrval != DD_OK )
        {
            Msg("Fill failed ddrval =0x%08lX", ddrval);
            return FALSE;
        }
    }

    return TRUE;

} /* DDClear */

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
    DDSURFACEDESC       ddsd;
    HRESULT             ddrval;
    DDSCAPS             ddscaps;
    DDCAPS              ddcaps;

    ddcaps.dwSize = sizeof( ddcaps );

    if( lpDD->GetCaps(&ddcaps, NULL ) != DD_OK )
        return FALSE;

    /*
     * fill in surface desc:
     * want a primary surface with 2 back buffers
     */
    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );

    if (bFullscreen && (m_flipping_pages>1))
    {
        //
        //  fullscreen case, create a primary (ie front) and
        //  either 1  or 2 back buffers
        //
		Flow("creating %d flipping surfaces", m_flipping_pages);

        ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        ddsd.dwBackBufferCount = nBufferCount-1;
        ddsd.ddsCaps.dwCaps = 
			DDSCAPS_PRIMARYSURFACE 
		  | DDSCAPS_FLIP 
		  | DDSCAPS_COMPLEX
		  | DDSCAPS_VIDEOMEMORY;

        ddrval = lpDD->CreateSurface(&ddsd, &lpFrontBuffer, NULL);

        if( ddrval != DD_OK )
        {
            Msg( "CreateSurface FAILED - %s", GetDdError(ddrval));
            return FALSE;
        }

        /*
         * go find the back buffer
         */
        ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
        ddrval = lpFrontBuffer->GetAttachedSurface(&ddscaps,&lpBackBuffer );

        if( ddrval != DD_OK )
        {
            Msg( "GetAttachedSurface failed - %s", GetDdError(ddrval) );
            return FALSE;
        }
    }
	/*
    else if (bFullscreen)
    {
        ddsd.dwFlags = DDSD_CAPS;
        //ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        ddsd.ddsCaps.dwCaps = 
			DDSCAPS_PRIMARYSURFACE 
		  | DDSCAPS_FLIP 
		  | DDSCAPS_COMPLEX
		  | DDSCAPS_VIDEOMEMORY;

        ddrval = lpDD->CreateSurface(&ddsd, &lpFrontBuffer, NULL );

        if( ddrval != DD_OK )
        {
            Msg( "CreateSurface FAILED - %s", GetDdError(ddrval) );
            return FALSE;
        }

        lpFrontBuffer->AddRef();
        lpBackBuffer = lpFrontBuffer;
    }
	*/
    else
    {
        //
        //  window case, create the primary surface
        //  and create a backbuffer in offscreen memory.
        //
        ddsd.dwFlags = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

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

        //
        // now create a DirectDrawClipper object.
        //
        ddrval = lpDD->CreateClipper(0, &lpClipper, NULL);

        if( ddrval != DD_OK )
        {
            Msg("Can't create clipper");
            return FALSE;
        }

        ddrval = lpClipper->SetHWnd(0, hWndMain);

        if( ddrval != DD_OK )
        {
            Msg("Can't set clipper window handle - %s", GetDdError(ddrval));
            return FALSE;
        }

        ddrval = lpFrontBuffer->SetClipper(lpClipper);

        if( ddrval != DD_OK )
        {
            Msg("Can't attach clipper to front buffer");
            return FALSE;
        }
    }

    return TRUE;

} /* DDCreateFlippingSurface */

//___________________________________________________________________________
//
/*
 * CreateSurface
 */
LPDIRECTDRAWSURFACE Display::CreateSurface(
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
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
    
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    if( sysmem )
    {
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    }
    
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
    lpDD->CreatePalette(DDPCAPS_8BIT,m_palette,&lpDDPalette,NULL );

	if( lpDDPalette == NULL )
	{
		Msg("Palette create failed");
		return FALSE;
	}

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
    HRESULT     ddrval;

	if (!(lpDDPalette && lpFrontBuffer))
		return FALSE;

    ddrval = lpFrontBuffer->SetPalette(lpDDPalette );
    if( ddrval == DDERR_SURFACELOST )
    {
		lpFrontBuffer->Restore();
        ddrval = lpFrontBuffer->SetPalette(lpDDPalette);
    }

	return (ddrval == DD_OK);
} /* doAttach */

//----------------------------------------------------------------------------
void Display::ReleasePalette()
{
    if( lpDDPalette )
    {
		lpDDPalette->Release();
        lpDDPalette = NULL;
    }
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
void Display::ReadPalFile( char *path, char *filename, PALETTE *pal)
{
    int                 fh;
	char lpszPathName[_MAX_PATH];

    pal->dwRiff = 0;

    if (filename)
    {
		// build complete path the the file
		_makepath( lpszPathName, NULL, path, filename, NULL);

        fh = _lopen( lpszPathName, OF_READ);

        if (fh != -1)
        {
            _lread(fh, pal, sizeof(PALETTE));
            _lclose(fh);
        }
		else
		{
			Msg("Palette file is missing.");
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
    else
    {
	    ddrval =   lpFrontBuffer->Blt(          // dest surface
	                    &rcWindow,              // dest rect
	                    lpBackBuffer,           // src surface
	                    NULL,                   // src rect (all of it)
	                    DDBLT_WAIT,
	                    NULL);

	    if( ddrval != DD_OK )
	    {
	        Msg( "*** Display::UpdateWindow FAILED - %s", GetDdError(ddrval));
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
    if ((lpFrontBuffer && lpFrontBuffer->IsLost() == DDERR_SURFACELOST) ||
        (lpBackBuffer && lpBackBuffer->IsLost() == DDERR_SURFACELOST))
    {
		Msg(" ******************************* Restore surfaces!");
        if (!Restore(TRUE))
        {
            errorCriticalStop("***** local_restore failed.");
        }
    }
}


char *Display::GetDdError(HRESULT hr)
{
#define DdError(ERRNO) case ERRNO: return #ERRNO

  switch (hr)
  {
	DdError(DD_OK);
	DdError(DDERR_ALREADYINITIALIZED);
	DdError(DDERR_BLTFASTCANTCLIP);
	DdError(DDERR_CANNOTATTACHSURFACE);
	DdError(DDERR_CANNOTDETACHSURFACE);
	DdError(DDERR_CANTCREATEDC);
	DdError(DDERR_CANTDUPLICATE);
	DdError(DDERR_CANTLOCKSURFACE);
	DdError(DDERR_CANTPAGELOCK);
	DdError(DDERR_CANTPAGEUNLOCK);
	DdError(DDERR_CLIPPERISUSINGHWND);
	DdError(DDERR_COLORKEYNOTSET);
	DdError(DDERR_CURRENTLYNOTAVAIL);
	DdError(DDERR_DCALREADYCREATED);
	DdError(DDERR_DIRECTDRAWALREADYCREATED);
	DdError(DDERR_EXCEPTION);
	DdError(DDERR_EXCLUSIVEMODEALREADYSET);
	DdError(DDERR_GENERIC);
	DdError(DDERR_HEIGHTALIGN);
	DdError(DDERR_HWNDALREADYSET);
	DdError(DDERR_HWNDSUBCLASSED);
	DdError(DDERR_IMPLICITLYCREATED);
	DdError(DDERR_INCOMPATIBLEPRIMARY);
	DdError(DDERR_INVALIDCAPS);
	DdError(DDERR_INVALIDCLIPLIST);
	DdError(DDERR_INVALIDDIRECTDRAWGUID );
	DdError(DDERR_INVALIDMODE);
	DdError(DDERR_INVALIDOBJECT);
	DdError(DDERR_INVALIDPARAMS);
	DdError(DDERR_INVALIDPIXELFORMAT);
	DdError(DDERR_INVALIDPOSITION);
	DdError(DDERR_INVALIDRECT);
	DdError(DDERR_INVALIDSURFACETYPE);
	DdError(DDERR_LOCKEDSURFACES);
	DdError(DDERR_NO3D);
	DdError(DDERR_NOALPHAHW);
	//DdError(DDERR_NOANTITEARHW);
	DdError(DDERR_NOBLTHW);
	//DdError(DDERR_NOBLTQUEUEHW);
	DdError(DDERR_NOCLIPLIST);
	DdError(DDERR_NOCLIPPERATTACHED);
	DdError(DDERR_NOCOLORCONVHW);
	DdError(DDERR_NOCOLORKEY);
	DdError(DDERR_NOCOLORKEYHW);
	DdError(DDERR_NOCOOPERATIVELEVELSET );
	DdError(DDERR_NODC);
	DdError(DDERR_NODDROPSHW);
	DdError(DDERR_NODIRECTDRAWHW);
	DdError(DDERR_NODIRECTDRAWSUPPORT);
	DdError(DDERR_NOEMULATION);
	DdError(DDERR_NOEXCLUSIVEMODE);
	DdError(DDERR_NOFLIPHW);
	DdError(DDERR_NOGDI);
	DdError(DDERR_NOHWND);
	DdError(DDERR_NOMIPMAPHW);
	DdError(DDERR_NOMIRRORHW);
	DdError(DDERR_NOOVERLAYDEST );
	DdError(DDERR_NOOVERLAYHW);
	DdError(DDERR_NOPALETTEATTACHED);
	DdError(DDERR_NOPALETTEHW);
	DdError(DDERR_NORASTEROPHW);
	DdError(DDERR_NOROTATIONHW);
	DdError(DDERR_NOSTRETCHHW);
	DdError(DDERR_NOT4BITCOLOR);
	DdError(DDERR_NOT4BITCOLORINDEX);
	DdError(DDERR_NOT8BITCOLOR);
	DdError(DDERR_NOTAOVERLAYSURFACE);
	DdError(DDERR_NOTEXTUREHW);
	DdError(DDERR_NOTFLIPPABLE);
	DdError(DDERR_NOTFOUND);
	DdError(DDERR_NOTLOCKED);
	DdError(DDERR_NOTPAGELOCKED);
	DdError(DDERR_NOTPALETTIZED);
	DdError(DDERR_NOVSYNCHW);
	DdError(DDERR_NOZBUFFERHW);
	DdError(DDERR_NOZOVERLAYHW );
	DdError(DDERR_OUTOFCAPS);
	DdError(DDERR_OUTOFMEMORY);
	DdError(DDERR_OUTOFVIDEOMEMORY);
	DdError(DDERR_OVERLAYCANTCLIP);
	DdError(DDERR_OVERLAYCOLORKEYONLYONEACTIVE);
	DdError(DDERR_OVERLAYNOTVISIBLE );
	DdError(DDERR_PALETTEBUSY);
	DdError(DDERR_PRIMARYSURFACEALREADYEXISTS);
	DdError(DDERR_REGIONTOOSMALL );
	DdError(DDERR_SURFACEALREADYATTACHED);
	DdError(DDERR_SURFACEALREADYDEPENDENT);
	DdError(DDERR_SURFACEBUSY);
	DdError(DDERR_SURFACEISOBSCURED);
	DdError(DDERR_SURFACELOST );
	DdError(DDERR_SURFACENOTATTACHED);
	DdError(DDERR_TOOBIGHEIGHT);
	DdError(DDERR_TOOBIGSIZE);
	DdError(DDERR_TOOBIGWIDTH);
	DdError(DDERR_UNSUPPORTED);
	DdError(DDERR_UNSUPPORTEDFORMAT);
	DdError(DDERR_UNSUPPORTEDMASK);
	DdError(DDERR_UNSUPPORTEDMODE);
	DdError(DDERR_VERTICALBLANKINPROGRESS);
	DdError(DDERR_WASSTILLDRAWING);
	DdError(DDERR_WRONGMODE);
	DdError(DDERR_XALIGN);
	default:
	  return "unknown error";
  }
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
#endif
