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
; File:       winMain.c
; Description:    Main windows procedure for THEGAME.
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 01/18/95  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifdef _WIN32

#include "ptypes.h"
#include "stdapp.h"
//#include <vfw.h>
#include "dsWave.h"
#include "music.h"

#include "paths.h"

#define __WAVETABLE_OWNER__
#include "Data/Database/tbl_wave.h"

// Managers
#include "user.h"
#include "gameman.h"
#include "display.h"
#include "netctrl.h"
#include "rcids.h"      // for MAYA_ICON
#include "resource.h"

#define GAME_SX 640
#define GAME_SY 480
#define GAME_BPP 8

static HWND		hWndMain;
SRECT			rcWindow;
BOOL bWasIconic = FALSE;
BOOL			bIsActive;
BOOL			bPaused;
BOOL			bPlayingVideo;
BOOL			bPostQuitMessage = FALSE;
BOOL			bIsForeground;
static SIZE		GameMode;   // display mode size
static UINT		GameBPP;    // the bpp we want

/*
 * PauseGame()
 */
static void PauseGame()
{
    Msg("**** PAUSE");
    bPaused = TRUE;
    InvalidateRect(hWndMain, NULL, TRUE);
}

/*
 * UnPauseGame()
 */
static void UnPauseGame()
{
    if (GetForegroundWindow() == hWndMain)
    {
        Msg("**** UNPAUSE");
        bPaused = FALSE;
    }
}


/*
 * MainWndProc
 *
 * Callback for all Windows messages
 */
