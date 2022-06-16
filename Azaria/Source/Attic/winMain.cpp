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

#include "Window.h"
#include "azaria.h"

#include "includes.h"
#include "stdapp.h"
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

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h> 
#include <SDL2/SDL_timer.h> 

static HWND		hWndMain;
RECT			rcWindow;
BOOL bWasIconic = FALSE;
BOOL			bIsActive;
BOOL			bPaused;
BOOL			bPlayingVideo;
BOOL			bPostQuitMessage = FALSE;
BOOL			bIsForeground;

bool IsIconic(HWND hWnd)
{
    return false;
}

static int MissingFileMessage(int buttons) {
  int rc=0;
  char buff[MAX_PATH+128];

  sprintf(buff, 
	"Azaria cannot find a required file:\n%s\n%s\n%s\nYou may need to re-install Azaria",
	missing_file_name,
	missing_file_path2,
	missing_file_path1);

  Msg(buff);
  return rc;
}

/*
 * WinMain
 */
int WinMain(const char * datapath, const char * altdatapath)
{
	missing_file_path1[0]=0;
	missing_file_path2[0]=0;
	missing_file_name[0]=0;

    gameman.InitPaths(datapath, altdatapath);

	try	{
	  // Initialize the display manager and setup our window for the 640x480 game
	  if( !display.Init())
		  return FALSE;

	  Wave::Init();

	  // start up the Game Manager.
	  if ( !gameman.Init() )
		goto FileError;
	} catch (int e)	{
	  switch (e) {
	  case ERROR_FILE_NOT_FOUND:
		MissingFileMessage(0);
		break;
	  }
	  goto FileError;
	}
    
    //NormalExit:
    Msg("Normal Exit:");

    return 0; //msg.wParam;
    
FileError:
    Msg("FileError Exit:");
    display.Disable(TRUE);        // destroy DirectDraw object
    netman.Close(FALSE);
    Wave::Release();
    return 0; //msg.wParam;    return 0;
}

void WinLoop(void)
 {
     POINT pos;
     RECT wr;
     if (bPostQuitMessage)
     {
         //PostMessage(hWndMain, WM_CLOSE, 0, 0);
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
     catch (int32_t e)
     {
         switch (e)
         {
             case ERROR_FILE_NOT_FOUND:
                 printf("%s:%d - Exception: ERROR_FILE_NOT_FOUND\n", __FILE__, __LINE__);
                 break;
             default:
                 printf("%s:%d - Exception(%d)\n", __FILE__, __LINE__, e);
                 break;
         }
     }
}

void HandleGameEvent(SDL_Event &event, float x_scale, float y_scale) {
    switch (event.type) {
        /* Keyboard events */
        case SDL_KEYDOWN:
            userKeyPressed(event.key.keysym.sym, false);
            break;
        case SDL_KEYUP:
            userKeyReleased(event.key.keysym.sym, true);
            break;
        case SDL_TEXTEDITING:
            // printf("SDL_TEXTEDITING\n");
            break;
        case SDL_TEXTINPUT:
            // printf("SDL_TEXTINPUT\n");
            break;
        case SDL_KEYMAPCHANGED:
            // printf("SDL_KEYMAPCHANGED\n");
            break;

        /* Mouse events */
        case SDL_MOUSEMOTION:
            {
                POINT p;
                p.x = event.motion.x/x_scale;
                p.y = event.motion.y/y_scale;
	            userMouseMove(p);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            {
                POINT p;
                p.x = event.button.x/x_scale;
                p.y = event.button.y/y_scale;
               	userMouseMove(p);
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
   		                userLButtonDown(p);
                        break;
                    case SDL_BUTTON_RIGHT:
   		                userRButtonDown(p);
                        break;
                }
                printf("SDL_MOUSEBUTTONDOWN((x:%d, y:%d)\n", p.x, p.y);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            {
                POINT p;
                p.x = event.button.x/x_scale;
                p.y = event.button.y/y_scale;
               	userMouseMove(p);
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
   		                userLButtonUp(p);
                        break;
                    case SDL_BUTTON_RIGHT:
   		                userRButtonUp(p);
                        break;
                }
                printf("SDL_MOUSEBUTTONUP(x:%d, y:%d)\n", p.x, p.y);
            }
            break;
        case SDL_MOUSEWHEEL:
            printf("SDL_MOUSEWHEEL\n");
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                x_scale = (float)(event.window.data1) / 640;
                y_scale = (float)(event.window.data2) / 480;
                printf("resized\n");
            }
            // printf("SDL_WINDOWEVENT\n");
            break;
        default:
            printf("SDL EVENT TYPE %x\n", event.type);
            break;
    }
}


int sdl2_main() 
{

    // retutns zero on success else non-zero 
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { 
		printf("error initializing SDL: %s\n", SDL_GetError()); 
	}

    gameWindow = new Window("AZARIA", 640, 480);

	// annimation loop 
    gameWindow->RunLoop(&HandleGameEvent, &WinLoop);

    delete gameWindow;
	return 0; 
}

/*
; https://www.opengl.org/resources/libraries/glut/spec3/spec3.html
*/
int main(int argc, char** argv) {
#if 0
    WinMain("/6tb/Home/Singularity/AzariaCD961112/DATA",
            "/6tb/Home/Singularity/AzariaCD961112/data");
#else
    WinMain("/6tb/Home/Singularity/AzariaCD2020/data",
            "/6tb/Home/Singularity/AzariaCD2020/data");
#endif            

    sdl2_main();
}