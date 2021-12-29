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

	  Wave::DirectSoundCreate(NULL);
	  Wave::SetCooperativeLevel( hWndMain, 0 /*DSSCL_NORMAL*/ );
        // TODO: Wave::Load(TBL_WAVE_COUNT, WavePath, AltWavePath, waveRamFileNames);

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

int WinLoop(void)
 {
     POINT pos;
     RECT wr;
     //GetCursorPos(&pos);
     //GetWindowRect(hWndMain, &wr);
     /*
     if (!sPtInRect(&wr, pos))
     {
         pos.x = -1;
         pos.y = -1;
         userMouseMove(pos);
     }
     */
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
                 //rc = MissingFileMessage(MB_ICONEXCLAMATION|MB_RETRYCANCEL);
                 //if (rc == IDCANCEL)
                 goto FileError;
                 break;
             default:
                 goto FileError;
                 break;
         }
     }

//NormalExit:
    // printf(".");
    return 0; //msg.wParam;

FileError:
	Msg("FileError Exit:");
	display.Disable(TRUE);        // destroy DirectDraw object
	netman.Close(FALSE);
	Wave::Release();
    return 0; //msg.wParam;

} /* WinMain */

SDL_Renderer* rend;
float x_scale=2;
float y_scale=2;

void HandleSdlEvent(SDL_Event &event) {
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

void __paint__(unsigned char *data) {
    // Set up the pixel format color masks for RGB(A) byte arrays.
    // Only STBI_rgb (3) and STBI_rgb_alpha (4) are supported here!
    Uint32 rmask, gmask, bmask, amask;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (req_format == STBI_rgb) ? 8 : 0;
    rmask = 0xff000000 >> shift;
    gmask = 0x00ff0000 >> shift;
    bmask = 0x0000ff00 >> shift;
    amask = 0x000000ff >> shift;
    #else // little endian, like x86
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0x00000000;
    #endif

    int width = 640;
    int height = 480;
    int depth=32;
    int pitch = 4*width;

    SDL_Surface* surf = SDL_CreateRGBSurfaceFrom((void*)data, width, height, depth, pitch,
                                             rmask, gmask, bmask, amask);

    if (surf == NULL) {
        SDL_Log("Creating surface failed: %s", SDL_GetError());
    }

	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf); 

    // ... do something useful with the surface ...
    // ...
	SDL_RenderCopy(rend, tex, NULL, NULL); 

    // when you don't need the surface anymore, free it..
    SDL_FreeSurface(surf);
   	SDL_DestroyTexture(tex); 

   	display.SetAllDirty();
}

int sdl2_main() 
{
    // retutns zero on success else non-zero 
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { 
		printf("error initializing SDL: %s\n", SDL_GetError()); 
	}

	SDL_Window* win = SDL_CreateWindow("AZARIA", // creates a window 
									SDL_WINDOWPOS_CENTERED, 
									SDL_WINDOWPOS_CENTERED, 
									640*x_scale, 480*y_scale, SDL_WINDOW_RESIZABLE); 

	rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); 

	// annimation loop 
	int close = 0; 
	while (!close) { 
		SDL_Event event; 

		// Events mangement 
		while (SDL_PollEvent(&event)) { 
			if (event.type == SDL_QUIT) {
				// handling of close button 
				close = 1; 
				break; 
            } else {
                HandleSdlEvent(event);
            }
		} 

		// clears the screen 
		SDL_RenderClear(rend); 
        WinLoop();

		// triggers the double buffers 
		// for multiple rendering 
		SDL_RenderPresent(rend); 

		// calculates to 60 fps 
		SDL_Delay(1000 / 120); 
	} 

	SDL_DestroyRenderer(rend); 
	SDL_DestroyWindow(win); 
	return 0; 
}

/*
; https://www.opengl.org/resources/libraries/glut/spec3/spec3.html
*/
int main(int argc, char** argv) {

    WinMain("/6tb/Home/Singularity/AzariaCD/DATA",
            "/6tb/Home/Singularity/AzariaCD961112/data");

    sdl2_main();
}