static long FAR PASCAL MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static BOOL bCaptured = FALSE;
    PAINTSTRUCT ps;
    HDC         hdc;
	POINT p;
	p.x = LOWORD(lParam);
	p.y = HIWORD(lParam);
	static HWND hMCIWnd;

    switch( message )
    {
	case MCIWNDM_NOTIFYMODE:
	  switch (lParam) {
	  case MCI_MODE_STOP:
		MCIWndDestroy(hMCIWnd);
		bPlayingVideo = FALSE;
		display.SetAllDirty();
		break;
	  }

	  break;

    case WM_SIZE:
    case WM_MOVE:
		// All backbuffers need repainting:

		gameman.RestoreDisplay();

        if (IsIconic(hWnd))
        {
            Msg("WM: minimized, pausing");
            PauseGame();
			bWasIconic = TRUE;
        }

        if (display.IsFullscreen())
        {
			RECT rc;
            SetRect(&rcWindow, 0, 0, GAME_SX, GAME_SY);
            GetClientRect(hWnd, &rc);
			Msg("WM: fullscreen(%d, %d)", rc.right-rc.left, rc.bottom-rc.top);
            ClientToScreen(hWnd, (LPPOINT)&rc);
            ClientToScreen(hWnd, (LPPOINT)&rc+1);
			Msg("WM: fullscreen(%d, %d)", rc.right-rc.left, rc.bottom-rc.top);
        }
        else
        {
            GetClientRect(hWnd, &rcWindow);
            ClientToScreen(hWnd, (LPPOINT)&rcWindow);
            ClientToScreen(hWnd, (LPPOINT)&rcWindow+1);

			// Msg("size/move != fullscreen(%d, %d)", rcWindow.right-rcWindow.left, rcWindow.bottom-rcWindow.top);
        }
        break;

    case WM_ACTIVATEAPP:
		Msg("WM_ACTIVATEAPP(%d, %d)", rcWindow.right-rcWindow.left, rcWindow.bottom-rcWindow.top);
		bIsForeground = (BOOL)wParam && GetForegroundWindow() == hWnd;
        bIsActive = !IsIconic(hWnd) && bIsForeground;

        if (bIsActive)
		{
            Msg("WM: Azaria is active");
			if (bWasIconic)
			{
				display.ReInit();
				bWasIconic = FALSE;
			}
		}
        else
            Msg("WM: Azaria is not active");

        if (bIsForeground)
            Msg("WM: Azaria is Foreground");
        else
            Msg("WM: Azaria is not Foreground");

        //
        // while we were not-active something bad happened that caused us
        // to pause, like a surface restore failing or we got a palette
        // changed, now that we are active try to fix things
        //
        if (bPaused && bIsActive)
        {
            if (display.Restore(GetForegroundWindow() == hWnd))
            {
                UnPauseGame();
            }
        }
        break;

    case WM_PALETTECHANGED:
        //
        //  if another app changed the palette we dont have full control
        //  of the palette. NOTE this only applies for Maya in a window
        //  when we are fullscreen we get all the palette all of the time.
        //
        if ((HWND)wParam != hWnd)
        {
            if (!display.IsFullscreen())
            {
				Msg("WM: Lost control of palette.");
            }
        }
        break;

    case WM_QUERYNEWPALETTE:
		// All backbuffers need repainting:
		display.SetAllDirty();

        //
        //  we are getting the palette focus, select our palette
        //
        if (!display.IsFullscreen())
        {
			// set the palette on the front buffer
			display.AttachPalette();
        }
        break;

    case WM_DISPLAYCHANGE:
		// All backbuffers need repainting:
		gameman.RestoreDisplay();

		Msg("WM_DISPLAYCHANGE(%d, %d)", rcWindow.right-rcWindow.left, rcWindow.bottom-rcWindow.top);
        break;

    case WM_CREATE:
        break;

    case WM_SETCURSOR:
        break;

	case WM_NCMOUSEMOVE:
		p.x = -1;
		p.y = -1;
		userMouseMove(p);

		if (bCaptured)
		{
			ShowCursor(TRUE);
			bCaptured = FALSE;
		}
		break;

	case WM_CAPTURECHANGED:
		p.x = -1;
		p.y = -1;
		userMouseMove(p);
		break;
	case WM_MOUSEMOVE:
		if (!bCaptured)
		{
			bCaptured = TRUE;
			ShowCursor(FALSE);
		}
		userMouseMove(p);
		break;

	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		userLButtonDown(p);
		break;
	case WM_LBUTTONUP:
		userLButtonUp(p);
		break;

	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
		userRButtonDown(p);
		break;
	case WM_RBUTTONUP:
		userRButtonUp(p);
		break;

/*
	case WM_RASDIALEVENT:
		PostMessage(hWndMain, WM_CLOSE, 0, 0);
		break;
*/

    case WM_SYSKEYDOWN:
        switch( wParam )
        {
        // handle ALT+ENTER (fullscreen)
        case VK_RETURN:
            display.setFullscreen(!display.IsFullscreen());
			/* display.ReInit(); */
            // ExitGame();
			Music::Pause();
            display.Disable(TRUE);        // destroy DirectDraw object
            if (!display.Init(hWnd, GameMode, GameBPP))
			{
				errorCriticalStop("WM: Unable to restore game.");
			}
			else
			  Music::Resume();
            break;
		}
        break;

	case WM_CHAR:
	  userKeyPressed(wParam, FALSE);
	  break;

	case WM_KEYUP:
		userKeyReleased(wParam, TRUE);
		break;
        
    case WM_KEYDOWN:
		if (userKeyPressed(wParam, TRUE)) switch (wParam)
		{
		case VK_F1:
		  if (!bPlayingVideo)
		  {
			hMCIWnd=MCIWndCreate(hWnd, NULL, 
			  MCIWNDF_SHOWALL|MCIWNDF_NOTIFYMODE, "f:\\funstuff\\videos\\goodtime.avi");
			if (hMCIWnd)
			{
				LONG result = MCIWndPlay(hMCIWnd);
				MCIWndSetZoom(hMCIWnd, 200);
  				bPlayingVideo = TRUE;
			}
		  }
		  else
		  {
			MCIWndDestroy(hMCIWnd);
			bPlayingVideo = FALSE;
			display.SetAllDirty();
		  }
		  break;
		case VK_F2:
			display.PrintPalette();
			break;

        case VK_F3:
			gameman.RestoreDisplay();
            break;

        case VK_F4:
            // treat F4 like ALT+ENTER (fullscreen)
            PostMessage(hWnd, WM_SYSKEYUP, VK_RETURN, 0);
            break;

        case VK_F5:
			display.setExclusive(!display.IsExclusive());
			display.ReInit();
            break;

        case VK_F6:
			display.setExclusive(FALSE);
			display.setFullscreen(FALSE);
			display.ReInit();
            break;
		}
        break;

    case WM_PAINT:
        hdc = BeginPaint( hWnd, &ps );
        if (bPaused && !display.IsFullscreen())
        {
			Msg("WM: Painting during paused game");
        }
        EndPaint( hWnd, &ps );
        return 1;

    case WM_DESTROY:
		bIsActive = FALSE;
        hWndMain = NULL;
        PostQuitMessage( 0 );
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
} /* MainWndProc */

/*
 * initApplication
 *
 * Do that Windows initialization stuff...
 */
static BOOL initApplication( HANDLE hInstance, int nCmdShow )
{
    WNDCLASS    wc;
    BOOL        rc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, MAKEINTATOM(MAYA_ICON));
    wc.hCursor = NULL;
    wc.hbrBackground = GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "SingularClass";
    rc = RegisterClass( &wc );
    if( !rc )
    {
        return FALSE;
    }

    hWndMain = CreateWindow(
        "SingularClass",
         "Azaria",
        WS_VISIBLE    // so we dont have to call ShowWindow
		| WS_POPUPWINDOW
		| WS_CAPTION
		,
        GetSystemMetrics(SM_CXSCREEN)/2 - GameMode.cx/2,
        GetSystemMetrics(SM_CYSCREEN)/2 - GameMode.cy/2,
        GameMode.cx,
        GameMode.cy,
        NULL,
        NULL,
        hInstance,
        NULL );

    if( !hWndMain )
    {
        return FALSE;
    }

    UpdateWindow( hWndMain );
    SetFocus( hWndMain );

    return TRUE;

} /* initApplication */

static int MissingFileMessage(int buttons)
{
  int rc;
  char buff[MAX_PATH+128];

  sprintf(buff, 
	"Azaria cannot find a required file:\n%s\n%s\n%s\nYou may need to re-install Azaria",
	missing_file_name,
	missing_file_path2,
	missing_file_path1
	);

  Msg(buff);

  display.FlipToGdi(FALSE);
  rc = MessageBox(hWndMain, buff,"Missing File ...",buttons);
  display.FlipToGdi(TRUE);

  missing_file_path1[0]=0;
  missing_file_path2[0]=0;
  missing_file_name[0]=0;

  return rc;
}

/*
 * WinMain
 */
int PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine,
                        int nCmdShow )
{
    MSG                 msg;
	int rc;

    GameMode.cx = GAME_SX;
    GameMode.cy = GAME_SY;
    GameBPP = GAME_BPP;

	missing_file_path1[0]=0;
	missing_file_path2[0]=0;
	missing_file_name[0]=0;

    // create window and other windows things
    if( !initApplication(hInstance, nCmdShow) )
        return FALSE;

	try
	{
	  // Initialize the display manager and setup our window for the 640x480 game
	  if( !display.Init(hWndMain, GameMode, GameBPP))
		  return FALSE;

	  Wave::DirectSoundCreate(NULL);
	  Wave::SetCooperativeLevel( hWndMain,DSSCL_NORMAL );
	  Wave::Load(TBL_WAVE_COUNT, WavePath, AltWavePath, waveRamFileNames);

	  // start up the Game Manager.
	  if ( !gameman.Init() )
		goto FileError;
	}
	catch (long e)
	{
	  switch (e)
	  {
	  case ERROR_FILE_NOT_FOUND:
		MissingFileMessage(MB_ICONSTOP|MB_OK);
		break;
	  }
	  goto FileError;
	}

    while( 1 )
    {
        if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessage( &msg, NULL, 0, 0))
            {
                break;
            }
            TranslateMessage(&msg); 
            DispatchMessage(&msg);
        }
        else //if (bGameActive)
        {
		  POINT pos;
		  RECT wr;
		  GetCursorPos(&pos);
		  GetWindowRect(hWndMain, &wr);

		  if (!PtInRect(&wr, pos))
		  {
			pos.x = -1;
			pos.y = -1;
			userMouseMove(pos);
		  }

			if (bPostQuitMessage)
			{
			  PostMessage(hWndMain, WM_CLOSE, 0, 0);
			  bPostQuitMessage = FALSE;
			}

			try
			{
				if (((bIsActive && !bPaused) 
				  || (!display.IsExclusive() && !IsIconic(hWndMain)))
				  && !bPlayingVideo)
				{
					gameman.Tick(TRUE); // GDI is available
				}
				else
				{
					gameman.Tick(FALSE); // GDI is not available
				}
			}
			catch (long e)
			{
			  switch (e)
			  {
			  case ERROR_FILE_NOT_FOUND:
				rc = MissingFileMessage(MB_ICONEXCLAMATION|MB_RETRYCANCEL);
				if (rc == IDCANCEL)
					goto FileError;
				break;
			  default:
				goto FileError;
				break;
			  }
			}
        }
    }

//NormalExit:
	Msg("Normal Exit:");
	display.Disable(TRUE);        // destroy DirectDraw object
	netman.Close(FALSE);
	ShowCursor(TRUE);
	Wave::Release();
    return msg.wParam;

FileError:
	Msg("FileError Exit:");
	display.Disable(TRUE);        // destroy DirectDraw object
	netman.Close(FALSE);
	ShowCursor(TRUE);
	Wave::Release();
    return msg.wParam;

} /* WinMain */

#endif

